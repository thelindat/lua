--[[
    3D Spatial Indexing using a list

@NOTES:
    This script exists only for testing/baseline comparisons

@LICENSE
    It's yours, I don't want it.
--]]
local SequentialSpatial

local table_remove = table.remove
local table_wipe = table.wipe or function() return {} end

local glm = glm
local glm_aabb_contains = glm.aabb.contains
local glm_aabb_distance = glm.aabb.distance
local glm_aabb_slabs = glm.aabb.slabs
local glm_aabb_intersectsRay = glm.aabb.intersectsRay
local glm_aabb_intersectsAABB = glm.aabb.intersectsAABB
local glm_aabb_intersectsSphere = glm.aabb.intersectsSphere

SequentialSpatial = setmetatable({}, {
    __call = function(_, ...)
        return SequentialSpatial.New(...)
    end
})
SequentialSpatial.__index = SequentialSpatial

function SequentialSpatial.New()
    return setmetatable({ items = {}, minBounds = {}, maxBounds = {}, }, SequentialSpatial)
end

--[[ @OVERRIDE --]]
function SequentialSpatial:Bounds(object)
    return self.minBounds[object],self.maxBounds[object]
end

--[[ @OVERRIDE --]]
function SequentialSpatial:Clear()
    self.items = table_wipe(self.items)
    self.minBounds = table_wipe(self.minBounds)
    self.maxBounds = table_wipe(self.maxBounds)
    return self
end

--[[ @OVERRIDE --]]
function SequentialSpatial:Compact()
    return self
end

---------------------------------------
------------ Modifications ------------
---------------------------------------

--[[ @OVERRIDE --]]
function SequentialSpatial:Rebuild() return self end

--[[ @OVERRIDE --]]
function SequentialSpatial:Immutable() return self end

--[[ @OVERRIDE --]]
function SequentialSpatial:Insert(object, aabbMin, aabbMax)
    if not self.minBounds[object] then
        local count = #self.items + 1

        self.items[count] = object
        self.minBounds[object] = aabbMin
        self.maxBounds[object] = aabbMax or aabbMin
    end
    return self
end

--[[ @OVERRIDE --]]
function SequentialSpatial:InsertPoint(object, point)
    return self:Insert(object, point, nil)
end

--[[ @OVERRIDE --]]
function SequentialSpatial:Remove(object)
    local items = self.items
    for i=1,#items do
        if object == items[i] then
            table_remove(items, i)
            self.minBounds[object] = nil
            self.maxBounds[object] = nil
            break
        end
    end
    return self
end

---------------------------------------
--------------- Queries ---------------
---------------------------------------

--[[ @OVERRIDE --]]
function SequentialSpatial:CreateQueryCache()
    return nil
end

--[[ Generalized stack query. --]]
function SequentialSpatial:GenericQuery(cache, F, arg0, arg1, yield)
    local items,minBounds,maxBounds = self.items,self.minBounds,self.maxBounds

    for i=1,#items do
        local object = items[i]
        local objMin = minBounds[object]
        if F(objMin, maxBounds[object] or objMin, arg0, arg1) then
            yield(object)
        end
    end
end

--[[ @OVERRIDE --]]
function SequentialSpatial:Each(yield)
    local items = self.items
    for i=1,#items do
        yield(items[i])
    end
end

--[[ @OVERRIDE --]]
function SequentialSpatial:Query(cache, point, yield)
    return self:GenericQuery(cache, glm_aabb_contains, point, nil, yield)
end

--[[ @OVERRIDE --]]
function SequentialSpatial:Raycast(cache, origin, direction, yield)
    return self:GenericQuery(cache, glm_aabb_intersectsRay, origin, direction, yield)
    --return self:GenericQuery(cache, glm_aabb_slabs, origin, direction, yield)
end

function SequentialSpatial:Raycast2(cache, origin, direction, yield)
    return self:GenericQuery(cache, glm.aabb.slabs, origin, direction, yield)
end

--[[ @OVERRIDE --]]
function SequentialSpatial:Colliding(cache, colMin, colMax, yield)
    return self:GenericQuery(cache, glm_aabb_intersectsAABB, colMin, colMax, yield)
end

--[[ @OVERRIDE --]]
function SequentialSpatial:SphereIntersection(cache, origin, radius, yield)
    return self:GenericQuery(cache, glm_aabb_intersectsSphere, origin, radius, yield)
end

--[[ @OVERRIDE --]]
function SequentialSpatial:NearestNeighbors(_, point, neighborList)
    local items,minBounds = self.items,self.minBounds
    for i=1,#items do
        neighborList:Insert(items[i], #(point - minBounds[items[i]]))
    end
end

return SequentialSpatial
