/*
** $Id: iterators.hpp $
** A set of iterator definitions for processing collections of type traits
** without temporary help allocations (and avoid interop issues).
**
** @TODO: Consider emulating O3DE OnDemandReflection to allow easier integration
** with C/C++ structures. This would be useful in performance conscious scripts.
** The definitions below use Lua tables as the base structure and parsing them
** can be quite slow (esp. with larger datasets).
**
** See Copyright Notice in lua.h
*/
#ifndef BINDING_ITERATORS_HPP
#define BINDING_ITERATORS_HPP

#include <functional>
#include <lua.hpp>

#include "bindings.hpp"

/// <summary>
/// Lua stack trait iterator interface
/// </summary>
template<typename Trait>
class gLuaIterator : public gLuaBase {
public:
  gLuaIterator(lua_State *L_, int idx_ = 1)
    : gLuaBase(L_, idx_) {
  }

  // virtual ~gLuaIterator() = default; // VERBOTEN!
  gLuaIterator(const gLuaIterator &) = default;
  gLuaIterator(gLuaIterator &&) = default;
  gLuaIterator &operator=(const gLuaIterator &) = default;
  gLuaIterator &operator=(gLuaIterator &&) = default;

  // Iterator Traits
  using value_type = typename Trait::type;
  using difference_type = ptrdiff_t;
  using pointer = typename Trait::type *;
  using reference = typename Trait::type &;
};

/// <summary>
/// Base container interface
/// </summary>
template<typename Trait>
class gLuaContainer : public gLuaBase {
public:
  gLuaContainer(lua_State *L_, int idx_ = 1)
    : gLuaBase(L_, idx_) {
  }

  // Interface Outline.

  /// <summary>
  /// Container dependent size
  /// </summary>
  using size_type = int;

  /// <summary>
  /// Returns the number of elements in the container.
  /// </summary>
  // size_type size() const;

  /// <summary>
  /// Returns a the element at specified location 'pos' (base-zero).
  /// </summary>
  // typename Trait::type operator[](size_type pos) const;

  /// <summary>
  /// Returns an iterator to the first element of the container.
  /// </summary>
  /// <returns></returns>
  // Iterator begin() const;

  /// <summary>
  /// Returns an iterator to the element following the last element of the container.
  /// </summary>
  // Iterator end() const;
};

/// <summary>
/// Trait defined over elements of a Lua stack.
/// </summary>
template<typename Tr>
class gLuaStack : public gLuaContainer<Tr> {
public:
  using size_type = typename gLuaContainer<Tr>::size_type;

  class Iterator : public gLuaIterator<Tr> {
    friend class gLuaStack;

  private:
    int m_top;  // Cache of lua_gettop on instantiation

    /// <summary>
    /// Ensure idx is within stack bounds.
    /// </summary>
    bool valid() const {
      return gLuaBase::idx >= 1 && gLuaBase::idx <= m_top;
    }

  public:
    Iterator(lua_State *L_, int idx_, int top_)
      : gLuaIterator<Tr>(L_, idx_), m_top(top_) {
    }

    /// <summary>
    /// Create a glm value starting at the current Lua stack index
    /// </summary>
    typename Tr::type operator*() const {
      typename Tr::type value = Tr::Zero();
      if (!gLuaBase::Pull(gLuaBase::L, gLuaBase::idx, value))
        luaL_error(gLuaBase::L, "iterator: invalid %s structure", Tr::Label());
      return value;
    }

    const Iterator &operator++() {
      gLuaBase::idx += Tr::stack_size;
      return *this;
    }

    const Iterator &operator++(int) {  // @HACK
      gLuaBase::idx += Tr::stack_size;
      return *this;
    }

    bool operator==(const Iterator &rhs) const {
      return (gLuaBase::idx == rhs.idx) || (!valid() && !rhs.valid());
    }

    bool operator!=(const Iterator &rhs) const {
      return !operator==(rhs);
    }
  };

private:
  int m_top;  // Cache of lua_gettop on instantiation

public:
  gLuaStack(lua_State *L_, int idx_ = 1)
    : gLuaContainer<Tr>(L_, idx_), m_top(gLuaBase::top()) {
  }

  inline size_type size() const {
    if (m_top < gLuaBase::idx)
      return 0;
    return static_cast<size_type>(1 + (m_top - gLuaBase::idx)) / Tr::stack_size;
  }

  typename Tr::type operator[](size_type pos) const {
    typename Tr::type value = Tr::Zero();
    if (pos >= 0 && pos < size()) {
      const int sidx = gLuaBase::idx + static_cast<int>(pos * Tr::stack_size);
      if (sidx < 0 || !gLuaBase::Pull(gLuaBase::L, sidx, value)) {
        luaL_error(gLuaBase::L, "operator[]: invalid %s structure", Tr::Label());
      }
    }
    return value;
  }

  Iterator begin() const {
    return Iterator(gLuaBase::L, gLuaBase::idx, m_top);
  }

  Iterator end() const {
    return Iterator(gLuaBase::L, m_top + 1, m_top);
  }
};

/// <summary>
/// Traits defined over elements of a Lua table.
/// </summary>
template<typename Tr>
class gLuaArray : public gLuaContainer<Tr> {
public:
  using size_type = typename gLuaContainer<Tr>::size_type;

  class Iterator : public gLuaIterator<Tr> {
    friend class gLuaArray;

  private:
    size_type m_arrayIdx;  // Current array index.
    size_type m_arraySize;  // (Precomputed) array size.

