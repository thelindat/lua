--[[
    Separating axis tests for data points on a specific axis.

    An Interval can represent points (single values) or intervals (two values).
    Single points are classified with the NodeType.Point field. Intervals are
    represented by two axis values classified by NodeType.Min and NodeType.Max
    respectively.

    Strategies: The function/approach used to select an axis and value for
    partitioning (denoted as 'cardinal' in the KdTree implementation).

        - Interval.SurfaceAreaHeuristic()
            : NOTE: Only to be used when indexing AABBs,
        - Interval.DynamicStrategy()
        - Interval.BestStrategy(axisSplitter)
        - Interval.GreedyStrategy(axisSelector, axisSplitter)
        - Interval.DefaultStrategy(axisSelector, axisSplitter)

        Axis Selection:
            - Interval.LargestInterval
            - Interval.SmallestInterval
            - Interval.MostVariableInterval
            - Interval.LeastVariableInterval

        Axis Splits:
            - Interval.EventSplit
            - Interval.MedianSplit
            - Interval.MeanSplit
            - Interval.IntervalCount

    Examples:
        Interval.SurfaceAreaHeuristic()
        Interval.BestStrategy(Interval.MedianSplit)
        Interval.GreedyStrategy(Interval.LargestInterval, Interval.MedianSplit)

@TODO: Axis selection strategy based on round robin, i.e., axis = depth % 3

@LICENSE
    See Copyright Notice in lua.h
--]]
local Interval

local vec3 = vec3

local table_insert = table.insert
local table_remove = table.remove
local table_sort = table.sort
local table_create = table.create or function() return {} end
local table_wipe = table.wipe or function() return {} end

local glm_min = glm.min
local glm_max = glm.max
local glm_log = glm.log
local glm_aabb_encloseAABB = glm.aabb.encloseAABB
local glm_aabb_surfaceArea = glm.aabb.surfaceArea

-- "Infinity" may not be available in cases where the Interval is serialized;
-- Replace with some significantly large value.
local glm_huge <const> = glm.huge
local glm_eps <const> = glm.feps

Interval = setmetatable({
    -- Internal point reference.
    NodeType = { Min = 0, Max = 1, Point = 3 },

    -- Surface Area Heuristic cost for traversal operations;
    -- @TODO Tune for Lua environments
    SAH_Traversal = 1.0,

    -- Surface Area Heuristic cost for intersection operations;
    -- @TODO Tune for Lua environments
    SAH_Intersection = 1.0,

    -- Axis partitioning types
    Partitions = {
        Binary = {
            -- AABB's that are "less-than-or-equal-to" the split value on the
            -- specified split axis.
            Lower = function(self, object, axisKey, value)
                return self.minBounds[object][axisKey] <= value
            end,

            -- AABB's that are "right/greater-than" the split value on the split
            -- axis.
            Upper = function(self, object, axisKey, value)
                return value < self.minBounds[object][axisKey]
            end,

            -- Nothing.
            Mid = function(self, object, axisKey, value)
                return false
            end,
        },

        --[[
            Ternary Partitioning

            @NOTE: At one point the kdtree implemented supported an 'optional'
            middle partition, i.e., all objects that intersect the boundary.
            However, that code has been removed to simplify the script.
        --]]
        Ternary = {
            -- AABB's that are "less-than" the split value on the specified split axis.
            Lower = function(self, object, axisKey, value) return
                value > self.maxBounds[object][axisKey]
            end,

            -- AABB's that are "greater-than" the split value on the split axis.
            Upper = function(self, object, axisKey, value)
                return value < self.minBounds[object][axisKey]
            end,

            -- AABB's that intersect with the split value on the split axis.
            Mid = function(self, object, axisKey, value)
                return self.minBounds[object][axisKey] <= value
                        and value <= self.maxBounds[object][axisKey]
            end,
        },
    },
}, {
    __call = function(_, ...)
        return Interval.New(...)
    end
})
Interval.__index = Interval

