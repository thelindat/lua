local type = type
local pairs = pairs
local table = table
local string = string
local math = math

--[[ Print "help" message and any error messages --]]
function PrintUsage(errorMessage)
    print([[
sort [--output] [-s | --seed] [-i | --iters] [-h | --help]

[INPUT]
  --output, -o: header files output location (optional, default '.')

[OPTIONS]
  --seed, -s: seed to initialize the pseudo-random number generator
  --iters, -i: maximum number of invalid shuffle attempts before throwing an error
]])

    if errorMessage then
        error(tostring(errorMessage))
    end
end

--[[ http://lua-users.org/wiki/AlternativeGetOpt --]]
function ParseArguments(arg, optionString)
    local options = setmetatable({ }, { __index = {
        String = function(self, long, short, default)
            local o = self[long] or self[short] or default
            return (type(o) == "string" and o) or nil
        end,

        Int = function(self, long, short, default)
            local o = self[long] or self[short] or default
            return (type(o) == "number" and o) or tonumber(o)
        end,

        Bool = function(self, long, short, default)
            local o = self[long] or self[short]
            if o == nil then
                o = default
            end

            if type(o) == "boolean" then
                return o
            elseif type(o) == "string" then
                return string.lower(o) ~= "false"
            end
            return default
        end,
    }})

    for k, v in ipairs(arg) do
        if v:sub(1, 2) == "--" then
            local x = v:find("=", 1, true)
            if x then
                options[v:sub(3, x - 1)] = v:sub(x + 1)
            else
                options[v:sub(3)] = true
            end
        elseif v:sub(1, 1) == "-" then
            local y = 2
            local l = v:len()
            while (y <= l) do
                local shortOption = v:sub(y, y)
                if optionString:find(shortOption, 1, true) then
                    if y < l then
                        options[shortOption] = v:sub(y + 1)
                        y = l
                    else
                        options[shortOption] = arg[k + 1]
                    end
                else
                    options[shortOption] = true
                end
                y = y + 1
            end
        end
    end
    return options
end

-------------------------------------------------------------------------------
---------------------------------- Execution ----------------------------------
-------------------------------------------------------------------------------

--[[ Load Operands --]]
local Lua = require('shuffle.opcodes')

local options = ParseArguments(arg, "")
if options:Bool("help", "h", false) then
    PrintUsage()
    return
end

local baseDirectory = options:String("output", "o", ".")
local shuffleIters = math.abs(options:Int("iters", "i", 25))
local randomSeed = options:Int("seed", "s", nil)
if randomSeed then
    math.randomseed(randomSeed)
end

---------------------------------------
--------------- Utility ---------------
---------------------------------------

--[[ Remove leading and trailing characters from a string according to some pattern (defaulting to: %s+) --]]
function string:trim(r)
   r = r or '%s+'
   return (string.gsub(string.gsub(self, '^' .. r, ''), r .. '$', ''))
end

--[[ Clone & shuffle an array --]]
function table:shuffle()
    local t = { }
    for i=1,#self do
        t[#t + 1] = self[i]
    end

    local j
    local iterations = #t
    for i = iterations, 2, -1 do
        j = math.random(i)
        t[i], t[j] = t[j], t[i]
    end

    return t
end

--[[ Append a source array to a sink according to some order --]]
function table:ordered_append(source, order)
    for i=1,#order do
        local value = source[order[i]]
        if string.trim(value) ~= "" then
            self[#self + 1] = value
        end
    end
end

--[[ Generate a sequence of integers corresponding to the shuffle index. --]]
function table.sequence(n)
    local t = { }
    for i=1,n do t[i] = i end
    return t
end

--[[
    Shuffle an array until either the condition-callback returns true or a
    maximum number of iterations has occurred.
--]]
function table.iterative_shuffle(sequence, maxIterations, condition)
    maxIterations = maxIterations or 1000

    local iterations = 0
    while not valid and iterations < maxIterations do
        iterations = iterations + 1
        local shuffle_next = table.shuffle(sequence)
        if condition(shuffle_next) then
            return shuffle_next
        end
    end

    return nil
end

-------------------------------------------------------------------------------
---------------------------------- Groupings ----------------------------------
-------------------------------------------------------------------------------

--[[ string is a preprocessor directive --]]
local function IsPreprocessorDirective(str)
    local prefix = "#if"
    return string.sub(str, 1, #prefix) == prefix
end

-- Generate a sequence of integers -- corresponding to the shuffle index -- and
-- sanitize:
--  1. Ensure subtables are of the same size;
--  2. Ensure all shared enums have their tables/subtables of the same size.
--     Using empty strings to denote empty space (trimmed on output)
local groupIndices = { }
for i=1,#Lua.Group.Order do
    local groupLabel = Lua.Group.Order[i]
    local group = Lua.Group[groupLabel]

    local groupLength = 0
    local checkGroupLength = function(groupLabel, name, len)
        if groupLabel == "Unsorted" then
            return
        elseif groupLength == 0 then
            groupLength = len
        elseif groupLength ~= len then
            local groupError = "Invalid CodeBlock %s<%s> %d %d"
            error(groupError:format(groupLabel, name, groupLength, len))
        end
    end

    local grouping = { }
    for name,header in pairs(group) do
        if type(Lua.Headers[name]) == "table" then
            local subGrouping = { }

            -- Ensure all substructs are of the same size
            local subGroupLength = 0
            for enumType,_ in pairs(Lua.Headers[name]) do
                local subHeader = header[enumType]
                if subGroupLength == 0 then
                    subGroupLength = #subHeader
                elseif subGroupLength ~= #subHeader then
                    local groupError = "Substructure %s<%s> %d %d"
                    error(groupError:format(groupLabel, enumType, subGroupLength, #subHeader))
                end
            end

            checkGroupLength(groupLabel, name, subGroupLength)
            grouping[name] = table.sequence(subGroupLength)
        else
            checkGroupLength(groupLabel, name, #header)
            grouping[name] = table.sequence(#header)
        end
    end

    if groupLabel == "Unsorted" then
        groupIndices[groupLabel] = grouping
    else
        groupIndices[groupLabel] = table.sequence(groupLength)
    end
end

--------------------
---- Local Sort ----
--------------------

-- Locally sort each grouping
local groupShuffled = { }
for i=1,#Lua.Group.Order do
    local groupLabel = Lua.Group.Order[i]
    local group = Lua.Group[groupLabel]
    local groupIndices = groupIndices[groupLabel]

    -- Rules:
    --  1. A shuffling cannot begin or end with an empty string or a preprocessor
    --  directive as those values are often used in mapping one enumeration to
    --  another.
    local sanitizeHeader = function(header, s)
        return string.trim(header[s[1]]) ~= ""
            and string.trim(header[s[#s]]) ~= ""
            and not IsPreprocessorDirective(header[s[1]])
            and not IsPreprocessorDirective(header[s[#s]])
    end

    -- For the "Unsorted" grouping, locally sort each enumeration as there is no
    -- dependency between enumerations in the group. Avoid the 'pairs' metamethod
    -- for determinism reasons
    if groupLabel == "Unsorted" then
        local grouping = { }
        for j=1,#Lua.Order do
            local headerName = Lua.Order[j]
            local header = group[headerName]
            if header ~= nil then
                local unsortedIndices = groupIndices[headerName]
                local shuffled = table.iterative_shuffle(unsortedIndices, shuffleIters, function(shuffled)
                    local valid = true
                    if #shuffled == 0 then -- Do nothing
                    elseif type(Lua.Headers[headerName]) == "table" then
                        for enumType,_ in pairs(Lua.Headers[headerName]) do
                            valid = valid and sanitizeHeader(header[enumType], shuffled)
                        end
                    else
                        valid = valid and sanitizeHeader(header, shuffled)
                    end
                    return valid
                end)

                if shuffled then
                    grouping[headerName] = shuffled
                else
                    error(("Could not find valid shuffling: %s"):format(groupLabel))
                end
            end
        end

        groupShuffled[groupLabel] = grouping

    -- If the grouping is flagged for shuffling, shuffle its ordering sequence
    -- and verify its a valid shuffling.
    elseif Lua.Group.Shuffle[groupLabel] then
        local shuffled = table.iterative_shuffle(groupIndices, shuffleIters, function(shuffled)
            local valid = true
            for name,header in pairs(group) do
                if type(Lua.Headers[name]) == "table" then
                    for enumType,_ in pairs(Lua.Headers[name]) do
                        valid = valid and sanitizeHeader(header[enumType], shuffled)
                    end
                else
                    valid = valid and sanitizeHeader(header, shuffled)
                end
            end
            return valid
        end)

        if shuffled then
            groupShuffled[groupLabel] = shuffled
        else
            error(("Could not find valid shuffling: %s"):format(groupLabel))
        end

    -- Otherwise: do not sort
    else
        groupShuffled[groupLabel] = groupIndices
    end
end

-------------------
---- Aggregate ----
-------------------

-- Randomize the order in which headers are generated
local LuaHeaderOrder = table.shuffle(Lua.Order)
local LuaGroupOrder = table.shuffle(Lua.Group.Order)

-- 1. Ensure "Unary" always comes after "Binary" in the grouping layout
-- 2. Have FastAccess as the first sequence as a performance tweak for ltm_maskflags
for i=#LuaGroupOrder,1,-1 do
    if LuaGroupOrder[i] == "Binary" then
        table.insert(LuaGroupOrder, i + 1, "Unary")
    elseif LuaGroupOrder[i] == "Unary" or LuaGroupOrder[i] == "FastAccess" then
        table.remove(LuaGroupOrder, i)
    end
end
table.insert(LuaGroupOrder, 1, "FastAccess")

-- Remove K-operands
for i=#LuaHeaderOrder,1,-1 do
    if Lua.KOperands.Base[LuaHeaderOrder[i]] ~= nil then
        table.remove(LuaHeaderOrder, i)
    end
end

-- Each header maintains a list of functions when invoked will unpack all strings
-- of the grouping related to the enum.
local LuaSortedHeaders = { }
for headerName,header in pairs(Lua.Headers) do
    LuaSortedHeaders[headerName] = { }
    if type(header) == "table" then
        for enumType,_ in pairs(header) do
            LuaSortedHeaders[headerName][enumType] = { }
        end
    end
end

for i=1,#LuaHeaderOrder do
    local headerName = LuaHeaderOrder[i]
    local headerSorted = LuaSortedHeaders[headerName]
    if Lua.KOperands.Base[headerName] ~= nil then
        headerSorted = LuaSortedHeaders[Lua.KOperands.Base[headerName]]
    end

    local header = Lua.Headers[headerName]
    for j=1,#LuaGroupOrder do
        local groupLabel = LuaGroupOrder[j]
        local group = Lua.Group[groupLabel]
        local groupShuffled = groupShuffled[groupLabel]
        if groupLabel == "Unsorted" then
            groupShuffled = groupShuffled[headerName]
        end

        if not group[headerName] then
            -- Continue: group doesn't sort within that header
        elseif type(header) == "table" then
            for enumType,_ in pairs(header) do
                local subHeader = headerSorted[enumType]
                subHeader[#subHeader + 1] = function(sink)
                    -- K-operands always precede base operands.
                    local kHeader = Lua.KOperands.Lookup[headerName]
                    if kHeader ~= nil and group[kHeader] then
                        table.ordered_append(sink, group[kHeader][enumType], groupShuffled)
                    end
                    table.ordered_append(sink, group[headerName][enumType], groupShuffled)
                end
            end
        else
            headerSorted[#headerSorted + 1] = function(sink)
                local kHeader = Lua.KOperands.Lookup[headerName]
                if kHeader ~= nil and group[kHeader] then
                    table.ordered_append(sink, group[kHeader], groupShuffled)
                end
                table.ordered_append(sink, group[headerName], groupShuffled)
            end
        end
    end
end

-- @TODO: Eventually compress this logic into the loop above.
local globalHeaders = { }
for headerName,header in pairs(Lua.Headers) do
    globalHeaders[headerName] = { }

    if type(header) == "table" then
        for enumType,enumValue in pairs(header) do
            globalHeaders[headerName][enumType] = { }

            local list = LuaSortedHeaders[headerName][enumType]
            for i=1,#list do
                list[i](globalHeaders[headerName][enumType])
            end
        end
    else
        local list = LuaSortedHeaders[headerName]
        for i=1,#list do
            list[i](globalHeaders[headerName])
        end
    end
end

--------------------
----- Generate -----
--------------------

local function macro_value(value)
    return string.gsub(value, "[,|/](.*)" .. '$', '')
end

--[[ @TODO: hack to avoid GCC warnings w/ (x <= o) && (o <= y) rules, where x == 0 --]]
local function macro_range(name, first, last)
    return ("#define %s(o) (%s <= (o) && (o) <= %s)"):format(name, macro_value(first), macro_value(last))
end

local function WriteString(directory, file, outputString)
    local directory = (directory and (directory .. "/")) or ""

    local fh = assert(io.open(directory .. file, "w"))
    fh:write(outputString)
    fh:write("\n")
    fh:flush()
    fh:close()
end

for headerName,header in pairs(Lua.Headers) do
    if type(header) == "table" then
        for subEnum,enumValue in pairs(header) do
            local list = globalHeaders[headerName][subEnum]
            WriteString(baseDirectory, enumValue, table.concat(list, "\n"))
        end
    elseif type(header) == "string" then
        local list = globalHeaders[headerName]
        if headerName == "LuaOp" then
            local formatted = { }
            for i=1,#list do -- Assign integers to the #defines
                formatted[i] = ("#define %s %d"):format(list[i], i - 1)
            end
            WriteString(baseDirectory, header, table.concat(formatted, "\n"))
        elseif headerName == "Tests" then
            local fmt = "static const char ops[] = \"%s\";"
            WriteString(baseDirectory, header, fmt:format(table.concat(list, "")))
        else
            WriteString(baseDirectory, header, table.concat(list, "\n"))
        end
    else
        error("Unexpected header value")
    end
end

-- Create Jumptable
local opcodes = table.concat(globalHeaders.Instructions.Codes, "\n")
WriteString(baseDirectory, Lua.Headers.Jumptable, opcodes:gsub("OP_", "&&L_OP_"))

-------------------
----- Mapping -----
-------------------

-- ldebug_info.h
--do
--    local lines = { }
--    lines[#lines + 1] = "/* Convey shuffling information */"
--    lines[#lines + 1] = "#define LUA_SHUFFLED 1"
--    WriteString(baseDirectory, "ldebug_info.h", table.concat(lines, "\n"))
--end

-- ltm_tmfast.h
do
    local group = Lua.Group.FastAccess.TaggedMethods
    local fastOrder = groupShuffled["FastAccess"]

    local lines = { }
    lines[#lines + 1] = "/* ORDER TM: Used to simplify logic in ltm.c: luaT_trybinTM */"
    lines[#lines + 1] = macro_range("tmfast", group.Opr[fastOrder[1]], group.Opr[fastOrder[#fastOrder]])
    lines[#lines + 1] = ""
    lines[#lines + 1] = [[
/*
** Unfolded tmfast:
** #define tmfast(o) (                                        \
**   TM_INDEX == (o) || TM_NEWINDEX == (o) || TM_GC == (o) || \
**   TM_MODE == (o) || TM_LEN == (o) || TM_EQ == (o)          \
** )
*/]]
    lines[#lines + 1] = ""

    WriteString(baseDirectory, "ltm_tmfast.h", table.concat(lines, "\n"))
end

-- ltm_maskflags.
do
    local lines = { }
    lines[#lines + 1] = [[
/*
** Mask with 1 in all fast-access methods. A 1 in any of these bits
** in the flag of a (meta)table means the metatable does not have the
** corresponding metamethod field. (Bit 7 of the flag is used for
** 'isrealasize'.)
*/]]

    lines[#lines + 1] = "#define maskflags (~(~0u << (5 + 1)))"
    lines[#lines + 1] = ""
    lines[#lines + 1] = "/* 1<<p means tagmethod(p) is not present */"
    lines[#lines + 1] = "#define tagmethod(p) cast_byte(1u << (p))"
    WriteString(baseDirectory, "ltm_maskflags.h", table.concat(lines, "\n"))
end

-- ltm_bitop.h
do
    local lines = { }
    lines[#lines + 1] = "/*#define tmbitop(o) ((TM_BAND <= (o) && (o) <= TM_SHR) || (o) == TM_BNOT) */"
    lines[#lines + 1] = ""
    lines[#lines + 1] = "/* Unfolded timbitop */"
    lines[#lines + 1] = [[
    #define tmbitop(o)  (                                                    \
      TM_BAND == (o) || TM_BOR == (o) || TM_BXOR == (o) || TM_BNOT == (o) || \
      TM_SHL == (o) || TM_SHR == (o)                                         \
    )
]]
    lines[#lines + 1] = ""

    WriteString(baseDirectory, "ltm_bitop.h", table.concat(lines, "\n"))
end

-- lopcodes_numopcode.h
do
    local lua_numop = "#define NUM_OPCODES ((int)(%s) + 1)"
    local codes = globalHeaders.Instructions.Codes
    WriteString(baseDirectory, "lopcodes_numopcode.h", lua_numop:format(macro_value(codes[#codes])))
end

-- lobject_loptotms.h
do
    local luaop_to_tms = "#define luaop_to_tms(op) cast(TMS, ((op) - %s) + %s)"
    local binaryOrder = groupShuffled["Binary"]
    WriteString(baseDirectory, "lobject_loptotms.h", luaop_to_tms:format(
        macro_value(Lua.Group.Binary.LuaOp[binaryOrder[1]]),
        macro_value(Lua.Group.Binary.TaggedMethods.Opr[binaryOrder[1]])
    ))
end

-- lcode_opvalid.h
do
    local luaop_valid = "#define luaop_valid(o) (%s <= (o) && (o) <= %s)"
    local codes = globalHeaders.Instructions.Codes
    WriteString(baseDirectory, "lcode_opvalid.h", luaop_valid:format(
        macro_value(codes[1]),
        macro_value(codes[#codes])
    ))
end

-- lcode_opfold.h
do
    local binaryOrder = groupShuffled["Binary"]

    local lines = { }
    lines[#lines + 1] = "/* true if opcode is foldable (that is, it is arithmetic or bitwise) */"
    lines[#lines + 1] = macro_range("luaop_fold",
        Lua.Group.Binary.Instructions.Codes[binaryOrder[1]],
        Lua.Group.Binary.Instructions.Codes[binaryOrder[#binaryOrder]]
    )
    lines[#lines + 1] = ""
    lines[#lines + 1] = [[
/*
** Unfolded foldop:
** #define luaop_fold(o) (                                                 \
**   OP_ADD == (o) || OP_SUB == (o) || OP_MUL == (o) || OP_MOD == (o) ||   \
**   OP_POW == (o) || OP_DIV == (o) || OP_IDIV == (o) || OP_BAND == (o) || \
**   OP_BOR == (o) || OP_BXOR == (o) || OP_SHL == (o) || OP_SHR == (o)     \
** )
*/]]

    WriteString(baseDirectory, "lcode_opfold.h", table.concat(lines, "\n"))
end

-- lcode_foldbinop.h
do
    local binaryOrder = groupShuffled["Binary"]

    local lines = { }
    lines[#lines + 1] = "/* true if operation is foldable (that is, it is arithmetic or bitwise) */"
    lines[#lines + 1] = macro_range("foldbinop",
        Lua.Group.Binary.BinaryOps.Opr[binaryOrder[1]],
        Lua.Group.Binary.BinaryOps.Opr[binaryOrder[#binaryOrder]]
    )
    lines[#lines + 1] = ""
    lines[#lines + 1] = [[
/*
** Unfolded-foldbinop
** #define foldbinop(op) (                                                       \
**   OPR_ADD == (op) || OPR_SUB == (op) || OPR_MUL == (op) || OPR_MOD == (op) || \
**   OPR_POW == (op) || OPR_DIV == (op) || OPR_IDIV == (op) ||                   \
**   OPR_BAND == (op) || OPR_BOR == (op) || OPR_BXOR == (op) ||                  \
**   OPR_SHL == (op) || OPR_SHR == (op)                                          \
** )
*/]]

    WriteString(baseDirectory, "lcode_foldbinop.h", table.concat(lines, "\n"))
end

-- lcode_map.h
do
    local opcode_to_tms = "#define opcode_to_tms(op) cast(TMS, ((op) - %s) + %s)"
    local binopr_to_tms = "#define binopr_to_tms(opr) cast(TMS, ((opr) - %s) + %s)"
    local binopr_to_luaop = "#define binopr_to_luaop(opr) (((opr) - %s) + %s)"
    local unopr_to_luaop = "#define unopr_to_luaop(op) (((op) - %s) + %s)"
    local binopr_to_kopcode = "#define binopr_to_kopcode(opr) cast(OpCode, ((opr) - %s) + %s)"
    local binopr_to_opcode = "#define binopr_to_opcode(opr) cast(OpCode, ((opr) - %s) + %s)"
    local unopr_to_opcode = "#define unopr_to_opcode(op) cast(OpCode, ((op) - %s) + %s)"

    -- Comparison groupings are not sorted (yet)
    local cmpbinopr_to_opcode = "#define cmpbinopr_to_opcode(opr) cast(OpCode, ((opr) - OPR_EQ) + OP_EQ)"
    local ncmpbinopr_to_opcode = "#define ncmpbinopr_to_opcode(opr) cast(OpCode, ((opr) - OPR_NE) + OP_EQ)"
    local cmpbinopr_to_iopcode = "#define cmpbinopr_to_iopcode(op) cast(OpCode, ((op) - OP_LT) + OP_LTI)"

    local unaryOrder = groupShuffled["Unary"]
    local binaryOrder = groupShuffled["Binary"]

    local unary_unaryops_b = macro_value(Lua.Group.Unary.UnaryOps[unaryOrder[1]])
    local unary_luaop_b = macro_value(Lua.Group.Unary.LuaOp[unaryOrder[1]])
    local unary_code_b = macro_value(Lua.Group.Unary.Instructions.Codes[unaryOrder[1]])

    local binary_code_b = macro_value(Lua.Group.Binary.Instructions.Codes[binaryOrder[1]])
    local binary_kcode_b = macro_value(Lua.Group.Binary.KInstructions.Codes[binaryOrder[1]])
    local binary_binaryop_b = macro_value(Lua.Group.Binary.BinaryOps.Opr[binaryOrder[1]])
    local binary_tm_b = macro_value(Lua.Group.Binary.TaggedMethods.Opr[binaryOrder[1]])
    local binary_luaop_b = macro_value(Lua.Group.Binary.LuaOp[binaryOrder[1]])

    local lines = { }
    lines[#lines + 1] = "/* Mapping binary operators to other enumerators */"
    lines[#lines + 1] = opcode_to_tms:format(binary_code_b, binary_tm_b)
    lines[#lines + 1] = binopr_to_tms:format(binary_binaryop_b, binary_tm_b)
    lines[#lines + 1] = binopr_to_luaop:format(binary_binaryop_b, binary_luaop_b)
    lines[#lines + 1] = unopr_to_luaop:format(unary_unaryops_b, unary_luaop_b)
    lines[#lines + 1] = binopr_to_kopcode:format(binary_binaryop_b, binary_kcode_b)
    lines[#lines + 1] = binopr_to_opcode:format(binary_binaryop_b, binary_code_b)
    lines[#lines + 1] = unopr_to_opcode:format(unary_unaryops_b, unary_code_b)
    lines[#lines + 1] = ""
    lines[#lines + 1] = cmpbinopr_to_opcode
    lines[#lines + 1] = ncmpbinopr_to_opcode
    lines[#lines + 1] = cmpbinopr_to_iopcode
    WriteString(baseDirectory, "lcode_map.h", table.concat(lines, "\n"))
end
