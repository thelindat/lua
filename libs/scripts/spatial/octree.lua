--[[
    3D Spatial Indexing: https://en.wikipedia.org/wiki/Octree

    This index does not require an explicit 'Build' function. However,
    Octree.EstimateParameters can be used to estimate parameters for the Octree
    that suits the dataset.

@NOTE:
    Designed using https://github.com/Nition/UnityOctree as a reference. This
    script is not, nor intended to be, a language-to-language translation.

@NOTE:
    Implemented to minimize the number of allocated Lua tables. The entire tree
    structure is linearised and stored in flat parallel arrays. One obvious
    downside to this approach is that the script is oblivious to array-growth
    algorithm implemented by ltable.c .

@TODO:
    (1) Optimize;
    (2) Experiment with removing recursion in :Add() and :Recycle();

@LICENSE
    See Copyright Notice in lua.h
--]]
local Octree

local vec3 = vec3
local vec4 = vec4

local table_remove = table.remove
local table_create = table.create or function() return {} end
local table_wipe = table.wipe or function() return {} end
local table_compact = table.compact or function(t) return t end

local glm = glm
local glm_aabb_contains = glm.aabb.contains
local glm_aabb_containsAABB = glm.aabb.containsAABB
local glm_aabb_distance = glm.aabb.distance
local glm_aabb_slabs = glm.aabb.slabs
local glm_aabb_intersectsRay = glm.aabb.intersectsRay
local glm_aabb_intersectsAABB = glm.aabb.intersectsAABB
local glm_aabb_intersectsSphere = glm.aabb.intersectsSphere
local glm_midpoint = function(x, y) return x + 0.5 * (y - x) end

-- "Infinity" may not be available in cases where the Octree is serialized;
-- Replace with some significantly large value.
local glm_huge <const> = glm.huge
local emptyBounds <const> = vec3(0)
local emptyCenter <const> = vec4(0)

Octree = setmetatable({
    DefaultLeafSize = 16, -- default leaf size.
    DefaultLoosenessFactor = 1.0,

    MaximumGrowIterations = 100,
    ChildDivision = { -- 4x2 subdivisions.
        vec3(-1.0,  1.0, -1.0),
        vec3( 1.0,  1.0, -1.0),
        vec3(-1.0,  1.0,  1.0),
        vec3( 1.0,  1.0,  1.0),
        vec3(-1.0, -1.0, -1.0),
        vec3( 1.0, -1.0, -1.0),
        vec3(-1.0, -1.0,  1.0),
        vec3( 1.0, -1.0,  1.0),
    },
}, {
    __call = function(_, ...)
        return Octree.New(...)
    end
})
Octree.__index = Octree

--[[
    Create a new Octree.

@PARAM position - initial location of the root node.
@PARAM leafSize - maximum number of objects a non-leaf node can manage.
@PARAM initialLength - initial axis length of the root node.
@PARAM minSize - minimum allowable axis length size, i.e., the maximum number of
    sub-divisions allowed at a specific point in space. This is an alternative
    to a "maximum allowable depth" constant. Note: minSize must be less than
    initialLength.
@PARAM looseness - A modifier ([1.0, 2.0]) for the behavior of elements that
    live on the edges of boundary nodes.
--]]
function Octree.New(position, leafSize, initialLength, minSize, looseness)
    local tree = setmetatable({
        --[[ Tree settings --]]
        leafSize = leafSize, -- Maximum number of objects within a node (unless a leaf)
        initialLength = initialLength, -- initial size of the root AABB
        minSize = glm.min(minSize, initialLength), -- minimum allowable size
        looseness = glm.clamp(looseness or 1.0, 1.0, 2.0), -- edge-overlap factor
        immutable = false,

        --[[ Data --]]
        minBounds = {}, maxBounds = {}, -- Object AABB

        --[[ Tree structure --]]
        root = -1,
        nodeCount = 1, -- Array/Index counter
        center = {}, -- vec4: Center of the node + axis length
        parent = {}, -- Cached reference to parent
        objects  = {}, -- Objects in this node

        -- [node] = {}: an array of indices corresponding to the eight
        -- potential subdivisions of node.
        children = {},

        -- Bounding box of each node. This value may not be equal to
        -- center +/ 0.5 * axisLength because the looseness factor modifies the
        -- behavior of elements that stride edges.
        nodeMinBounds = {}, nodeMaxBounds = {},

        --[[ Cached Structures. --]]

        -- Precomputed structure that holds whether a node, or any of its
        -- children, have objects. This is used to simplify searching and
        -- compression of the tree.
        hasObjects = {},

        -- A master table that converts an object identifier to a node index.
        -- This structure is an optimization for Remove() so the tree does not
        -- require a complete DFS to delete a single object.
        indexLookup = {},

        -- available/recycled indices within the (flat-)array tree structure
        availableIndices = {},
    }, Octree)

    tree.root = tree:NewNode(-1, position, tree.initialLength)
    return tree