    /// <summary>
    /// Within array bounds & is a valid trait.
    /// </summary>
    GLM_INLINE bool valid() const {
      return m_arrayIdx >= 1 && m_arrayIdx <= m_arraySize;
    }

  public:
    Iterator(lua_State *L_, int idx_, size_type arrayIdx, size_type arraySize)
      : gLuaIterator<Tr>(L_, idx_), m_arrayIdx(arrayIdx), m_arraySize(arraySize) {
    }

    Iterator(lua_State *L_, int idx_, size_type arrayIdx)
      : gLuaIterator<Tr>(L_, idx_), m_arrayIdx(arrayIdx), m_arraySize(0) {
      if (lua_istable(L_, idx_)) {
        m_arraySize = static_cast<size_type>(lua_rawlen(L_, idx_));
      }
    }

    /// <summary>
    /// Goto the next element in the array.
    /// </summary>
    const Iterator &operator++() {
      m_arrayIdx += Tr::stack_size;
      return *this;
    }

    const Iterator &operator++(int) {  // @HACK
      m_arrayIdx += Tr::stack_size;
      return *this;
    }

    bool operator==(const Iterator &rhs) const {
      return (m_arrayIdx == rhs.m_arrayIdx) || (!valid() && !rhs.valid());
    }

    bool operator!=(const Iterator &rhs) const {
      return !operator==(rhs);
    }

    typename Tr::type operator*() const {
      typename Tr::type value = Tr::Zero();
      lua_State *L_ = gLuaBase::L;

      // Fetch the objects within the array that *should* correspond to the trait.
      // luaL_checkstack(L_, Tr::stack_size, "array iterator");
      for (int i = 0; i < Tr::stack_size; ++i) {  // [..., base_element, ..., element]
        lua_rawgeti(L_, gLuaBase::idx, static_cast<lua_Integer>(m_arrayIdx + i));
      }

      if (!gLuaBase::Pull(L_, lua_absindex(L_, -Tr::stack_size), value)) {
        lua_pop(L_, Tr::stack_size);
        luaL_error(L_, "iterator: invalid table index: %d for %s", static_cast<int>(m_arrayIdx), Tr::Label());
        return value;  // quash compiler warnings, luaL_error is noret.
      }

      lua_pop(L_, Tr::stack_size);  // [...]
      return value;
    }
  };

private:
  /// <summary>
  /// Cached array length.
  /// </summary>
  size_type m_arraySize;

public:
  gLuaArray(lua_State *L_, int idx_ = 1)
    : gLuaContainer<Tr>(L_, idx_), m_arraySize(0) {
    if (lua_istable(L_, idx_)) {
      m_arraySize = static_cast<size_type>(lua_rawlen(gLuaBase::L, gLuaBase::idx));
    }
  }

  /// <summary>
  /// Ensure the object being referenced is a table.
  /// </summary>
  bool valid() const {
    return lua_istable(gLuaBase::L, gLuaBase::idx);
  }

  /// <summary>
  /// Update the array state and ensure its size is still (cache) coherent.
  /// </summary>
  void validate() {
    m_arraySize = valid() ? static_cast<size_type>(lua_rawlen(gLuaBase::L, gLuaBase::idx)) : 0;
  }

  /// <summary>
  /// Returns the number of trait types in the array.
  /// </summary>
  inline size_type size() const {
    return m_arraySize / Tr::stack_size;
  }

  /// <summary>
  /// With how this binding is implemented, Lua stack adjustments, i.e.,
  /// luaD_growstack, should be prevented. Avoid handing control of the runtime
  /// to lua_geti and potential __index metamethod.
  /// </summary>
  typename Tr::type operator[](size_type pos) const {
    typename Tr::type value = Tr::Zero();
    if (pos >= 0 && pos < size()) {
      lua_State *L_ = gLuaBase::L;

      // Fetch the objects within the array.
      // luaL_checkstack(L_, Tr::stack_size, "trait array");
      const lua_Integer sidx = 1 + static_cast<lua_Integer>(pos * Tr::stack_size);
      for (int i = 0; i < Tr::stack_size; ++i) {  // [..., base_element, ..., element]
        lua_rawgeti(L_, gLuaBase::idx, sidx + i);
      }

      if (!gLuaBase::Pull(L_, lua_absindex(L_, -Tr::stack_size), value)) {
        lua_pop(L_, Tr::stack_size);
        luaL_error(L_, "operator[]: invalid %s structure", Tr::Label());
      }
      lua_pop(L_, Tr::stack_size);  // [...]
    }
    return value;
  }

  Iterator begin() const {
    return Iterator(gLuaBase::L, gLuaBase::idx, 1);
  }

  Iterator end() const {
    const size_type arraySize = m_arraySize;
    return Iterator(gLuaBase::L, gLuaBase::idx, arraySize + 1, arraySize);
  }

  /// <summary>
  /// Create an iterator at the specified array index.
  /// </summary>
  Iterator begin(size_type arrayIndex = 1) {
    return Iterator(gLuaBase::L, gLuaBase::idx, arrayIndex);
  }

  /// <summary>
  /// Create an iterator ending at the specified array index.
  /// </summary>
  Iterator end(size_type arrayEndIndex = 0) {
    const size_type arraySize = m_arraySize;
    arrayEndIndex = (arrayEndIndex == 0) ? arraySize + 1 : arrayEndIndex;
    return Iterator(gLuaBase::L, gLuaBase::idx, arrayEndIndex, arraySize);
  }
};

#endif
