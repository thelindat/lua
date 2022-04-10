-- $Id: testes/glm.lua $
-- See Copyright Notice in file all.lua
-- @TODO: Eventually merge repository of other test scripts

print("testing glm lib")
local function _eq(x, y) return x == y end
local function _meq(x, y) return x == y and math.type(x) == math.type(y) end

local v3 = vec(1, 2, 3)
local v4 = vec(1, 2, 3, 4)
local q = quat(0.953717, 0.080367, 0.160734, 0.241101)

local c1 = vec(1, 2, 3)
local c2 = vec(4, 5, 6)
local c3 = vec(7, 8, 9)
local c4 = vec(10, 11, 12)
local mt = debug.getmetatable(mat(c1, c2, c3, c4)) -- Save previous matrix metatable

------------------------------------------
-- lmathlib string coercion consistency --
------------------------------------------

if glm then
  print("lmathlib string coercion consistency")

  assert(_meq(math.abs("-1"), glm.abs("-1")))
  assert(_meq(math.acos("0.5"), glm.acos("0.5")))
  assert(_meq(math.asin("0.5"), glm.asin("0.5")))
  assert(_meq(math.atan("0.5"), glm.atan("0.5")))

  assert(_meq(math.ceil("0.5"), glm.ceil("0.5")))
  assert(_meq(math.floor("1.5"), glm.floor("1.5")))
  assert(_meq(math.tointeger("3.0"), glm.tointeger("3.0")))

  assert(_meq(math.cos("0.78539816339745"), glm.cos("0.78539816339745")))
  assert(_meq(math.sin("0.78539816339745"), glm.sin("0.78539816339745")))
  assert(_meq(math.tan("0.78539816339745"), glm.tan("0.78539816339745")))
  assert(_meq(math.deg("0.78539816339745"), glm.deg("0.78539816339745")))
  assert(_meq(math.rad("45.0"), glm.rad("45.0")))
  assert(_meq(math.rad("45"), glm.rad("45")))

  assert(_meq(math.sqrt("5"), glm.sqrt("5")))
  assert(_meq(math.exp("3"), glm.exp("3")))
  assert(_meq(math.log("2"), glm.log("2")))

  local a,b = math.modf("8.275")
  local x,y = glm.modf("8.275")

  assert(_meq(a, x) and _meq(b, y))
  assert(_meq(math.fmod("8", "5"), glm.fmod("8", "5")))
  assert(_meq(math.max("1", "4", "3", "2"), glm.max("1", "4", "3", "2")))
  assert(_meq(math.min("1", "4", "3", "2"), glm.min("1", "4", "3", "2")))
end

---------------------------------------
---------- gettable/settable ----------
---------------------------------------
print("gettable/settable")

do
  local x, y, z = T.testC("gettable 2; pushvalue 4; gettable 2; pushvalue 3; gettable 2; return 3", v3, "z", "y", "x")
  assert(_eq(v3.x, x) and _eq(v3.y, y) and _eq(v3.z, z))
end

do
  local x, y = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", v4, "y", "x")
  local z, w = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", v4, "w", "z")
  assert(_eq(v4.x, x) and _eq(v4.y, y) and _eq(v4.z, z) and _eq(v4.w, w))
end

do
  local x, y = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", q, "y", "x")
  local z, w = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", q, "w", "z")
  assert(_eq(q.x, x) and _eq(q.y, y) and _eq(q.z, z) and _eq(q.w, w))
end

do
  local m = mat(c1, c2, c3, c4)
  local x1,x2 = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", m, 2, 1)
  local x3,x4 = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", m, 4, 3)
  assert(_eq(c1, x1) and _eq(c2, x2) and _eq(c3, x3) and _eq(c4, x4))
end

do -- Invalid gettable access.
  local v2 = vec2(1, 2)
  local x, y = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", v2, "y", "x")
  local z, w = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", v2, "w", "z")
  assert(_eq(v2.x, x) and _eq(v2.y, y) and z == nil and w == nil)
end

do
  local v2 = vec2(1, 2)
  local x, y = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", v2, 2, 1)
  local z, w = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", v2, 4, 3)
  assert(_eq(v2.x, x) and _eq(v2.y, y) and z == nil and w == nil)
end

do -- gettable access if the vector metatable is GLM
  local v2 = vec2(1, 2)
  if glm ~= nil and debug.getmetatable(v2) == glm then
    local abs = T.testC("gettable 2; return 1", v2, "abs")
    assert(abs == glm.abs)
  end
end

