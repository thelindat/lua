/*
** $Id: iterators.hpp $
** A set of iterator definitions for processing collections of traits without
** the requirement of allocating additional memory (and to avoid interop issues)
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
class glmLuaIterator : public gLuaBase {
public:
  glmLuaIterator(lua_State *L_, int idx_ = 1)
    : gLuaBase(L_, idx_) {
  }

  // virtual ~glmLuaIterator() = default; // VERBOTEN!
  glmLuaIterator(const glmLuaIterator &) = default;
  glmLuaIterator(glmLuaIterator &&) = default;
  glmLuaIterator &operator=(const glmLuaIterator &) = default;
  glmLuaIterator &operator=(glmLuaIterator &&) = default;

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
class glmLuaContainer : public gLuaBase {
public:
  glmLuaContainer(lua_State *L_, int idx_ = 1)
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
class glmLuaStack : public glmLuaContainer<Tr> {
public:
  using size_type = typename glmLuaContainer<Tr>::size_type;

  class Iterator : public glmLuaIterator<Tr> {
    friend class glmLuaStack;

  private:
    int m_top;  // Cache of lua_gettop on instantiation

    /// <summary>
    /// Within stack bounds.
    /// </summary>
    bool valid() const {
      return gLuaBase::idx >= 1 && gLuaBase::idx <= m_top;
    }

  public:
    Iterator(lua_State *L_, int idx_, int top_)
      : glmLuaIterator<Tr>(L_, idx_), m_top(top_) {
    }

    /// <summary>
    /// Create a glm value starting at the current Lua stack index
    /// </summary>
    typename Tr::type operator*() const {
      lua_State *L_ = gLuaBase::L;

      typename Tr::type value = Tr::Zero();
      if (!gLuaBase::Pull(L_, gLuaBase::idx, value)) {
        luaL_error(L_, "iterator: invalid %s structure", Tr::Label());
      }
      return value;
    }

    /// <summary>
    /// Iterate to the next glm value on the Lua stack. At one point this could
    /// have possibly corresponded to multiple stack values. However, the
    /// current implementation is one-to-one (i.e., each GLM structure is
    /// equivalent to one Lua stack value).
    /// </summary>
    const Iterator &operator++() {
      gLuaBase::idx++;
      return *this;
    }

    const Iterator &operator++(int) {  // @HACK
      gLuaBase::idx++;
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
  glmLuaStack(lua_State *L_, int idx_ = 1)
    : glmLuaContainer<Tr>(L_, idx_), m_top(gLuaBase::top()) {
  }

  inline size_type size() const {
    return static_cast<size_type>(m_top);
  }

  typename Tr::type operator[](size_type pos) const {
    typename Tr::type value = Tr::Zero();
    if (pos >= 0 && pos < size()) {
      if (!gLuaBase::Pull(gLuaBase::L, pos + 1, value)) {
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
class glmLuaArray : public glmLuaContainer<Tr> {
public:
  using size_type = typename glmLuaContainer<Tr>::size_type;

  class Iterator : public glmLuaIterator<Tr> {
    friend class glmLuaArray;

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
      : glmLuaIterator<Tr>(L_, idx_), m_arrayIdx(arrayIdx), m_arraySize(arraySize) {
    }

    Iterator(lua_State *L_, int idx_, size_type arrayIdx)
      : glmLuaIterator<Tr>(L_, idx_), m_arrayIdx(arrayIdx) {
      lua_assert(lua_istable(L_, idx_));
      m_arraySize = static_cast<size_type>(lua_rawlen(L_, idx_));
    }

    /// <summary>
    /// Goto the next element in the array.
    /// </summary>
    const Iterator &operator++() {
      m_arrayIdx++;
      return *this;
    }

    const Iterator &operator++(int) {  // @HACK
      m_arrayIdx++;
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

      // Fetch the object within the array that *should* correspond to the trait.
      lua_State *L_ = gLuaBase::L;
      lua_rawgeti(L_, gLuaBase::idx, static_cast<lua_Integer>(m_arrayIdx));  // [..., element]
      if (!gLuaBase::Pull(L_, lua_absindex(L_, -1), value)) {
        lua_pop(L_, 1);
        luaL_error(L_, "iterator: invalid table index: %d for %s", static_cast<int>(m_arrayIdx), Tr::Label());
        return value;  // quash compiler warnings, luaL_error is noret.
      }

      lua_pop(L_, 1);  // [...]
      return value;
    }
  };

private:
  /// <summary>
  /// Cached array length.
  /// </summary>
  size_type m_arraySize = 0;

public:
  glmLuaArray(lua_State *L_, int idx_ = 1)
    : glmLuaContainer<Tr>(L_, idx_) {
    lua_assert(lua_istable(L_, idx_));
    m_arraySize = static_cast<size_type>(lua_rawlen(gLuaBase::L, gLuaBase::idx));
  }

  bool valid() const {
    return lua_istable(gLuaBase::L, gLuaBase::idx);
  }

  /// <summary>
  /// Update the array state and ensure its size is still (cache) coherent.
  /// </summary>
  void validate() {
    lua_assert(lua_istable(gLuaBase::L, gLuaBase::idx));
    m_arraySize = static_cast<size_type>(lua_rawlen(gLuaBase::L, gLuaBase::idx));
  }

  inline size_type size() const {
    return m_arraySize;
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
      lua_rawgeti(L_, gLuaBase::idx, static_cast<lua_Integer>(pos + 1));  // [..., element]
      if (!gLuaBase::Pull(L_, lua_absindex(L_, -1), value)) {
        lua_pop(L_, 1);
        luaL_error(L_, "operator[]: invalid %s structure", Tr::Label());
      }
      lua_pop(L_, 1);  // [...]
    }
    return value;
  }

  Iterator begin() const {
    return Iterator(gLuaBase::L, gLuaBase::idx, 1);
  }

  Iterator end() const {
    const size_type arraySize = size();
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
    const size_type arraySize = size();
    arrayEndIndex = (arrayEndIndex == 0) ? arraySize + 1 : arrayEndIndex;
    return Iterator(gLuaBase::L, gLuaBase::idx, arrayEndIndex, arraySize);
  }
};

#endif
