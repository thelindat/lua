--[[
    A bounded collection of values ordered by some numeric score, i.e.,
    <item, distance> pairs.

@LICENSE
    It's yours, I don't want it.
--]]
local OrderedList

local glm = glm
local table = table
local table_insert = table.insert
local table_wipe = table.wipe or function() return {} end

OrderedList = setmetatable({
    SequentialBinaryThreshold = 32,

    __len = function(self) return #self.data end,
    __pairs = function(self) return ipairs(self.data) end,
    __index = function(self, key)
        local index = tonumber(key)
        if index ~= nil then
            return self.data[index]
        end
        return rawget(OrderedList, key)
    end,

}, {
    __call = function(_, ...)
        return OrderedList.New(...)
    end
})

--[[
@PARAM maxSize - the number of neighbors to keep in the list.
@PARAM fixedPoint - fixed point to compare all neighbors to.
--]]
function OrderedList.New(maxSize)
    local size = maxSize or 2^31
    return setmetatable({
        maxSize = size, -- maximum list size.
        worstDist = glm.huge,

        data = {}, -- sorted list of comparator-sorted indices.
        dataIndexScores = {}, -- map of neighbors.
    }, OrderedList)
end

--[[
    Remove all <item, value> pairs from the list, potentially changing the
    maximum size of the ordered list.
--]]
function OrderedList:Clear(maxSize)
    local size = maxSize or self.maxSize

    self.worstDist = glm.huge
    self.maxSize = size
    self.data = table_wipe(self.data)
    self.dataIndexScores = table_wipe(self.dataIndexScores)
    return self
end

--[[
    Return the maximum size of this distance list. However, overflow may occur
    when multiple items have the same "worst" distance.
--]]
function OrderedList:GetMaximumSize() return self.maxSize end

--[[ Return the total number of <item, distance> pairs in this list. --]]
function OrderedList:Size() return #self.data end

--[[ Fetch the i^{th} item index. --]]
function OrderedList:Get(i) return self.data[i] end

--[[ --]]
function OrderedList:Score(item) return self.dataIndexScores[item] end

--[[ Get the farthest item index in this list. --]]
function OrderedList:GetWorstDistance() return self.dataIndexScores[self.data[#self.data]] end

--[[ Binary insert without a comparator function. --]]
local function BinaryInsert(values, valueScores, object, dist)
    local tidx = 0
    local tstart,tend,tmid = 1,#values,1
    while tstart <= tend do -- Get insert position
        tmid = ((tstart + tend) * 0.5) // 1.0
        if dist < valueScores[values[tmid]] then -- compare
            tend,tidx = tmid - 1,0
        else
            tstart,tidx = tmid + 1,1
        end
    end

    table_insert(values, tmid + tidx, object)
    return (tmid + tidx)
end

--[[ Insert an item into the nearest neighbor list. --]]
function OrderedList:Insert(object, dist)
    local data = self.data
    local worstDist = self.worstDist
    local dataIndexScores = self.dataIndexScores
    if dataIndexScores[object] ~= nil then
        return
    end

    if #data < self.maxSize then -- not at capacity, add the element
        dataIndexScores[object] = dist
        BinaryInsert(data, dataIndexScores, object, dist)

        self.worstDist = (worstDist < dist and dist) or worstDist

    -- Equal to the "worstDistance": instead of arbitrarily breaking ties simply
    -- expand the neighbors buffer. If some other value undertakes this as the
    -- "worstDistance" and fills up the buffer.
    elseif dist == worstDist then
        data[#data + 1] = object
        dataIndexScores[object] = dist

    -- Kick out all elements that fall outside the maximum allowable bounds.
    elseif dist < worstDist then
        dataIndexScores[object] = dist
        BinaryInsert(data, dataIndexScores, object, dist)

        -- Try to trim all neighbors that fall outside of maxSize
        local largestWorst = dataIndexScores[data[self.maxSize]]
        for i=#data,(self.maxSize + 1),-1 do
            if dataIndexScores[data[i]] > largestWorst then
                dataIndexScores[data[i]] = nil
                data[i] = nil
            end
        end

        self.worstDist = dataIndexScores[data[#data]]
    end
    return self
end

return OrderedList