--[[
    Create a new interval

@PARAM strategy: See script header
--]]
function Interval.New(strategy)
    strategy = strategy or Interval.SurfaceAreaHeuristic()
    --strategy = strategy or Interval.GreedyStrategy(Interval.LargestInterval, Interval.MedianSplit)
    local interval = setmetatable({
        -- AABB parameters of each object within the KdTree. If the index only
        -- references points, then 'minBounds' stores the point of each object
        -- within the interval.
        minBounds = {}, maxBounds = {},

        --[[ Interval information --]]

        index = 1, -- counter for flat representation
        values = {}, -- point in vector space
        nodeType = {}, -- Interval.NodeType
        siblingIndex = {}, -- index of sibling value for max/min

        objects = {}, -- [Object] = Index
        objectLookup = {}, -- [Index] = Object; reverse lookup.

        --[[ Partitioning ]]

        strategy = strategy, -- Partitioning strategy
        comparators = nil, -- Comparators for table.sort'ing intervals.

        -- Three independent lists of value indices, one for each dimension,
        -- that are sorted according to their value on that dimension. Before
        -- these lists are sorted, they are three identical lists of
        -- incrementing 'values' indices.
        sorted = false,
        sortedIndex = { {}, {}, {} },
    }, Interval)

    interval.comparators = {
        function(a, b) return interval.values[a][1] < interval.values[b][1] end,
        function(a, b) return interval.values[a][2] < interval.values[b][2] end,
        function(a, b) return interval.values[a][3] < interval.values[b][3] end,
    }

    return interval
end

function Interval:Clear()
    self.index = 1
    self.values = table_wipe(self.values)
    self.objects = table_wipe(self.objects)
    self.objectLookup = table_wipe(self.objectLookup)
    self.nodeType = table_wipe(self.nodeType)
    self.siblingIndex = table_wipe(self.siblingIndex)

    self.sorted = false
    self.sortedIndex[1] = table_wipe(self.sortedIndex[1])
    self.sortedIndex[2] = table_wipe(self.sortedIndex[2])
    self.sortedIndex[3] = table_wipe(self.sortedIndex[3])
    return self
end

--[[ Add a new data-point to the interval list. --]]
function Interval:Append(value, object, nodeType)
    local bininsert = function(t, value, fcomp)
        local tidx = 0
        local tstart,tend,tmid = 1,#t,1
        while tstart <= tend do
            tmid = (tstart + tend) >> 1 -- @TODO Can technically overflow
            if fcomp(value, t[tmid]) then
                tend,tidx = tmid - 1,0
            else
                tstart,tidx = tmid + 1,1
            end
        end

        table_insert(t, tmid + tidx, value)
        return (tmid + tidx)
    end

    local index = self.index

    self.index = self.index + 1
    self.values[index] = value
    self.objects[index] = object
    self.objectLookup[object] = index
    self.nodeType[index] = nodeType
    self.siblingIndex[index] = -1
    if self.sorted then
        bininsert(self.sortedIndex[1], index, self.comparators[1]) -- X
        bininsert(self.sortedIndex[2], index, self.comparators[2]) -- Y
        bininsert(self.sortedIndex[3], index, self.comparators[3]) -- Z
    else
        self.sortedIndex[1][index] = index -- X
        self.sortedIndex[2][index] = index -- Y
        self.sortedIndex[3][index] = index -- Z
    end
    return index
end

--[[
    Mark two indices as siblings (pertaining to the same AABB dimension segment)
    in the interval.
--]]
function Interval:SetSiblings(firstIndex, secondIndex)
    self.siblingIndex[firstIndex] = secondIndex
    self.siblingIndex[secondIndex] = firstIndex
    return self
end

--[[ Append each dimension of the interval to the collection --]]
function Interval:AppendPoint(object, point)
    if not self.minBounds[object] then
        self.minBounds[object] = point
        self.maxBounds[object] = nil
        return self:Append(point, object, Interval.NodeType.Point)
    end
    return self
end

--[[ Append each dimension to the min/max bound pairing to the intervals collection --]]
function Interval:AppendBounds(object, aabbMin, aabbMax)
    if not self.minBounds[object] then
        self.minBounds[object] = aabbMin
        self.maxBounds[object] = aabbMax
        return self:SetSiblings(
            self:Append(aabbMin, object, Interval.NodeType.Min),
            self:Append(aabbMax, object, Interval.NodeType.Max)
        )
    end
    return self
