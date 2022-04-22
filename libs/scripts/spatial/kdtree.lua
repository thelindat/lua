--[[
    3D Spatial Indexing: https://en.wikipedia.org/wiki/K-d_tree

    This index depends upon the 'Interval' script for generating and maintaining
    sorted value lists used to select axes/points for sub-space division. After
    Build(intervals) is invoked, the KdTree will maintain a reference to that
    'Interval' object until Immutable is invoked.

    To build a KdTree:

        -- Setup intervals and partitioning strategy
        local intervals = Interval(Interval.SurfaceAreaHeuristic())
        for i=1,#minBounds do
            intervals:AppendBounds(i, minBounds[i], maxBounds[i])
        end

        -- Create a tree
        local tree = KDTree():Build(intervals):Immutable()

        -- Create a cache for querying (e.g., DFS stack)
        local cache = tree:CreateQueryCache()

        -- Querying the tree:
        tree:Query(cache, coordinate, function(object)
            -- Do something with object ('i' above).
        end)

        tree:Raycast(cache, rayOrigin, rayDir, function(object)
            -- Do something with object
            -- @TODO: Expose parametric intersection values in yield.
        end)

    See 'notes.txt' for a list of operations

@NOTE:
    Implemented to minimize the number of allocated Lua tables. The entire tree
    structure is linearised and stored in flat parallel arrays. One obvious
    downside to this approach is that the script is oblivious to array-growth
    algorithm implemented by ltable.c .

@TODO:
    (1) Reduce bloat!

        For large datasets, e.g., 148225 uniformly distributed AABB's, the index
        structure is 27MB, while requiring 251.02MB to build.

        This equates to costing roughly ~190 bytes per object. This can and
        should be improved by taking Lua internals into consideration. Moreover,
        integer vector support (see README) would be able reduce 'waste': see
        'cardinalValues' and apply it to the integer fields. These numbers are
        still quite respectable for a pure Lua implementation.

        The current Split() building algorithm is incredibly greedily and the
        memory overhead can be reduced within 'sat.lua'. Dealing with this is
        low priority as its a one-time cost.

    (2) KDTree:RemoveNode can be improved

@LICENSE
    See Copyright Notice in lua.h
--]]
local KDTree
local Interval = Interval or require('sat')

local vec3 = vec3

local table_remove = table.remove
local table_create = table.create or function() return {} end
local table_wipe = table.wipe or function() return {} end
local table_compact = table.compact or function(t) return t end

local glm = glm
local glm_abs = glm.abs
local glm_feps = glm.feps
local glm_min = glm.min
local glm_max = glm.max
local glm_mix = glm.mix
local glm_aabb_contains = glm.aabb.contains
local glm_aabb_encloseAABB = glm.aabb.encloseAABB
local glm_aabb_slabs = glm.aabb.slabs
local glm_aabb_intersectsRay = glm.aabb.intersectsRay
local glm_aabb_intersectsAABB = glm.aabb.intersectsAABB
local glm_aabb_intersectsSphere = glm.aabb.intersectsSphere

-- "Infinity" may not be available in cases where the K-d Tree is serialized;
-- Replace with some significantly large value.
local glm_huge <const> = glm.huge
local emptyBounds <const> = vec3(0)

KDTree = setmetatable({
    DefaultLeafSize = 16, -- Maximum number of objects per leaf node.
    DefaultIntervalPooler = 64, -- Maximum size of the table pooler used when building the KDTree.
}, {
    __call = function(_, ...)
        return KDTree.New(...)
    end
})
KDTree.__index = KDTree

--[[ Initialize an empty KdTree instance --]]
function KDTree.New()
    return setmetatable({
        leafSize = KDTree.DefaultLeafSize,
        modificationCount = 0, -- Number of mutable operations applied to tree.

        --[[ Interval structure --]]
        intervals = nil, minBounds = nil, maxBounds = nil,

        --[[ Tree structure --]]
        root = nil, -- Root index of the tree.
        nodeCount = 1, -- Index count
        cardinalAxis = {}, -- Specifies along which axis this node is split.
        low = {}, high = {}, -- Partitions/Children

        -- Shared vector:
        --  'x' - cardinalValue: the position along the cardinal axis of the split;
        --  'y' - lowMax: highest value on the cardinal axis in the 'lower' split;
        --  'z' - highMin: smallest value on the cardinal axis  in the 'higher' plane.
        cardinalValues = {},

        -- Accumulative bounds of this node: union of all child bounds.
        nodeMinBounds = {}, nodeMaxBounds = {},

        --[[ Leaf structure. --]]
        leafCount = 1, --  Flat-Index Count.
        objects = {}, -- Object indices being captured by a leaf at a specified index.
        leafMinBounds = {}, leafMaxBounds = {}, -- Accumulative bounds of all objects within the leaf.

        --[[ Cache structures --]]
        availableNodeIndices = {}, -- available nodes within the (flat-)array tree structure.
        availableLeafIndices = {}, -- available leafs within the (flat-)array tree structure.
    }, KDTree)