do
  -- As metatables exist for the entire matrix type; set it once.
  local sanitizeIndex = false
  debug.setmetatable(mat(c1, c2, c3, c4), {
    __index = function(self, k)
      if type(k) == "string" then
        return self[tonumber(k)]
      elseif type(k) == "number" and sanitizeIndex then
        local idx = math.max(1, math.min(#self, math.floor(k)))
        return self[idx]
      end
      return nil
    end,

    __newindex = function(self, k, v)
      if sanitizeIndex then
        local idx = math.max(1, math.min(#self, math.floor(k)))
        rawset(self, idx, v)
      end
    end,
  })

  do
    local m = mat(c1, c2, c3, c4)
    local x1,x2 = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", m, "2", "1")
    local x3,x4 = T.testC("gettable 2; pushvalue 3; gettable 2; return 2", m, "4", "3")
    assert(_eq(c1, x1) and _eq(c2, x2) and _eq(c3, x3) and _eq(c4, x4))
    assert(m[1] == m["1"] and m[2] == m["2"] and m[3] == m["3"] and m[4] == m["4"])
    assert(m[m] == nil)
    assert(m[-1] == nil)
    assert(m[0] == nil)
    assert(m[5] == nil)

    sanitizeIndex = true
    assert(_eq(c1, m[-1]))
    assert(_eq(c1, m[0]))
    assert(_eq(c4, m[5]))
    sanitizeIndex = false
  end

  do
    local m = mat(c1, c2, c3, c4)
    T.testC("settable -3", m, 1, c4) assert(_eq(c4, m[1]) and _eq(c2, m[2]) and _eq(c3, m[3]) and _eq(c4, m[4]))
    T.testC("settable -3", m, 2, c4) assert(_eq(c4, m[1]) and _eq(c4, m[2]) and _eq(c3, m[3]) and _eq(c4, m[4]))
    T.testC("settable -3", m, 3, c4) assert(_eq(c4, m[1]) and _eq(c4, m[2]) and _eq(c4, m[3]) and _eq(c4, m[4]))
  end

  do
    local m = mat(c1, c2, c3, c4)
    sanitizeIndex = true
    T.testC("settable -3", m, 0, c4) assert(_eq(c4, m[1]) and _eq(c2, m[2]) and _eq(c3, m[3]) and _eq(c4, m[4]))
    T.testC("settable -3", m, 5, c1) assert(_eq(c4, m[1]) and _eq(c2, m[2]) and _eq(c3, m[3]) and _eq(c1, m[4]))
    sanitizeIndex = false
  end

  debug.setmetatable(m, mt) -- Reset metatable to default
end

---------------------------------------
---------- getfield/setfield ----------
---------------------------------------
print("getfield/setfield")

do
  local x, y, z = T.testC("getfield 2 x; getfield 2 y; getfield 2 z; return 3", v3)
  assert(_eq(v3.x, x) and _eq(v3.y, y) and _eq(v3.z, z))
end

do
  local q = quat(0.953717, 0.080367, 0.160734, 0.241101)
  local x, y, z, w = T.testC("getfield 2 x; getfield 2 y; getfield 2 z; getfield 2 w; return 4", q)
  assert(_eq(q.x, x) and _eq(q.y, y) and _eq(q.z, z) and _eq(q.w, w))
end

do -- Invalid gettable access.
  local v2 = vec2(1, 2)
  local x, y = T.testC("getfield 2 x; getfield 2 y; return 2", v2)
  local z, w = T.testC("getfield 2 z; getfield 2 w; return 2", v2)
  assert(_eq(v2.x, x) and _eq(v2.y, y) and z == nil and w == nil)
end

do -- gettable access if the vector metatable is GLM
  local v2 = vec2(1, 2)
  if glm ~= nil and debug.getmetatable(v2) == glm then
    local abs = T.testC("getfield 2 abs; return 1", v2)
    assert(abs == glm.abs)
  end
end

do
  local m = debug.setmetatable(mat(c1, c2, c3, c4), {
    __index = function(self, k)
      if type(k) == "string" then
        return self[tonumber(k)]
      end
      return nil
    end,

    __newindex = function(self, k, v)
      rawset(self, tonumber(k), v)
    end,
  })

  assert(_eq(T.testC("getfield 2 \"1\" ; return 1", m), c1))
  assert(_eq(T.testC("getfield 2 \"2\" ; return 1", m), c2))
  assert(_eq(T.testC("getfield 2 \"3\" ; return 1", m), c3))
  assert(_eq(T.testC("getfield 2 \"4\" ; return 1", m), c4))
  assert(T.testC("getfield 2 \"0\" ; return 1", m) == nil)
  assert(T.testC("getfield 2 \"-1\" ; return 1", m) == nil)
  assert(T.testC("getfield 2 \"-5\" ; return 1", m) == nil)

  T.testC("setfield 2 \"1\"", m, c4) assert(_eq(c4, m[1]) and _eq(c2, m[2]) and _eq(c3, m[3]) and _eq(c4, m[4]))
  T.testC("setfield 2 \"2\"", m, c4) assert(_eq(c4, m[1]) and _eq(c4, m[2]) and _eq(c3, m[3]) and _eq(c4, m[4]))
  T.testC("setfield 2 \"3\"", m, c4) assert(_eq(c4, m[1]) and _eq(c4, m[2]) and _eq(c4, m[3]) and _eq(c4, m[4]))
  T.testC("setfield 2 \"4\"", m, c1) assert(_eq(c4, m[1]) and _eq(c4, m[2]) and _eq(c4, m[3]) and _eq(c1, m[4]))
  assert(_eq(c4, m["1"]) and _eq(c4, m["2"]) and _eq(c4, m["3"]) and _eq(c1, m["4"]))

  debug.setmetatable(m, mt)
end

---------------------------------------
----------- rawgeti/rawseti -----------
---------------------------------------

print("rawgeti/rawseti")
do
  local x, y, z = T.testC("rawgeti 2 1; rawgeti 2 2; rawgeti 2 3; return 3", v3)
  assert(_eq(v3.x, x) and _eq(v3.y, y) and _eq(v3.z, z))
end

do
  local x, y, z, w = T.testC("rawgeti 2 1; rawgeti 2 2; rawgeti 2 3; rawgeti 2 4; return 4", q)
  assert(_eq(q.x, x) and _eq(q.y, y) and _eq(q.z, z) and _eq(q.w, w))
end

do
  local m = mat(c1, c2, c3, c4)
  local x1,x2,x3,x4 = T.testC("rawgeti 2 1; rawgeti 2 2; rawgeti 2 3; rawgeti 2 4; return 4", m)
  assert(_eq(c1, x1) and _eq(c2, x2) and _eq(c3, x3) and _eq(c4, x4))
end

do
  local m = mat(c1, c2)
  assert(T.testC("rawgeti 2 3; return 1", m) == nil)
  assert(T.testC("rawgeti 2 0; return 1", m) == nil)
  assert(T.testC("rawgeti 2 -1; return 1", m) == nil)
end