end

--[[
    Remove a data-point from the interval list. If the 'index' has a sibling,
    that siblings type is converted to NodeType.Point.
--]]
function Interval:Remove(index)
    local values,objects,nodeType,objectLookup,siblingIndex = self.values,
        self.objects,self.nodeType,self.objectLookup,self.siblingIndex

    -- Remove sibling links
    local sibling = siblingIndex[index]
    if sibling ~= -1 then
        siblingIndex[sibling] = -1
        self.nodeType[sibling] = Interval.NodeType.Point
    end

    self.index = self.index - 1
    objectLookup[objects[index]] = nil

    table_remove(objects, index)
    table_remove(values, index)
    table_remove(nodeType, index)
    table_remove(siblingIndex, index)

    -- Update the siblings list: If the sibling index is greater than the index
    -- being removed, reduce its index.
    local numSiblings = #siblingIndex
    for i=numSiblings,1,-1 do
        objectLookup[objects[i]] = i -- only needs: i=index,#objects
        if siblingIndex[i] >= index then
            siblingIndex[i] = siblingIndex[i] - 1
        end
    end

    -- If this Interval structure is already sorted,
    local sortedIndex = self.sortedIndex
    if self.sorted then
        for j=1,#sortedIndex do
            local items = sortedIndex[j]

            for i=numSiblings,1,-1 do -- At this point, #items == (numSiblings + 1)
                local val = items[i]
                if val > index then -- Decrement Index.
                    items[i] = val - 1
                elseif val == index then -- Remove
                    table_remove(items, i)
                end
            end

            local last = #items
            if last > 0 then -- Check the last axis index
                local val = items[last]
                if val > index then -- Decrement Index.
                    items[last] = val - 1
                elseif val == index then -- Remove
                    table_remove(items, last)
                end
            end
        end
    else
        for j=1,#sortedIndex do
            table_remove(sortedIndex[j], index)
        end
    end
end

--[[
    Remove an object from the axis interval. This entails deleting the indices
    that correspond to the provided object.
--]]
function Interval:RemoveObject(object)
    local index = self.objectLookup[object]
    if index then
        local sibling = self.siblingIndex[index]
        if index and sibling ~= -1 then
            self:Remove(((index > sibling and index) or sibling)) -- max first
            self:Remove(((index < sibling and index) or sibling))
        else
            self:Remove(index)
        end
    end
end

--[[
    Create an index, that represents Interval:values in sorted order, ensuring:
        GetValue(sortedIndex[i]) <= GetValue(sortedIndex[i + 1]) for all items in the array.
--]]
function Interval:SortIndex()
    local sortedIndex = self.sortedIndex
    if not self.sorted then
        table_sort(sortedIndex[1], self.comparators[1])
        table_sort(sortedIndex[2], self.comparators[2])
        table_sort(sortedIndex[3], self.comparators[3])
        self.sorted = true
    end
    return sortedIndex
end

--[[
    Create a 'sortedIndex' for *only* the the list of provided objects. These
    'objects' often belong to the same 'leaf node' of some spatial index.

@RETURN
    subSortedIndex -- The sorted axis values for each object
    objectCount -- The length of objects
    min,max -- the (minimally) encapsulated AABB dimensions of all the objects
--]]
function Interval:SubsortIndex(objects)
    local values = self.values

    local result = { {}, {}, {} }
    local objectCount,itemIndex = 0,1
    local min,max = vec3(glm_huge),vec3(-glm_huge)

    for i=1,#objects do
        local index = self.objectLookup[objects[i]]
        local sibling = self.siblingIndex[index]

        objectCount = objectCount + 1
        if sibling ~= -1 then -- AABB
            min = glm_min(min, values[index], values[sibling])
            max = glm_max(max, values[index], values[sibling])
            result[1][itemIndex] = index ; result[1][itemIndex + 1] = sibling
            result[2][itemIndex] = index ; result[2][itemIndex + 1] = sibling
            result[3][itemIndex] = index ; result[3][itemIndex + 1] = sibling

            itemIndex = itemIndex + 2
        else -- Point
            min = glm_min(min, values[index])
            max = glm_max(max, values[index])
            result[1][itemIndex] = index
            result[2][itemIndex] = index
            result[3][itemIndex] = index

            itemIndex = itemIndex + 1
        end
    end

    table_sort(result[1], self.comparators[1])
    table_sort(result[2], self.comparators[2])
    table_sort(result[3], self.comparators[3])
    return result,objectCount,min,max