end

--[[
    Estimate Octree parameters for a given (AABB) dataset.

@TODO: This function can be improved.
--]]
function Octree.EstimateParameters(minBounds, maxBounds)
    local mid,width = vec3(0),vec3(0)
    local minPos,maxPos = vec3(glm_huge),vec3(-glm_huge)
    local minDim,maxDim = vec3(glm_huge),vec3(-glm_huge)

    local npts = #minBounds
    assert(#minBounds > 0) -- @TODO: Return default <centroid, axis, minSize>
    for i=1,npts do
        local min = minBounds[i]
        local max = (maxBounds and maxBounds[i]) or nil

        local point = (max and glm_midpoint(min, max)) or min
        mid = mid + point
        minPos = glm.min(minPos, point)
        maxPos = glm.max(maxPos, point)

        if max then -- An AABB
            local dimensions = max - min
            width = width + dimensions
            minDim = glm.min(minDim, dimensions)
            maxDim = glm.max(maxDim, dimensions)
        end
    end

    -- @TODO: The above approach to compute the centroid is not well-suited for
    -- large datasets. In the meantime, ensure no values are infinite (or NaN?).
    local centroid = (mid / npts) + (width / npts)
    if glm.any(glm.isinf(centroid)) then
        centroid = vec3(0)
    end

    if maxBounds and not glm.equal(0.0, glm.compMin(minDim), glm.feps) then -- At least some AABBs exist
        return centroid, -- middle point
            0.5 * glm.compMax(maxDim - minDim), -- initAxis
            0.25 * glm.compMin(minDim) -- minSize
    else -- Otherwise, assume all points.
        return centroid,
            0.5 * glm.compMax(maxPos - minPos),
            0.01 * glm.compMin(maxPos - minPos)
    end
end

--[[ @OVERRIDE --]]
function Octree:Bounds(object)
    return self.minBounds[object],self.maxBounds[object]
end

--[[ @OVERRIDE --]]
function Octree:Clear()
    --[[ Data --]]
    self.minBounds = table_wipe(self.minBounds)
    self.maxBounds = table_wipe(self.maxBounds)

    --[[ Tree structure --]]
    self.root = nil
    self.nodeCount = 1
    self.center = table_wipe(self.center)
    self.parent = table_wipe(self.parent)
    self.objects = table_wipe(self.objects)
    self.children = table_wipe(self.children)
    self.nodeMinBounds = table_wipe(self.nodeMinBounds)
    self.nodeMaxBounds = table_wipe(self.nodeMaxBounds)

    --[[ Cached Structures. --]]
    self.hasObjects = table_wipe(self.hasObjects)
    self.indexLookup = table_wipe(self.indexLookup)
    self.availableIndices = table_wipe(self.availableIndices)

    return self
end

--[[ @OVERRIDE --]]
function Octree:Compact()
    self.center = table_compact(self.center)
    self.parent = table_compact(self.parent)
    self.objects = table_compact(self.objects)
    self.children = table_compact(self.children)
    self.nodeMinBounds = table_compact(self.nodeMinBounds)
    self.nodeMaxBounds = table_compact(self.nodeMaxBounds)
    return self
end

----------------------------------------
----------- Flat Octree Node -----------
----------------------------------------

--[[ Find which child node the object would be most likely to fit in. --]]
function Octree:BestChildBounds(node, minBound, maxBound)
    local center = self.center[node]
    local objCenter = (maxBound and glm_midpoint(minBound, maxBound)) or minBound
    return 1
        + ((objCenter[1] <= center[1] and 0) or 1)
        + ((objCenter[2] >= center[2] and 0) or 4)
        + ((objCenter[3] <= center[3] and 0) or 2)
end

--[[
    Create a new node centered at the specified position with a parent node and
    axis length; recycling where possible.

@RETURN
    The index of the newly created node.
--]]
function Octree:NewNode(parent, position, axisLength)
    local node = nil
    if #self.availableIndices > 0 then
        node = table_remove(self.availableIndices)
    else
        node = self.nodeCount
        self.nodeCount = self.nodeCount + 1
    end

    self.hasObjects[node] = false
    self.parent[node] = parent
    self.objects[node] = self.objects[node] or {}
    self.children[node] = self.children[node] or {}
    self:SetNodeLocation(node, position, axisLength)
    return node
end

--[[
    Set the node location and dimensions.

@NOTE Assume that 'node' has no children, as that would require recursively
    updating its location relative to this new position.
--]]
function Octree:SetNodeLocation(node, position, axisLength)
    local adjLength = 0.5 * self.looseness * axisLength
    self.center[node] = vec4(position.xyz, axisLength)
    self.nodeMinBounds[node] = position - adjLength -- Bounding box of this node
    self.nodeMaxBounds[node] = position + adjLength
    --if #self.children[node] > 0 then
    --    error("unexpected child count")
    --end
    return self
end

--[[
    Recycle the entire subtree rooted at the 'node' and clear everything.
    All nodes indices are dumped into 'availableIndices' for recycling.

@NOTE: callers of this function must manage/cleanup the objects contained in
    'indexLookup'.
--]]
function Octree:Recycle(node)
    local children = self.children[node]
    for i=#children,1,-1 do -- Recursively recycle all children...
        if children[i] ~= 0 then
            self:Recycle(children[i])
        end
    end

    self.children[node] = table_wipe(self.children[node])
    self.objects[node] = table_wipe(self.objects[node])
    self.availableIndices[#self.availableIndices + 1] = node

    self.parent[node] = 0
    self.center[node] = emptyCenter
    self.nodeMinBounds[node] = emptyBounds
    self.nodeMaxBounds[node] = emptyBounds
    self.hasObjects[node] = false
    return self
end

-------------------------
-- Splitting & Merging --
-------------------------

--[[
@RETURN
    true if 'node' is a leaf, i.e., it can no longer be subdivided without
    breaking the limits specified by the tree.
--]]
function Octree:IsLeaf(node) return (self.center[node][4] * 0.5) < self.minSize end

--[[ Populate the children list of 'node'. --]]
function Octree:Split(node)
    local divisions = Octree.ChildDivision
    local children,center = self.children[node],self.center[node]

    local center3 = center.xyz
    local newLen,quarter = center[4] * 0.5,center[4] * 0.25
    children[1] = self:NewNode(node, center3 + (divisions[1] * quarter), newLen)
    children[2] = self:NewNode(node, center3 + (divisions[2] * quarter), newLen)
    children[3] = self:NewNode(node, center3 + (divisions[3] * quarter), newLen)
    children[4] = self:NewNode(node, center3 + (divisions[4] * quarter), newLen)
    children[5] = self:NewNode(node, center3 + (divisions[5] * quarter), newLen)
    children[6] = self:NewNode(node, center3 + (divisions[6] * quarter), newLen)
    children[7] = self:NewNode(node, center3 + (divisions[7] * quarter), newLen)
    children[8] = self:NewNode(node, center3 + (divisions[8] * quarter), newLen)
    return self
end

--[[
    Update the 'hasObjects' cache for a specific node. Its assumed this tree is
    constructed from the leaves up (requires no DFS to update caches).

    'Node' has objects if either (1) it has objects (obviously); or (2) one of
    its children has objects: hasObjects[child] == true.
--]]
function Octree:UpdateObjectCache(node)
    local hasObjects = self.hasObjects
    if #self.objects[node] > 0 then
        hasObjects[node] = true
    else
        local hasObjs = false
        local children = self.children[node]
        for i=1,#children do
            hasObjs = hasObjs or hasObjects[children[i]]
        end
        hasObjects[node] = hasObjs
    end
    return self
end

--[[
    Determine if the children of 'node' can be merged into 'node'. This is
    possible if: (1) None of the children of 'node' have children; and (2) The
    total number of objects among 'node' and its children is less than the
    configured leaf size of the tree.

@RETURN
    true if the children of this node can be merged into the node; false ow.
--]]
function Octree:CanMerge(node)
    local children,objects = self.children,self.objects

    local totalObjects = #objects[node]
    local nodeChildren = children[node]
    for i=1,#nodeChildren do
        local child = nodeChildren[i]
        if #children[child] > 0 then -- has children
            return false
        end

        totalObjects = totalObjects + #objects[child]
    end

    return totalObjects <= self.leafSize
end

--[[
    Merge all children of 'node' into itself. See the conditions to CanMerge.

@TODO:
    Since "children" is a single array holding arrays of size 8, contemplate
    recycling that table versus throwing it into the void. At the moment the
    array is recycled.
--]]
function Octree:Merge(node)
    local indexLookup,objects = self.indexLookup,self.objects
    local children = self.children[node]

    local nodeObjects = objects[node]
    for i=#children,1,-1 do
        local childObjects = objects[children[i]]
        for j=1,#childObjects do
            local object = childObjects[j]

            nodeObjects[#nodeObjects + 1] = object
            indexLookup[object] = node
        end

        childObjects = table_wipe(childObjects)
        self:Recycle(children[i]) -- Allow the child node to be re-used elsewhere.
    end

    self.children[node] = table_wipe(children)
    return self:UpdateObjectCache(node)
end

--[[
    For the given node determine if it, or any of its hierarchic parents, can
    merge its children.
--]]
function Octree:MergeParentHierarchy(node)
    while node ~= -1 and self:CanMerge(node) do
        self:Merge(node)
        node = self.parent[node]
    end
    return self
end

---------------------------------------
------------ Modifications ------------
---------------------------------------

local function CouldNotGrowError()
    error(
        "Insert operation has exceeded the number of successive grow " ..
        "operations. Its likely the data does not match the configuration " ..
        "supplied to the tree on creation"
    )
end

--[[
    Given the delta vector (i.e., difference between the element to be added and
    current root), shift root center by half of its length and then double its
    bounds.

    This ensures the new bounds still contain the root bounds while growing in
    the direction that will encompass the object to be added.
--]]
function Octree:ShiftAndGrow(direction)
    local oldRoot = self.root
    local oldCenter = self.center[oldRoot]
    local axisLength = oldCenter[4]

    local half = 0.5 * axisLength
    local position = oldCenter.xyz + glm.signP(direction) * half

    -- Expand the root node by twice its length.
    local root = self:NewNode(-1, position, 2 * axisLength)

    self.root = root
    if self.hasObjects[oldRoot] then
        local center = self.center[root]
        local rootAsChildIndex = 0
            + ((oldCenter[1] <= center[1] and 0) or 1)
            + ((oldCenter[2] >= center[2] and 0) or 4)
            + ((oldCenter[3] <= center[3] and 0) or 2)

        -- Create the octet, with the previous root as a child of the new root.
        local children = self.children[root]
        for i=0,7 do
            if i == rootAsChildIndex then
                children[i + 1] = oldRoot

                self.parent[oldRoot] = root
                if self:UpdateObjectCache(oldRoot):CanMerge(oldRoot) then
                    self:Merge(oldRoot)
                end
            else
                local qx = (i % 2 == 0 and -1) or 1
                local qy = (i > 3 and -1) or 1
                local qz = ((i < 2 or (i > 3 and i < 6)) and -1) or 1
                local q = vec3(qx, qy, qz)
                children[i + 1] = self:NewNode(root, position + half * q, axisLength)
            end
        end

        self:UpdateObjectCache(root)
    elseif oldRoot then
        self:MergeParentHierarchy(oldRoot):Recycle(oldRoot)
    end
    return root
end

--[[
    An inverse ShiftAndGrow: attempt to reduce the root node and its dimensions
    while enclosing all objects within the node.
--]]
function Octree:ShrinkAndShift()
    local root = self.root
    if not root then return end

    local rootChild,rootChildNode = self:CompressNode(root)
    if rootChild then
        -- The root doesn't have any children, however, CompressNode tells us
        -- that its possible to shrink the dimensions of the root. Update its
        -- dimensions.
        if #self.children[root] == 0 then
            local center = self.center[rootChild]
            self:SetNodeLocation(root, center, 0.5 * center[4])

        -- New root: set the root as the compressed child and recycle the
        -- previous root.
        elseif root ~= rootChild then
            self.root = rootChild
            self.parent[rootChild] = -1
            self.children[root][rootChildNode] = 0

            self:Recycle(root, true)
        end
    end
end

--[[ @OVERRIDE --]]
function Octree:Rebuild()
    self.immutable = false
    return self
end

--[[ @OVERRIDE --]]
function Octree:Immutable()
    self.immutable = true
    return self
end

--[[ @OVERRIDE --]]
function Octree:Insert(object, min, max)
    max = max or min
    if self.immutable then
        error("Tree is immutable")
    end

    local root = self.root
    local nodeMin,nodeMax = self.nodeMinBounds,self.nodeMaxBounds
    local objCenter = glm_midpoint(min, max)

    -- If the object bounds cannot be added to the root, then continually expand
    -- the root node until it can.
    local count = 0
    while not root or not glm_aabb_containsAABB(nodeMin[root], nodeMax[root] or nodeMin[root], min, max) do
        self:ShiftAndGrow(objCenter - self.center[root].xyz)

        count = count + 1
        root = self.root
        if count > Octree.MaximumGrowIterations then
            CouldNotGrowError()
            return false
        end
    end

    self.minBounds[object] = min
    self.maxBounds[object] = max
    self:Add(root, object)
    return true
end

--[[ @OVERRIDE --]]
function Octree:InsertPoint(object, point)
    return self:Insert(object, point, nil)
end

--[[ @OVERRIDE --]]
function Octree:Remove(object, shrink)
    if self.immutable then
        error("Tree is immutable")
    end

    -- The node that 'object' belongs to is cached: remove 'object' from the
    -- node and determine if the node can be destroyed/merged into its parent.
    local node = self.indexLookup[object]
    if node then
        local objects = self.objects[node]
        for i=1,#objects do
            if objects[i] == object then
                table_remove(objects, i)

                self.indexLookup[object] = nil
                self:UpdateObjectCache(node, true):MergeParentHierarchy(node, true)
                if shrink then
                    self:ShrinkAndShift()
                end

                return true
            end
        end
    end
    return false
end

--[[
    Add an object into the (sub-)tree rooted at 'node'. Creating new children on
    the fly to satisfy the constraints of the tree.
--]]
function Octree:Add(node, objectToAdd)
    local minBounds,maxBounds,nodeMinBounds,nodeMaxBounds =
        self.minBounds,self.maxBounds,self.nodeMinBounds,self.nodeMaxBounds

    local objects,children = self.objects[node],self.children[node]
    if #children == 0 then -- 'node' is a leaf;

        -- (1) Leaf slots in the given node available; or (2) the node can no
        -- longer be sub-divided without exceeding some minimum size threshold.
        if #objects < self.leafSize or (self.center[node][4] * 0.5) < self.minSize then
            objects[#objects + 1] = objectToAdd
            self.indexLookup[objectToAdd] = node

            return self:UpdateObjectCache(node) -- No children.
        end

        -- Subdivide the current node into eight children quadrants.
        self:Split(node)

        -- (Re-)categorize: For a given object find which quadrant its centroid
        -- would fall into. Then determine if that quadrant completely contains
        -- the objects bounds.
        for i=#objects,1,-1 do
            local object = objects[i]
            local objMin = minBounds[object]
            local objMax = maxBounds[object] or objMin

            local bestChild = children[self:BestChildBounds(node, objMin, objMax)]
            if glm_aabb_containsAABB(nodeMinBounds[bestChild], nodeMaxBounds[bestChild], objMin, objMax) then
                table_remove(objects, i)
                self.indexLookup[object] = nil

                self:Add(bestChild, object)
            end
        end
    end

    -- Insert the object
    local objMin = minBounds[objectToAdd]
    local objMax = maxBounds[objectToAdd] or objMin

    local objNode = children[self:BestChildBounds(node, objMin, objMax)]
    if glm_aabb_containsAABB(nodeMinBounds[objNode], nodeMaxBounds[objNode], objMin, objMax) then
        self:Add(objNode, objectToAdd)
    else
        objects[#objects + 1] = objectToAdd
        self.indexLookup[objectToAdd] = node
    end

    return self:UpdateObjectCache(node)
end

--[[
    The root node can be compressed (shrunk) if either:
        (1) all of its objects can be placed within one of its children; or
        (2) the number of children with objects is <= 1.
--]]
function Octree:CompressNode(node)
    local hasObjects,minBounds,maxBounds,nodeMinBounds,nodeMaxBounds =
        self.hasObjects,self.minBounds,self.maxBounds,
        self.nodeMinBounds,self.nodeMaxBounds

    local objects,children = self.objects[node],self.children[node]
    if self.center[node][4] < 2 * self.minSize then
        return nil -- The nodes axis length >= twice the minimum length.
    elseif #objects == 0 and #children == 0 then
        return nil -- The node has nothing to compress
    end

    -- Determine if all objects can be placed within a child
    local bestNode = 0
    for i=1,#objects do
        local object = objects[i]
        local objMin = minBounds[object]
        local objMax = maxBounds[object] or objMin

        -- Ensure the object is contained completely within the child. Any
        -- overlap across multiple children disqualifies compression.
        local bestChild = children[self:BestChildBounds(node, objMin, objMax)]
        if i == 1 or dest == bestNode then
            if glm_aabb_containsAABB(nodeMinBounds[bestChild], nodeMaxBounds[bestChild], objMin, objMax) then
                bestNode = bestChild
            else
                return nil
            end
        end
    end

    -- Using the 'hasObjects' cache, look through each child node to determine
    -- whether at most one of them has objects (and its equal to bestNode) and
    -- all others are empty.
    local hasContent = false
    for i=1,#children do
        if hasObjects[children[i]] then
            if hasContent or (bestNode > 0 and bestNode ~= i) then
                return nil -- Multiple children have objects, cannot compress.
            end

            hasContent = true
            bestNode = i -- if bestNode == 0
        end
    end
    return children[bestNode],bestNode
end

---------------------------------------
--------------- Queries ---------------
---------------------------------------

--[[ @OVERRIDE --]]
function Octree:CreateQueryCache()
    return { -- Recyclable/Reused stack structure
        stack = setmetatable(table_create(32, 0), { __mode = "v" }),
        rootbfs = setmetatable(table_create(1, 0), { __mode = "v" }),
    }
end

--[[ @OVERRIDE --]]
function Octree:Each(yield)
    local objects = self.objects
    for i=1,self.nodeCount-1 do
        local list = objects[i]
        if list then
            for j=1,#list do
                yield(list[j])
            end
        end
    end
end

--[[
    Generalized stack query (breadth-first search).

@PARAM arg0/arg1 are explicitly defined to avoid the overhead of TK_DOTS; arg1
    should be ignored for glm_aabb_contains anyway. A few hundred nanoseconds
    can be saved (per-query) by making the query 'less' generic.

@TODO Process the root independent of all other nodes.
--]]
function Octree:GenericQuery(cache, F, arg0, arg1, yield)
    if not self.root then return end
    local objects,children,hasObjects,minBounds,maxBounds,nodeMin,nodeMax =
        self.objects,self.children,self.hasObjects,
        self.minBounds,self.maxBounds,self.nodeMinBounds,self.nodeMaxBounds

    local pointer = 1
    local stack,rootbfs = cache.stack,cache.rootbfs

    rootbfs[1] = self.root ; stack[1] = rootbfs
    while pointer > 0 do
        local childList = stack[pointer] ; stack[pointer] = nil ; pointer = pointer - 1
        for i=1,#childList do
            local node = childList[i]
            if F(nodeMin[node], nodeMax[node], arg0, arg1) then
                local objs,childs = objects[node],children[node]
                for i=1,#objs do
                    local object = objs[i]
                    if F(minBounds[object], maxBounds[object] or minBounds[object], arg0, arg1) then
                        yield(object)
                    end
                end

                if hasObjects[node] and #childs > 0 then
                    pointer = pointer + 1 ; stack[pointer] = childs
                end
            end
        end
    end
end

--[[ @OVERRIDE --]]
function Octree:Query(cache, point, yield)
    self:GenericQuery(cache, glm_aabb_contains, point, nil, yield)
end

--[[ @OVERRIDE --]]
function Octree:Raycast(cache, origin, direction, yield)
    self:GenericQuery(cache, glm_aabb_intersectsRay, origin, direction, yield)
end

function Octree:Slabs(cache, origin, direction, yield)
    self:GenericQuery(cache, glm_aabb_slabs, origin, 1.0 / direction, yield)
end

--[[ @OVERRIDE --]]
function Octree:Colliding(cache, colMin, colMax, yield)
    self:GenericQuery(cache, glm_aabb_intersectsAABB, colMin, colMax, yield)
end

--[[ @OVERRIDE --]]
function Octree:SphereIntersection(cache, origin, radius, yield)
    self:GenericQuery(cache, glm_aabb_intersectsSphere, origin, radius, yield)
end

--[[ @OVERRIDE --]]
local RecursiveNeighborSearch = nil
function Octree:NearestNeighbors(_, point, neighborList)
    if neighborList.maxSize < 1 then
        return neighborList
    end

    return RecursiveNeighborSearch(self, self.root, point, neighborList)
end

--[[ @TODO: Improve... incredibly inefficient --]]
RecursiveNeighborSearch = function(self, node, point, neighborList)
    local minBounds = self.minBounds
    local objects = self.objects[node]

    -- Compute the distance between the point and object points
    for i=1,#objects do
        neighborList:Insert(objects[i], #(point - minBounds[objects[i]]))
    end

    -- For each child quadrant (sorted by distance to 'point'), only iterate
    -- through its subtree if:
    --  (1) the neighborList isn't full; or
    --  (2) the distance between the child quadrant and the point is less than
    --      the worst neighbor distance.
    --
    -- This approach is incredibly inefficient: other Octree/Quadtree solutions
    -- will use some MaxHeap implementation to auto-magically sort the next
    -- closest quadrants. This solution brute-forces that information.
    local children = self.children[node]
    if #children > 0 then
        local nodeMin,nodeMax = self.nodeMinBounds,self.nodeMaxBounds

        local visited = 0
        local continue = true
        while continue do
            local nextBest,nextDist = 0,glm_huge
            for i=1,8 do
                local childNode = children[i]
                if ((visited >> i) & 1) == 0 then
                    local distance = glm_aabb_distance(nodeMin[childNode], nodeMax[childNode], point)
                    if distance < nextDist then
                        nextBest = i
                        nextDist = distance
                    end
                end
            end

            if nextBest > 0 and (
                #neighborList.data < neighborList.maxSize
                or (nextDist <= (neighborList.worstDist + glm.feps))
            ) then
                RecursiveNeighborSearch(self, children[nextBest], point, neighborList)
                visited = visited | (1 << nextBest) -- mark as visited
            else
                continue = false
            end
        end
    end

    return neighborList
end

----------------------------------------
----------- Statistics/Debug -----------
----------------------------------------

local IndentString = "  "
local function OutputSubtree(self, lines, node, indent)
    local nextIndent = indent .. IndentString
    local center = self.center[node]
    local objects = self.objects[node]
    local minBnd = self.nodeMinBounds[node]
    local maxBnd = self.nodeMaxBounds[node]

    lines[#lines + 1] = ("%s[%d] = {"):format(indent, node)
    lines[#lines + 1] = ("%sCenter : %s,"):format(nextIndent, tostring(center.xyz))
    lines[#lines + 1] = ("%sAxis   : %f,"):format(nextIndent, center[4])
    lines[#lines + 1] = ("%sSize   : %s,"):format(nextIndent, tostring(maxBnd - minBnd))

    if #objects > 0 then
        lines[#lines + 1] = ("%sObjects: {"):format(nextIndent)
        for i=1,#objects do
            lines[#lines + 1] = ("%s%s%d: Min = %s, Max = %s"):format(
                nextIndent, IndentString, objects[i],
                tostring(self.minBounds[objects[i]]),
                tostring(self.maxBounds[objects[i]])
            )
        end
        lines[#lines + 1] = ("%s}"):format(nextIndent)
    end

    local children = self.children[node]
    if #children > 0 then
        lines[#lines + 1] = ("%sChildren: {"):format(nextIndent)
        for i=1,#children do
            local child = children[i]
            OutputSubtree(self, lines, child, nextIndent .. IndentString)
        end
        lines[#lines + 1] = ("%s}"):format(nextIndent)
    end
    lines[#lines + 1] = ("%s}"):format(indent)
end

--[[ @OVERRIDE --]]
function Octree:Output(output)
    if self.root then
        local lines = {}
        OutputSubtree(self, lines, self.root, "")
        output(table.concat(lines, "\n"))
    end
    return self
end

return Octree