end

--[[ @OVERRIDE --]]
function KDTree:Bounds(object) return self.minBounds[object],self.maxBounds[object] end

--[[ @OVERRIDE --]]
function KDTree:Clear()
    self.modificationCount = 0

    --[[ Interval structure --]]
    self.intervals = nil
    self.minBounds,self.maxBounds = nil,nil

    --[[ Tree structure --]]
    self.root = nil
    self.nodeCount = 1
    self.cardinalAxis = table_wipe(self.cardinalAxis)
    self.cardinalValues = table_wipe(self.cardinalValues)
    self.low = table_wipe(self.low)
    self.high = table_wipe(self.high)
    self.nodeMinBounds = table_wipe(self.nodeMinBounds)
    self.nodeMaxBounds = table_wipe(self.nodeMaxBounds)

    --[[ Leaf structure. --]]
    self.leafCount = 1
    self.objects = table_wipe(self.objects)
    self.leafMinBounds = table_wipe(self.leafMinBounds)
    self.leafMaxBounds = table_wipe(self.leafMaxBounds)

    --[[ Cached Data.--]]
    self.availableNodeIndices = table_wipe(self.availableNodeIndices)
    self.availableLeafIndices = table_wipe(self.availableLeafIndices)

    return self
end

--[[ @OVERRIDE --]]
function KDTree:Compact()
    --[[ Tree structure --]]
    self.cardinalAxis = table_compact(self.cardinalAxis)
    self.cardinalValues = table_compact(self.cardinalValues)
    self.low = table_compact(self.low)
    self.high = table_compact(self.high)
    self.nodeMinBounds = table_compact(self.nodeMinBounds)
    self.nodeMaxBounds = table_compact(self.nodeMaxBounds)

    --[[ Leaf structure. --]]
    self.objects = table_compact(self.objects)
    self.leafMinBounds = table_compact(self.leafMinBounds)
    self.leafMaxBounds = table_compact(self.leafMaxBounds)

    return self
end

--[[ Return an AABB that encloses all geometry in this KDTree --]]
function KDTree:BoundAABB()
    local root = self.root
    if not root then
        return nil,nil
    elseif root < 0 then
        return self.leafMinBounds[root],self.leafMaxBounds[root]
    end

    return self.nodeMinBounds[root],self.nodeMaxBounds[root]
end

----------------------------------------
----------- Flat KDTree Node -----------
----------------------------------------

--[[
    Helper function for converting a node index (negative for leafs) into the
    minimum component of its AABB.
--]]
local function MinBound(self, node)
    return (node < 0 and self.leafMinBounds[-node]) or self.nodeMinBounds[node]
end

--[[
    Helper function for converting a node index into the maximum component of
    its AABB.
--]]
local function MaxBound(self, node)
    return (node < 0 and self.leafMaxBounds[-node]) or self.nodeMaxBounds[node]
end