end

----------------------------------------
------------- Partitioning -------------
----------------------------------------

--[[
    Partition this axis interval (sorted dimension of values) defined by a
    partitioning function.

@PARAMETERS
    itemSet - Items of the interval to process.
    partFunction - partFunction(object, axis, value)

    partitionResult - sink used for partitioning the axis items (on each dimension).
    axisObjectCount - number of partitioned object indices.
    minVal - absolute minimum bound.
    maxVal - absolute maximum bound.
--]]
function Interval:Partition(itemSet, partFunction, axis, intervalKey, value, result)
    local result = result or table_create(#itemSet, 0)
    local values,objects,nodeType = self.values,self.objects,self.nodeType

    local objCount,minVal,maxVal = 0,glm_huge,-glm_huge
    for i=1,#itemSet do
        local index = itemSet[i]
        if partFunction(self, objects[index], axis, value) then
            local val = values[index][intervalKey]
            minVal = glm_min(val, minVal)
            maxVal = glm_max(val, maxVal)

            result[#result + 1] = index
            if nodeType[index] == Interval.NodeType.Min or nodeType[index] == Interval.NodeType.Point then
                objCount = objCount + 1
            end
        end
    end

    return result,objCount,minVal,maxVal
end

--[[ Partition each axis interval defined by a partitioning function. --]]
function Interval:PartitionIntervals(axisItemSet, partFunction, axisKey, value, itemResult)
    if partFunction == nil then
        return nil,0,nil,nil
    end

    local _,nx,x1,x2 = self:Partition(axisItemSet[1], partFunction, axisKey, 1, value, itemResult[1])
    local _,ny,y1,y2 = self:Partition(axisItemSet[2], partFunction, axisKey, 2, value, itemResult[2])
    local _,nz,z1,z2 = self:Partition(axisItemSet[3], partFunction, axisKey, 3, value, itemResult[3])
    return itemResult,glm_max(nx, ny, nz),vec3(x1, y1, z1),vec3(x2, y2, z2)
end

----------------------------------------
------------ Axis Selection ------------
----------------------------------------
--
-- Axis selection approaches used by Default and Greedy partitioning
--

--[[
    Compute the interval statistics over the defined set of axis items (object
    indices defining some interval) in a single pass using Welfords algorithm.

@RETURN:
    mean - average interval location, with the location of an interval is its middle point.
    variance - the variance interval location.

@TODO:
    Introduce min/max/length/dispersion/entropy/etc. statistics.
--]]
function Interval:ComputeIntervalStatistics(items, axis)
    local values,siblings,nodeType = self.values,self.siblingIndex,self.nodeType

    --[[ In-line compute mean and standard deviation. --]]
    local k,m,m_last,s,s_last = 0,0.0,0.0,0.0,0.0
    for i=1,#items do
        local index = items[i]
        if nodeType[index] == Interval.NodeType.Min or nodeType[index] == Interval.NodeType.Point then
            local x = values[index][axis]
            if siblings[index] > 0 then
                x = 0.5 * (x + values[siblings[index]][axis])
            end

            m_last = (k == 0 and x) or m
            s_last = (k == 0 and 0.0) or s
            k = k + 1
            m = m_last + (x - m_last) / k
            s = s_last + (x - m_last) * (x - m)
        end
    end

    return m,(k < 2 and 0) or (s / k)
end

-- Axis selection approaches for greedy & selection threshold algorithms, i.e.,
-- only apply a thresholding algorithm to a single dimension based on one of
-- these properties.

--[[ Select the axis with the largest difference in maximum and minimum. --]]
function Interval:LargestInterval(itemSet, ignoreAxis)
    local values = self.values

    local cardinalAxis = nil
    local largestWidth = -glm_huge
    for i=1,3 do
        if ignoreAxis == nil or not ignoreAxis[i] then
            local items = itemSet[i]
            local width = values[items[#items]][i] - values[items[1]][i]
            if width > largestWidth then
                cardinalAxis = i
                largestWidth = width
            end
        end
    end
    return cardinalAxis
end

--[[ Select the axis with the smallest difference in maximum and minimum. --]]
function Interval:SmallestInterval(itemSet, ignoreAxis)
    local values = self.values

    local cardinalAxis = nil
    local smallestWidth = glm_huge
    for i=1,3 do
        if ignoreAxis == nil or not ignoreAxis[i] then
            local items = itemSet[i]
            local width = values[items[#items]][i] - values[items[1]][i]
            if width < smallestWidth then
                cardinalAxis = i
                smallestWidth = width
            end
        end
    end
    return cardinalAxis
end

--[[ Select the axis with the greatest amount of variability. --]]
function Interval:MostVariableInterval(itemSet, ignoreAxis)
    local cardinalAxis = nil
    local splitVariance = -glm_huge
    for i=1,3 do
        if ignoreAxis == nil or not ignoreAxis[i] then
            local mean,variance = self:ComputeIntervalStatistics(itemSet[i], i)
            if splitVariance < variance then
                cardinalAxis = i
                splitVariance = variance
            end
        end
    end
    return cardinalAxis
end

--[[ Select the axis with the smallest amount of variability. --]]
function Interval:LeastVariableInterval(itemSet, ignoreAxis)
    local cardinalAxis = nil
    local splitVariance = glm_huge
    for i=1,3 do
        if ignoreAxis == nil or not ignoreAxis[i] then
            local mean,variance = self:ComputeIntervalStatistics(itemSet[i], i)
            if variance < splitVariance then
                cardinalAxis = i
                splitVariance = variance
            end
        end
    end
    return cardinalAxis
end

----------------------------------------
------------ Axis Splitting ------------
----------------------------------------
--
-- Axis split-value approaches used by Default and Greedy partitioning
--

--[[ Halfway point between maximum and minimum values --]]
function Interval:EventSplit(itemSet, axis, _)
    local items = itemSet[axis]
    return 0.5 * (self.values[items[#items]][axis] + self.values[items[1]][axis])
end

--[[ Median axis value --]]
function Interval:MedianSplit(itemSet, axis, _)
    local items = itemSet[axis]
    local mid = #items >> 1
    if #items % 2 == 0 then
        return 0.5 * (self.values[items[mid]][axis] + self.values[items[mid + 1]][axis])
    else
        return self.values[items[mid + 1]][axis]
    end
end

--[[ Mean axis value --]]
function Interval:MeanSplit(itemSet, axis, _)
    local mean,_ = self:ComputeIntervalStatistics(itemSet[axis], axis)
    return mean
end

--[[
    Interval Count Heuristic: inspired by the surface area heuristic but applied
    to single axes.
--]]
function Interval:IntervalCount(itemSet, axis, objectCount)
    local values = self.values
    local nodeType = self.nodeType

    local items = itemSet[axis]
    local minItem = values[items[1]][axis]
    local maxItem = values[items[#items]][axis]
    if (maxItem - minItem) < glm_eps then
        return self:MedianSplit(itemSet, axis, objectCount)
    end

    -- Brute-force minimum cost.
    local invBoxWidth = 1.0 / (maxItem - minItem)

    local minID,minCost = nil,glm_huge
    local closedIntervals,openIntervals = 0,0
    for i=1,#items do
        local item = items[i]
        if nodeType[item] == Interval.NodeType.Max then
            openIntervals = openIntervals - 1
            closedIntervals = closedIntervals + 1
        end

        local alpha = (values[item][axis] - minItem) * invBoxWidth
        local cost = alpha * (closedIntervals + openIntervals) + (1.0 - alpha) * (objectCount - closedIntervals)
        if cost < minCost then
            minID,minCost = i,cost
        end

        if nodeType[item] == Interval.NodeType.Min then
            openIntervals = openIntervals + 1
        end
    end

    local minItem = items[minID]
    if minID == nil or minID == minItem or minID == maxItem then
        return self:MedianSplit(itemSet, axis, objectCount)
    end
    return values[minItem][axis]
end

---------------------------------------
------------ Axis Strategy ------------
---------------------------------------

--[[
    Count the number of valid objects that satisfy the provided partitioning
    function: partFunction(object, axis, value)
--]]
function Interval:CountPartition(partitionings, itemSet, axis, value)
    local lower,upper,mid = partitionings.Lower,partitionings.Upper,partitionings.Mid
    local objects,nodeType = self.objects,self.nodeType

    local lowCount,upCount,midCount = 0,0,0
    local items = itemSet[axis]
    for i=1,#items do
        local item = items[i]

        local object,valueType = objects[item],nodeType[item]
        if valueType == Interval.NodeType.Min or valueType == Interval.NodeType.Point then
            lowCount = lowCount + ((lower(self, object, axis, value) and 1) or 0)
            upCount = upCount + ((upper(self, object, axis, value) and 1) or 0)
            if mid then
                midCount = midCount + ((mid(self, object, axis, value) and 1) or 0)
            end
        end
    end

    return lowCount,upCount,midCount
end

--[[
    Default: run the selector and filter it through the splitter; with no
    additional caveats.
--]]
function Interval.DefaultStrategy(selector, splitter)
    return function(self, partitionings, itemSet, objCount)
        local axis = selector(self, itemSet, processedAxis)
        local value = (axis and splitter(self, itemSet, axis, objCount)) or nil
        if axis and value then
            local low,high,mid = self:CountPartition(partitionings, itemSet, axis, value)
            if objCount ~= low and objCount ~= high and objCount ~= mid then
                return axis,value
            end
        end
        return nil,nil
    end
end

--[[
    Select the first axis/value tuple that does not partition all of the objects
    into the same subset/partition. The ordering/heuristic of which axis to
    select and the axis-split algorithm are parameters
--]]
function Interval.GreedyStrategy(selector, splitter)
    local processedAxis = table_create(3, 0)
    return function(self, partitionings, itemSet, objCount)
        interval_debug = true

        processedAxis[1],processedAxis[2],processedAxis[3] = false,false,false
        for i=1,3 do
            local axis = selector(self, itemSet, processedAxis)
            local value = (axis and splitter(self, itemSet, axis, objCount)) or nil
            if axis and value then
                local low,high,mid = self:CountPartition(partitionings, itemSet, axis, value)

                -- Ensure the count is not equal to the entire object.
                if objCount ~= low and objCount ~= high and objCount ~= mid then
                    interval_debug = false
                    return axis,value
                end
            end
            processedAxis[axis] = true
        end
        interval_debug = false
        return nil,nil
    end
end

--[[
    For a given axis-splitting algorithm, choose the axis & value that 'best'
    (most evenly) partitions the axisIntervals.
--]]
function Interval.BestStrategy(splitter)
    return function(self, partitionings, itemSet, objCount)
        local hasMid = partitionings.Mid ~= nil

        local minAxis,minValue,minCost = nil,nil,glm_huge
        for axis=1,3 do
            local value = splitter(self, itemSet, axis, objCount)
            if value then
                local low,high,mid = self:CountPartition(partitionings, itemSet, axis, value)
                if low ~= objCount and high ~= objCount and mid ~= objCount then
                    local lcount = low + mid
                    local hcount = high + mid
                    local lcost = lcount == 0 and 0 or (lcount * glm_log(lcount) / (lcount + hcount))
                    local hcost = hcount == 0 and 0 or (hcount * glm_log(hcount) / (lcount + hcount))
                    if (lcost + hcost) < minCost then
                        minCost,minAxis,minValue = (lcost + hcost),axis,value
                    end
                end
            end
        end
        return minAxis,minValue,minCost
    end
end

--[[ BestStrategy applied to every axis-splitting algorithm. --]]
function Interval.DynamicStrategy()
    local Splitting = {
        { "EventSplit", Interval.BestStrategy(Interval.EventSplit), },
        { "MedianSplit", Interval.BestStrategy(Interval.MedianSplit), },
        { "MeanSplit", Interval.BestStrategy(Interval.MeanSplit), },
        { "IntervalCount", Interval.BestStrategy(Interval.IntervalCount) },
    }

    return function(self, partitionings, itemSet, objCount)
        local minCost,minAxis,minValue = glm_huge,nil,nil
        for i=1,#Splitting do
            local axis,value,cost = Splitting[i][2](self, partitionings, itemSet, objCount)
            if axis and value and cost < minCost then
                minCost,minAxis,minValue = cost,axis,value
            end
        end
        return minAxis,minValue
    end
end

--[[
    Wikipedia (https://en.wikipedia.org/wiki/Bounding_interval_hierarchy):

    "Calculate the surface area and number of objects for both children, over
    the set of all possible split plane candidates, then choose the one with the
    lowest costs (claimed to be optimal, though the cost function poses unusual
    demands to proof the formula, which can not be fulfilled in real life. also
    an exceptionally slow heuristic to evaluate)"
--]]
function Interval.SurfaceAreaHeuristic()
    return function(self, partitionings, itemSet, objCount)
        local NodeType_Min = Interval.NodeType.Min
        local SAH_Traversal = Interval.SAH_Traversal
        local SAH_Intersection = Interval.SAH_Intersection
        local minBounds,maxBounds,nodeType,objects =
            self.minBounds,self.maxBounds,self.nodeType,self.objects

        local iobjects,areaLess = {},{}

        -- Collect all objects within the current itemSet.
        local _items = itemSet[1]
        for i=1,#_items do
            local item = _items[i]
            if nodeType[item] == NodeType_Min then
                iobjects[#iobjects + 1] = objects[item]
            end
        end

        local ocount = #iobjects
        local minAxis,minValue,minCost = nil,nil,glm_huge
        for axis=1,3 do
            -- Sort all objects based on their centroid positions on the current
            -- projected axis.
            table_sort(iobjects, function(oa, ob)
                local acenter = 0.5 * (minBounds[oa] + maxBounds[oa])
                local bcenter = 0.5 * (minBounds[ob] + maxBounds[ob])
                return acenter[axis] < bcenter[axis]
            end)

            -- Compute the surface areas for all 'less-than' objects
            local lmin,lmax = vec3(glm_huge),vec3(-glm_huge)
            for i=1,ocount do
                local o = iobjects[i]
                lmin,lmax = glm_aabb_encloseAABB(lmin, lmax, minBounds[o], maxBounds[o])
                areaLess[i] = glm_aabb_surfaceArea(lmin, lmax)
            end

            local int_factor = SAH_Intersection / glm_aabb_surfaceArea(lmin, lmax)

            -- Compute the surface are area for all 'greater-than' objects
            local rmin,rmax = vec3(glm_huge),vec3(-glm_huge)
            for i=ocount-1,2,-1 do
                local o = iobjects[i]
                rmin,rmax = glm_aabb_encloseAABB(rmin, rmax, minBounds[o], maxBounds[o])

                local la = areaLess[i - 1]
                local ra = glm_aabb_surfaceArea(rmin, rmax)
                local cost = 2.0 * SAH_Traversal + int_factor * ((i * la) + ((ocount - i) * ra))
                if cost < minCost then
                    minAxis = axis
                    minCost = cost
                    minValue = maxBounds[o][axis]
                end
            end
        end

        -- Ensure the partitioning divides objects; otherwise assume the data is
        -- identical and belongs to the same node.
        if minAxis ~= nil then
            local low,high,mid = self:CountPartition(partitionings, itemSet, minAxis, minValue)
            if low ~= objCount and high ~= objCount and mid ~= objCount then
                return minAxis,minValue,minCost
            end
        end
        return nil,nil,minCost
    end
end

return Interval
