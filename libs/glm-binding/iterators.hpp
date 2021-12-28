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
#include <iterator>
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
    gLuaBase::top();  // Cache lua_gettop
  }

  // virtual ~glmLuaIterator() = default; // VERBOTEN!
  glmLuaIterator(const glmLuaIterator &) = default;
  glmLuaIterator(glmLuaIterator &&) = default;
  glmLuaIterator &operator=(const glmLuaIterator &) = default;
  glmLuaIterator &operator=(glmLuaIterator &&) = default;

  // Iterator Traits
  using iterator_category = std::forward_iterator_tag;
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
    gLuaBase::top();  // Cache lua_gettop
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

  glmLuaStack(lua_State *L_, int idx_ = 1)
    : glmLuaContainer<Tr>(L_, idx_) {
  }

  class Iterator : public glmLuaIterator<Tr> {
    friend class glmLuaStack;

  private:
    /// <summary>
    /// Within stack bounds.
    /// </summary>
    bool valid() const {
      return gLuaBase::idx >= 1 && gLuaBase::idx <= lua_gettop(gLuaBase::L);
    }

  public:
    Iterator(lua_State *L_, int idx_ = 1)
      : glmLuaIterator<Tr>(L_, idx_) {
    }

    /// <summary>
    /// Create a glm value starting at the current Lua stack index
    /// </summary>
    typename Tr::type operator*() const {
      lua_State *L_ = gLuaBase::L;

      typename Tr::type value = Tr::zero();
      if (!gLuaBase::Pull(L_, gLuaBase::idx, value)) {
        luaL_error(L_, "Invalid %s structure", Tr::Label());
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

  size_type size() const {
    return static_cast<size_type>(gLuaBase::top());
  }

  typename Tr::type operator[](size_type pos) const {
    typename Tr::type value = Tr::zero();
    if (pos >= 0 && pos < size()) {
      lua_State *L_ = gLuaBase::L;
      if (!gLuaBase::Pull(L_, pos + 1, value)) {
        luaL_error(L_, "Invalid %s structure", Tr::Label());
      }
    }
    return value;
  }

  Iterator begin() const {
    return Iterator(gLuaBase::L, gLuaBase::idx);
  }

  Iterator end() const {
    return Iterator(gLuaBase::L, gLuaBase::top() + 1);
  }

  /// <summary>
  /// Sugar forEach helper.
  /// </summary>
  void forEach(std::function<void(const typename Tr::type &)> func) {
    auto e = end();
    for (auto b = begin(); b != e; ++b) {
      func(*b);
    }
  }
};

/// <summary>
/// Traits defined over elements of a Lua table.
/// </summary>
template<typename Tr>
class glmLuaArray : public glmLuaContainer<Tr> {
public:
  using size_type = typename glmLuaContainer<Tr>::size_type;

private:
  /// <summary>
  /// Cached array length.
  ///
  /// @TODO: Method to invalidate if table is mutated.
  /// </summary>
  size_type arraySize = 0;

  bool valid() const {
    return lua_istable(gLuaBase::L, gLuaBase::idx);
  }

public:
  glmLuaArray(lua_State *L_, int idx_ = 1)
    : glmLuaContainer<Tr>(L_, idx_) {
    if (!lua_istable(L_, idx_)) {
      // @TODO: Throw assertion failure.
    }

    arraySize = static_cast<size_type>(lua_rawlen(gLuaBase::L, gLuaBase::idx));
  }

  class Iterator : public glmLuaIterator<Tr> {
    friend class glmLuaArray;

  private:
    size_type arrayIdx;  // Current array index.
    size_type arraySize;  // (Precomputed) array size.

    /// <summary>
    /// Within array bounds & is a valid trait.
    /// </summary>
    GLM_INLINE bool valid() const {
      return arrayIdx >= 1 && arrayIdx <= arraySize;
    }

  public:
    Iterator(lua_State *L_, int idx_, size_type arrayIdx_, size_type arraySize_)
      : glmLuaIterator<Tr>(L_, idx_), arrayIdx(arrayIdx_), arraySize(arraySize_) {
    }

    Iterator(lua_State *L_, int idx_, size_type arraySize_ = 1)
      : glmLuaIterator<Tr>(L_, idx_), arrayIdx(arraySize_) {
      arraySize = lua_istable(L_, idx_) ? static_cast<size_type>(lua_rawlen(L_, idx_)) : 0;
    }

    /// <summary>
    /// Goto the next element in the array.
    /// </summary>
    const Iterator &operator++() {
      arrayIdx++;
      return *this;
    }

    const Iterator &operator++(int) {  // @HACK
      arrayIdx++;
      return *this;
    }

    bool operator==(const Iterator &rhs) const {
      return (arrayIdx == rhs.arrayIdx) || (!valid() && !rhs.valid());
    }

    bool operator!=(const Iterator &rhs) const {
      return !operator==(rhs);
    }

    typename Tr::type operator*() const {
      typename Tr::type value = Tr::zero();

      // Fetch the object within the array that *should* correspond to the trait.
      lua_State *L_ = gLuaBase::L;
      lua_rawgeti(L_, gLuaBase::idx, static_cast<lua_Integer>(arrayIdx));
      const int top_ = lua_gettop(L_);  // gLuaBase uses absolute values.

      // Parse the trait given the relative stack (starting) index.
      if (!gLuaBase::Pull(L_, top_, value)) { /* noret */
        lua_pop(L_, 1);
        luaL_error(L_, "Invalid table index: %d for %s", static_cast<int>(arrayIdx), Tr::Label());

        return value;  // quash compiler warnings, luaL_error is noret.
      }

      lua_pop(L_, 1);
      return value;
    }
  };

  size_type size() const {
    return arraySize;
  }

  /// <summary>
  /// With how this binding is implemented, Lua stack adjustments, i.e.,
  /// luaD_growstack, should be prevented. Avoid handing control of the runtime
  /// to lua_geti (and potential __index metamethod).
  ///
  /// @TODO: Optimize
  /// </summary>
  typename Tr::type operator[](size_type pos) const {
    typename Tr::type value = Tr::zero();
    if (pos >= 0 && pos < size()) {
      lua_State *L_ = gLuaBase::L;
      lua_rawgeti(L_, gLuaBase::idx, static_cast<lua_Integer>(pos + 1));  // [..., element]
      if (!gLuaBase::Pull(L_, lua_absindex(L_, -1), value)) {
        lua_pop(L_, 1);
        luaL_error(L_, "Invalid %s structure", Tr::Label());
      }
      lua_pop(L_, 1);  // [...]
    }
    return value;
  }

  Iterator begin() const {
    return Iterator(gLuaBase::L, gLuaBase::idx, 1);
  }

  Iterator end() const {
    const size_type _tablesize = size();
    return Iterator(gLuaBase::L, gLuaBase::idx, _tablesize + 1, _tablesize);
  }

  /// <summary>
  /// Create an iterator at the specified array index "a_idx".
  /// </summary>

  Iterator begin(size_type a_idx = 1) {
    return Iterator(gLuaBase::L, gLuaBase::idx, a_idx);
  }

  Iterator end(size_type a_end_idx = 0) {
    const size_type _tablesize = static_cast<size_type>(lua_rawlen(gLuaBase::L, gLuaBase::idx));

    a_end_idx = (a_end_idx == 0) ? _tablesize + 1 : a_end_idx;
    return Iterator(gLuaBase::L, gLuaBase::idx, a_end_idx, _tablesize);
  }

  /// Sugar
  void forEach(std::function<void(const typename Tr::type &)> func) {
    auto e = end();
    for (auto b = begin(); b != e; ++b) {
      func(*b);
    }
  }
};

#endif