--[[
    Create a simple pooling class to allow array recycling when recursively
    splitting array axes.
--]]
local function CreateIntervalPooler(poolLimit)
    local limit = poolLimit or KDTree.DefaultIntervalPooler
    return {
        limit = limit,
        objectPool = table_create(limit, 0),

        New = function(self) return { {},{},{} } end,

        Recycle = function(self, t)
            t[1] = table_wipe(t[1])
            t[2] = table_wipe(t[2])
            t[3] = table_wipe(t[3])
            return t;
        end,

        Release = function(self, t)
            local pool = self.objectPool
            if #pool < self.limit then
                pool[#pool + 1] = t
            end
        end,

        Pool = function(self)
            local pool = self.objectPool
            if #pool == 0 then
                return self:New()
            else
                local tuple = pool[#pool] ; pool[#pool] = nil
                return self:Recycle(tuple)
            end
        end,
    }
end

--[[
    Construct a new intermediate tree node.

@PARAM axis - the separating axis used to define the low/mid/high nodes
@PARAM value - the separating axis value.
@PARAM minBound - minimum AABB bounds of all objects within the partition.
@PARAM maxBound - minimum AABB bounds of all objects within the partition.
@PARAM low - KDNode node child: containing all elements that satisfy the condition:
    low.maxBound[axis] < value
@PARAM mid - KDNode node child: containing all elements that satisfy the condition:
    mid.minBound[axis] <= value <= mid.maxBound[axis]
@PARAM high - KDNode node child: containing all elements that satisfy the condition:
    low.minBound[axis] > value

@RETURN:
    the node.
--]]
function KDTree:NewNode(axis, value, minBound, maxBound, low, high)
    local node = nil
    if #self.availableNodeIndices > 0 then
        node = table_remove(self.availableNodeIndices)
    else
        node = self.nodeCount
        self.nodeCount = node + 1
    end

    self.cardinalAxis[node] = axis
    self.cardinalValues[node] = vec3(value, -glm_huge, glm_huge)
    self.low[node] = low or 0 -- or nil, not an array.
    self.high[node] = high or 0

    self.nodeMinBounds[node] = minBound
    self.nodeMaxBounds[node] = maxBound

    self:UpdateQueryCache(node)
    return node
end

--[[
    Construct a new leaf node.

@PARAM objects - array of element indices, representing the node of each AABB in
    the tree. Each of those indices representing the return value to: tree:Bounds(node)
@PARAM minBound - minimum AABB bounds of the leaf object.
@PARAM maxBound - maximum AABB bounds of the leaf object.

@RETURN:
    the node.
--]]
function KDTree:NewLeaf(objects, minBound, maxBound)
    local leafNode = nil
    if #self.availableLeafIndices > 0 then
        leafNode = table_remove(self.availableLeafIndices)
    else
        leafNode = self.leafCount ; self.leafCount = leafNode + 1
    end

    self.objects[leafNode] = objects
    self.leafMinBounds[leafNode] = minBound
    self.leafMaxBounds[leafNode] = maxBound
    return -leafNode
end

--[[
    For a given interval and subset of objects (items) within it, create a
    new leaf node that represents & encapsulates all objects.
--]]
function KDTree:BuildLeafNode(interval, items, minBound, maxBound)
    local objects,nodeType = interval.objects,interval.nodeType

    local leafObjects = {}
    for i=1,#items do
        -- Ignore NodeType.Max as that object is already covered by NodeType.Min
        local itemType = interval.nodeType[items[i]]
        if itemType == Interval.NodeType.Min or itemType == Interval.NodeType.Point then
            leafObjects[#leafObjects + 1] = objects[items[i]]
        end
    end

    return self:NewLeaf(leafObjects, minBound, maxBound)
end

--[[ Delete 'node' from the tree table. --]]
function KDTree:DeleteNode(node)
    if node > 0 then
        self.low[node] = 0
        self.high[node] = 0
        self.cardinalAxis[node] = 0
        self.cardinalValues[node] = vec3(0, -glm_huge, glm_huge)
        self.nodeMinBounds[node] = emptyBounds
        self.nodeMaxBounds[node] = emptyBounds

        self.availableNodeIndices[#self.availableNodeIndices + 1] = node
    else
        local leafNode = -node

        self.objects[leafNode] = table_wipe(self.objects[leafNode])
        self.leafMinBounds[leafNode] = emptyBounds
        self.leafMaxBounds[leafNode] = emptyBounds
        self.availableLeafIndices[#self.availableLeafIndices + 1] = leafNode
    end
    return self
end

--[[
@RETURN
    True if 'node' has at least one child node (if its a non-leaf) or contains
    more than one object (if it s a leaf).
--]]
function KDTree:HasChildren(node)
    if node < 0 then
        return #self.objects[-node] > 0
    elseif self.low[node] ~= nil then
        return self.low[node] ~= 0 or self.high[node] ~= 0
    end
    return false
end

--[[
    Recompute the absolute dimensions of the subtree rooted at 'node'

@PARAM allPaths - if true, traverse the entire subtree and recompute the bounds
    to each node. Otherwise, only look at the children (or leaves) of this node.

@RETURN:
    self - this node.
--]]
function KDTree:ComputeBounds(node, allPaths)
    local min,max = vec3(glm_huge),vec3(-glm_huge)
    if node < 0 then
        local leafNode = -node

        local minBounds,maxBounds = self.minBounds,self.maxBounds
        local objects = self.objects[leafNode]
        for i=1,#objects do
            local objMin = minBounds[objects[i]]
            local objMax = maxBounds[objects[i]] or objMin
            min,max = glm_aabb_encloseAABB(min, max, objMin, objMax)
        end

        self.leafMinBounds[leafNode] = min
        self.leafMaxBounds[leafNode] = max
    else
        local low,high,axis,cardinal = self.low[node],self.high[node],
            self.cardinalAxis[node],self.cardinalValues[node]

        if low ~= 0 then
            if allPaths then self:ComputeBounds(low, true) end
            local lowMin,lowMax = MinBound(self, low),MaxBound(self, low)
            min,max = glm_aabb_encloseAABB(min, max, lowMin, lowMax or lowMin)
        end

        if high ~= 0 then
            if allPaths then self:ComputeBounds(high, true) end
            local highMin,highMax = MinBound(self, high),MaxBound(self, high)
            min,max = glm_aabb_encloseAABB(min, max, highMin, highMax or highMin)
        end

        self.nodeMinBounds[node] = min
        self.nodeMaxBounds[node] = max

        if low ~= 0 then cardinal = vec3(cardinal[1], MaxBound(self, low)[axis], cardinal[3]) end
        if high ~= 0 then cardinal = vec3(cardinal[1], cardinal[2], MinBound(self, high)[axis]) end
        self.cardinalValues[node] = cardinal
    end

    return self
end

--[[
    Insert an object into the subtree rooted at 'node'. This function will
    return 'node', although its subtree may be reorganized.

@RETURN
    Index of the node.
--]]
function KDTree:InsertNode(intervals, node, object, pooler)
    pooler = pooler or CreateIntervalPooler()
    if node == nil then -- Invalid Node.
        return 0
    elseif node < 0 then
        local objects = self.objects[-node]
        objects[#objects + 1] = object

        -- Create a new subtree when the number of objects in this node exceeds its limit
        if #objects > self.leafSize then
            return self:Split(pooler, intervals, intervals:SubsortIndex(objects))
        end

        self:ComputeBounds(node, false)
        return node
    else
        local partitions = Interval.Partitions.Binary
        local low,high,nodeMinBnds,nodeMaxBnds =
            self.low,self.high,self.nodeMinBounds,self.nodeMaxBounds

        -- Determine which partition the node would belong to according to the
        -- previously computed (at build-time) intervals data. Then attempt to
        -- insert that object into the sub-tree.
        local axis,value = self.cardinalAxis[node],self.cardinalValues[node][1]

        local newChild = nil
        if partitions.Lower(intervals, object, axis, value) then
            newChild = self:InsertNode(intervals, low[node], object, pooler)
            low[node] = newChild
        elseif partitions.Upper(intervals, object, axis, value) then
            newChild = self:InsertNode(intervals, high[node], object, pooler)
            high[node] = newChild
        else
            error("Unexpected KDTree State!")
        end

        -- After insertion, ensure the bounds of this 'node' accounts for any
        -- growth/resizing.
        local cmin = MinBound(self, newChild)
        nodeMinBnds[node] = glm_min(nodeMinBnds[node], cmin)
        nodeMaxBnds[node] = glm_max(nodeMaxBnds[node], MaxBound(self, newChild) or cmin)

        self:UpdateQueryCache(node)
        return node
    end
end

--[[
    Remove 'object' from the subtree rooted at 'node'. This function may
    reorganize the entire sub-tree (e.g., delete an entire branch). So the
    returned node may be different.

@RETURN:
    The node of the new subtree root ('node' if nothing has changed).
--]]
function KDTree:RemoveNode(intervals, node, object)
    if node < 0 then -- Remove item from the object list
        local objects = self.objects[-node]

        local prevObjCount = #objects
        for i=prevObjCount,1,-1 do
            if object == objects[i] then
                table_remove(objects, i)
                break
            end
        end

        -- No more objects within the leaf; delete the node and recycle
        --
        -- @TODO: Alleviate this constraint in hoping that subsequent "Insert"
        -- operations will find themselves back in this leaf?
        if #objects == 0 then
            self:DeleteNode(node)
            return 0
        -- Recompute the bounds of all objects left in the leaf node.
        elseif prevObjCount ~= #objects then
            self:ComputeBounds(node, false)
        end
        return node
    else
        local partitions = Interval.Partitions.Binary
        local low,high,axis,value = self.low[node],self.high[node],
            self.cardinalAxis[node],self.cardinalValues[node][1]

        -- Determine the child node that the 'object' would fall into.
        if partitions.Lower(intervals, object, axis, value) then
            low = self:RemoveNode(intervals, low, object)
        elseif partitions.Upper(intervals, object, axis, value) then
            high = self:RemoveNode(intervals, high, object)
        else
            error("Unexpected KDTree State!")
        end

        -- If a 'node' is deleted, it must be replaced with a leaf-node from
        -- one of its subtrees. This ensures the 'invariant' is still held.
        if low == 0 then -- 'low' was deleted
            self:DeleteNode(node)
            return high
        elseif high == 0 then -- 'high' was deleted
            self:DeleteNode(node)
            return low
        end

        -- Compute the bounds of this node by taking the union among each of the
        -- available children nodes. Since this function is bottom-up, no
        -- depth-first computation of the entire sub-tree is required
        self.low[node],self.high[node] = low,high
        self:ComputeBounds(node, false)
        return node
    end
end

--[[ Cache any intermediate values to increase the performance of querying. --]]
function KDTree:UpdateQueryCache(node)
    if node > 0 then
        local low,high,axis,cardinal = self.low[node],self.high[node],
            self.cardinalAxis[node],self.cardinalValues[node]

        if low and low ~= 0 then cardinal = vec3(cardinal[1], MaxBound(self, low)[axis], cardinal[3]) end
        if high and high ~= 0 then cardinal = vec3(cardinal[1], cardinal[2], MinBound(self, high)[axis]) end
        self.cardinalValues[node] = cardinal
    end
    return self
end

----------------------------------------
------------- Construction -------------
----------------------------------------

function KDTree:Build(intervals)
    local itemSet = intervals:SortIndex()
    local pooler = CreateIntervalPooler()

    local minBounds,maxBounds = vec3(glm_huge),vec3(-glm_huge)
    for i=1,#intervals.values do
        minBounds = glm_min(minBounds, intervals.values[i])
        maxBounds = glm_max(maxBounds, intervals.values[i])
    end

    self.root = self:Split(pooler, intervals, itemSet, #intervals.objects, minBounds, maxBounds)
    self.modificationCount = 0
    self.intervals = intervals
    self.minBounds = intervals.minBounds
    self.maxBounds = intervals.maxBounds
    return self -- @TODO force a garbage collection cycle for the pooler?
end

--[[
    @OVERRIDE

@PARAM newIntervals An optional intervals reference, potentially different to
    the one maintained by the tree. The original intent of this parameter is to
    compensate for any previous invocations to: KDTree:Immutable()
--]]
function KDTree:Rebuild(newIntervals)
    local intervals = newIntervals or self.intervals
    if intervals then
        return self:Clear():Build(intervals)
    else
        error("Cannot rebuild KDTree: 'intervals' is missing!")
    end
    return self
end

--[[ @OVERRIDE --]]
function KDTree:Immutable()
    table_wipe(self.availableNodeIndices)
    table_wipe(self.availableLeafIndices)

    self.intervals = nil
    self.availableNodeIndices = nil
    self.availableLeafIndices = nil
    return self
end

--[[ Recursively build the Kd-Tree. --]]
function KDTree:Split(pooler, intervals, itemSet, objectCount, minBounds, maxBounds)
    if objectCount == 0 then
        return nil
    elseif objectCount <= self.leafSize then
        return self:BuildLeafNode(intervals, itemSet[1], minBounds, maxBounds)
    end

    -- Given a (sub-)set of objects being partitioned, find an axis and value
    -- that best divides them.
    local partitions = Interval.Partitions.Binary
    local axis,value = intervals:strategy(partitions, itemSet, objectCount)

    -- On failure, i.e., could not partition, assume all of the data is
    -- identical or near-identical. Let it all be managed by a single leaf node.
    if axis == nil or value == nil then
        return self:BuildLeafNode(intervals, itemSet[1], minBounds, maxBounds)
    end

    -- Apply the axis,value partition to each interval: create sub-partitions
    local part = pooler:Pool()
    local lowSplit,highSplit,set,count,min,max

    set,count,min,max = intervals:PartitionIntervals(itemSet, partitions.Lower, axis, value, part)
    if count > 0 then
        lowSplit = self:Split(pooler, intervals, set, count, min, max)
    end

    set,count,min,max = intervals:PartitionIntervals(itemSet, partitions.Upper, axis, value, pooler:Recycle(part))
    if count > 0 then
        highSplit = self:Split(pooler, intervals, set, count, min, max)
    end

    pooler:Release(part)
    return self:NewNode(axis, value, minBounds, maxBounds, lowSplit, highSplit)
end

--[[ @OVERRIDE --]]
function KDTree:Insert(object, aabbMin, aabbMax)
    local intervals = self.intervals
    if not intervals then
        error("Tree is immutable")
    end

    self.modificationCount = self.modificationCount + 1
    intervals:AppendBounds(object, aabbMin, aabbMax)
    if self.root then
        self.root = self:InsertNode(intervals, self.root, object, nil)
    else
        self.root = self:Split(nil, intervals, intervals:SubsortIndex({ object }))
    end
    return self
end

--[[ @OVERRIDE --]]
function KDTree:InsertPoint(object, point)
    local intervals = self.intervals
    if not intervals then
        error("Tree is immutable")
    end

    self.modificationCount = self.modificationCount + 1
    intervals:AppendPoint(object, point)
    if self.root then
        self.root = self:InsertNode(intervals, self.root, object, nil)
    else
        self:Build(self.intervals)
    end
    return self
end

--[[ @OVERRIDE --]]
function KDTree:Remove(object)
    if not self.intervals then
        error("Tree is immutable")
    end

    if self.root then
        self.modificationCount = self.modificationCount + 1
        self.root = self:RemoveNode(self.intervals, self.root, object)
        if self.root == 0 then -- Everything was deleted.
            self.root = nil
        end
    end
    return true
end

---------------------------------------
--------------- Queries ---------------
---------------------------------------

--[[ @OVERRIDE --]]
function KDTree:CreateQueryCache()
    return setmetatable(table_create(64, 0), { __mode = "v" }) -- Recyclable/Reused stack structure
end

--[[ @OVERRIDE --]]
function KDTree:Each(yield)
    local objects = self.objects
    for i=1,self.leafCount-1 do
        local leafObjects = objects[i]
        for j=1,#leafObjects do
            yield(leafObjects[j])
        end
    end
end

--[[ @OVERRIDE --]]
function KDTree:Query(stack, point, yield)
    if not self.root then return end
    local low,high,objects,cardinalAxis,cardinal,minBounds,maxBounds =
        self.low,self.high,self.objects,self.cardinalAxis,
        self.cardinalValues,self.minBounds,self.maxBounds

    local pointer = 1 ; stack[1] = self.root
    while pointer > 0 do
        local node = stack[pointer] ; stack[pointer] = nil ; pointer = pointer - 1
        if node < 0 then
            local leafObjects = objects[-node]
            for i=1,#leafObjects do
                local object = leafObjects[i]
                local minB = minBounds[object]
                if glm_aabb_contains(minB, maxBounds[object] or minB, point) then
                    yield(object)
                end
            end
        else
            local value = point[cardinalAxis[node]]
            if value <= cardinal[node][2] then pointer = pointer + 1 ; stack[pointer] = low[node] end
            if value >= cardinal[node][3] then pointer = pointer + 1 ; stack[pointer] = high[node] end
        end
    end
end

--[[
    Generalized stack query.

@PARAM arg0/arg1 are explicitly defined to avoid the overhead of TK_DOTS; arg1
    should be ignored for glm_aabb_contains anyway. A few hundred nanoseconds
    can be saved (per-query) by making the query 'less' generic.
--]]
function KDTree:GenericQuery(stack, F, arg0, arg1, yield)
    if not self.root then return end
    local objects,low,high,minBounds,maxBounds,nodeMinBnds,nodeMaxBnds =
        self.objects,self.low,self.high,self.minBounds,self.maxBounds,
        self.nodeMinBounds,self.nodeMaxBounds

    local pointer = 1 ; stack[1] = self.root
    while pointer > 0 do
        local node = stack[pointer] ; stack[pointer] = nil ; pointer = pointer - 1
        if node < 0 then -- Node is a leaf, see what objects intersect
            local leafObjects = objects[-node]
            for i=1,#leafObjects do
                local object = leafObjects[i]
                if F(minBounds[object], maxBounds[object] or minBounds[object], arg0, arg1) then
                    yield(object)
                end
            end
        else -- Intersects the node AABB, process children
            local nodeMin = nodeMinBnds[node]
            if F(nodeMin, nodeMaxBnds[node] or nodeMin, arg0, arg1) then
                if low[node] ~= 0 then pointer = pointer + 1 ; stack[pointer] = low[node] end
                if high[node] ~= 0 then pointer = pointer + 1 ; stack[pointer] = high[node] end
            end
        end
    end
end

--[[ KDTree:Query but implemented with GenericQuery --]]
function KDTree:Query2(stack, point, yield)
    self:GenericQuery(stack, glm_aabb_contains, point, nil, yield)
end

--[[ @OVERRIDE --]]
function KDTree:Raycast(stack, origin, direction, yield)
    self:GenericQuery(stack, glm_aabb_intersectsRay, origin, direction, yield)
end

--[[ KDTree:Raycast but implemented with 'slabs' --]]
function KDTree:Slabs(stack, origin, direction, yield)
    self:GenericQuery(stack, glm_aabb_slabs, origin, 1.0 / direction, yield)
end

--[[ @OVERRIDE --]]
function KDTree:Colliding(stack, colMin, colMax, yield)
    self:GenericQuery(stack, glm_aabb_intersectsAABB, colMin, colMax, yield)
end

--[[ @OVERRIDE --]]
function KDTree:SphereIntersection(cache, origin, radius, yield)
    self:GenericQuery(cache, glm_aabb_intersectsSphere, origin, radius, yield)
end

--[[ @OVERRIDE --]]
local RecursiveNeighborSearch = nil
function KDTree:NearestNeighbors(_, point, neighborList)
    if neighborList.maxSize < 1 then
        return neighborList
    end

    return RecursiveNeighborSearch(self, self.root, point, neighborList)
end

--[[ @NOTE: Designed only for Interval.NodeType.Point --]]
RecursiveNeighborSearch = function(self, node, point, neighborList)
    if node == nil then
        return neighborList
    elseif node < 0 then -- A leaf
        local minBounds = self.minBounds
        local objects = self.objects[-node]
        for i=1,#objects do
            neighborList:Insert(objects[i], #(point - minBounds[objects[i]]))
        end
        return neighborList
    else
        local low,high,axis,cardinal = self.low[node],self.high[node],
            self.cardinalAxis[node],self.cardinalValues[node]

        -- Recurse the subtree that this point intersects on the cardinal axis
        -- of this node.
        if point[axis] <= cardinal[1] then
            RecursiveNeighborSearch(self, low, point, neighborList) ; low = nil
        else
            RecursiveNeighborSearch(self, high, point, neighborList) ; high = nil
        end

        -- Search other subtree if:
        --  (1) not enough nearest-neighbors;
        --  (2) distance to test point on the axis is less than the worst
        --      distance of neighborList (axes could be equal).
        if #neighborList.data < neighborList.maxSize then
            if low then RecursiveNeighborSearch(self, low, point, neighborList) end
            if high then RecursiveNeighborSearch(self, high, point, neighborList) end
        elseif low and glm_abs(cardinal[2] - point[axis]) <= (neighborList.worstDist + glm_feps) then
            RecursiveNeighborSearch(self, low, point, neighborList)
        elseif high and glm_abs(cardinal[3] - point[axis]) <= (neighborList.worstDist + glm_feps) then
            RecursiveNeighborSearch(self, high, point, neighborList)
        end
        return neighborList
    end
end

----------------------------------------
----------- Statistics/Debug -----------
----------------------------------------

local IndentString = "  "
local OutputSubtree = nil
OutputSubtree = function(self, lines, node, indent)
    local nextIndent = indent .. IndentString
    if node < 0 then
        local leafNode = -node
        local minBound = self.leafMinBounds[leafNode]
        local maxBound = self.leafMaxBounds[leafNode] or minBound
        local objects = self.objects[leafNode]

        local elementBuffer =  {}
        for i=1,#objects do
            elementBuffer[i] = objects[i]
        end

        lines[#lines + 1] = ("%sLeaf Index = %d,"):format(indent, -node)
        lines[#lines + 1] = ("%sMin = %s,"):format(indent, tostring(minBound))
        lines[#lines + 1] = ("%sMax = %s,"):format(indent, tostring(maxBound))
        lines[#lines + 1] = ("%sElements = [%s],"):format(indent, table.concat(elementBuffer, ", "))
    else
        local minBound = self.nodeMinBounds[node]
        local maxBound = self.nodeMaxBounds[node] or minBound

        lines[#lines + 1] = ("%sNode Index = %d,"):format(indent, node)
        lines[#lines + 1] = ("%sCardinal Axis = %d,"):format(indent, self.cardinalAxis[node])
        lines[#lines + 1] = ("%sCardinal Value = %f,"):format(indent, self.cardinalValues[node][1])
        lines[#lines + 1] = ("%sMin = %s,"):format(indent, tostring(minBound))
        lines[#lines + 1] = ("%sMax = %s,"):format(indent, tostring(maxBound))

        local low = self.low[node]
        if low and low ~= 0 then
            lines[#lines + 1] = ("%sLow {"):format(indent)
            OutputSubtree(self, lines, low, indent .. IndentString)
            lines[#lines + 1] = ("%s},"):format(indent)
        end

        local high = self.high[node]
        if high and high ~= 0 then
            lines[#lines + 1] = ("%sHigh {"):format(indent)
            OutputSubtree(self, lines, high, indent .. IndentString)
            lines[#lines + 1] = ("%s},"):format(indent)
        end
    end
end

--[[ @TODO: This function is a mess. --]]
local function ComputeStatistics(self, rootIndex)
    local stack = self:CreateQueryCache()
    local objects,low,high = self.objects,self.low,self.high

    local objCount,nodeCount,leafCount = 0,0,0
    local minLeafCount,maxLeafCount = glm_huge,-glm_huge
    local minDepth,maxDepth = glm_huge,-glm_huge

    -- Welfords algorithm for object count distribution
    local k,m,m_last,s,s_last = 0,0.0,0.0,0.0,0.0

    local pointer = 1
    local depthMap = {}

    rootIndex = rootIndex or self.root
    stack[1] = rootIndex ; depthMap[rootIndex] = 1
    while pointer > 0 do
        local node = stack[pointer] ; stack[pointer] = nil ; pointer = pointer - 1

        local depth = depthMap[node]
        if node < 0 then
            local objects = objects[-node]
            local x = #objects

            leafCount = leafCount + 1
            objCount = objCount + x
            minLeafCount = (minLeafCount > x and x) or minLeafCount
            maxLeafCount = (maxLeafCount > x and maxLeafCount) or x
            minDepth = (minDepth > depth and depth) or minDepth
            maxDepth = (maxDepth > depth and maxDepth) or depth

            -- Mean/Variants of object counts
            m_last = (k == 0 and x) or m
            s_last = (k == 0 and 0.0) or s

            k = k + 1
            m = m_last + (x - m_last) / k
            s = s_last + (x - m_last) * (x - m)
        else
            nodeCount = nodeCount + 1

            local low,high = low[node],high[node]
            if low then pointer = pointer + 1 ; depthMap[low] = depth + 1 ; stack[pointer] = low end
            if high then pointer = pointer + 1 ; depthMap[high] = depth + 1 ; stack[pointer] = high end
        end
    end

    return {
        objectCount = objCount,
        nodeCount = nodeCount,
        leafCount = leafCount,
        totalNodeCount = nodeCount + leafCount,
        minDepth = minDepth,
        maxDepth = maxDepth,
        minBound = (rootIndex and self.minBounds[rootIndex]) or nil,
        maxBound = (rootIndex and self.maxBounds[rootIndex]) or nil,
        leafMeanCount = m,
        leafVarCount = glm.sqrt((k < 2 and 0) or (s / k)),
        minLeafCount = minLeafCount,
        maxLeafCount = maxLeafCount,
    }
end

--[[ @OVERRIDE --]]
function KDTree:Output(output)
    if self.root then
        local lines = {}
        OutputSubtree(self, lines, self.root, "")
        output(table.concat(lines, "\n"))
    end
    return self
end

--[[ @OVERRIDE --]]
function KDTree:OutputStatistics(output)
    if not self.root then return end

    local stats = ComputeStatistics(self, self.root)
    local lines  = {}
    lines[#lines + 1] = "Tree Statistics:"
    lines[#lines + 1] = ("Leaf Size = %d"):format(self.leafSize)
    lines[#lines + 1] = ("Min = %s,"):format(tostring(stats.minBound))
    lines[#lines + 1] = ("Max = %s,"):format(tostring(stats.maxBound))
    lines[#lines + 1] = ("Object Count = %d"):format(stats.objectCount)
    lines[#lines + 1] = ("Node Count = %d"):format(stats.nodeCount)
    lines[#lines + 1] = ("Leaf Count = %d"):format(stats.leafCount)
    lines[#lines + 1] = ("Count = %d"):format(stats.totalNodeCount)
    lines[#lines + 1] = ""
    lines[#lines + 1] = ("Minimum Depth = %d"):format(stats.minDepth)
    lines[#lines + 1] = ("Maximum Depth = %d"):format(stats.maxDepth)
    lines[#lines + 1] = ""
    lines[#lines + 1] = "Leaf Statistics:"
    lines[#lines + 1] = ("Leaf Element Mean = %f"):format(stats.leafMeanCount)
    lines[#lines + 1] = ("Leaf Element Variance = %f"):format(stats.leafVarCount)
    lines[#lines + 1] = ("Minimum Leaf Elements = %f"):format(stats.minLeafCount)
    lines[#lines + 1] = ("Maximum Leaf Elements = %f"):format(stats.maxLeafCount)

    output(table.concat(lines, ("\n%s"):format(IndentString)))
end

return KDTree
