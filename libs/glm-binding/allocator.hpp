/*
** $Id: allocator.hpp $
** An allocator implementation for C++ STL objects and other STL analogues.
**
** See Copyright Notice in lua.h
*/
#ifndef BINDING_ALLOCATOR_HPP
#define BINDING_ALLOCATOR_HPP

#include <cstring>
#include <new>
#include <utility>
#if defined(LUA_ALLOC_DEBUG)
  #include <iostream>
#endif

#include <lua.hpp>

#if __cplusplus >= 201103L
  #define LUA_ALLOC_NOEXCEPT noexcept
  #define LUA_ALLOC_CONSTEXPR constexpr
  #define LUA_ALLOC_NULLPTR nullptr
#else
  #define LUA_ALLOC_NOEXCEPT
  #define LUA_ALLOC_CONSTEXPR
  #define LUA_ALLOC_NULLPTR NULL
#endif

#if __cplusplus >= 201703L
  #define LUA_ALLOC_IF_CONSTEXPR if constexpr
#else
  #define LUA_ALLOC_IF_CONSTEXPR if
#endif

/* Basic exception-handling check */
#if defined(__EXCEPTIONS) || defined(__cpp_exceptions) || defined(_CPPUNWIND)
  #define LUA_ALLOC_EXCEPTIONS 1
#else
  #define LUA_ALLOC_EXCEPTIONS 0
#endif

namespace lua {
#if __cplusplus >= 202002L
using std::construct_at;
#else
template<class T, class... Args>
T *construct_at(T *p, Args &&...args) {
  return ::new (const_cast<void *>(static_cast<const volatile void *>(p))) T(std::forward<Args>(args)...);
}
#endif

#if __cplusplus >= 201703L
using std::destroy_at;
#else
template<class T>
void destroy_at(T *p) {
  p->~T();
}
#endif

/// <summary>
/// A std::allocator that uses lua_Alloc.
/// </summary>
template<class T>
struct STLAllocator {
  typedef T value_type;
  mutable lua_Alloc m_alloc = LUA_ALLOC_NULLPTR;  // A cache of the memory-allocation function used by Lua states.
  mutable void *m_ud = LUA_ALLOC_NULLPTR;  // An (optional) opaque pointer used by the allocator.

  STLAllocator() = default;
  STLAllocator(lua_State *L) {
    m_alloc = lua_getallocf(L, &m_ud);
  }

  template<class U>
  LUA_ALLOC_CONSTEXPR STLAllocator(const STLAllocator<U> &o) LUA_ALLOC_NOEXCEPT
    : m_alloc(o.l_alloc),
      m_ud(o.l_ud) {
  }

  /// <summary>
  /// Update the allocator state and ensure its allocation function and opaque
  /// pointer are still (cache) coherent.
  ///
  /// Caching the lua_Alloc pointer is susceptible to errors: If any external
  /// library, e.g., a memory profiler, replaces the allocator then l_alloc and
  /// l_ud to have the potential to reference invalid data.
  /// </summary>
  LUA_INLINE STLAllocator &validate(lua_State *L) const {
    m_alloc = lua_getallocf(L, &m_ud);
    return *const_cast<STLAllocator *>(this);
  }

  /// <summary>
  /// lua_Alloc: See Lua manual
  /// </summary>
  inline void *realloc(void *block, size_t osize, size_t nsize) {
    return (m_alloc == LUA_ALLOC_NULLPTR) ? LUA_ALLOC_NULLPTR : m_alloc(m_ud, block, osize, nsize);
  }

  T *allocate(std::size_t n) {
#if LUA_ALLOC_EXCEPTIONS
    try {
#endif
      if (n <= std::numeric_limits<std::size_t>::max() / sizeof(T)) {
        if (auto p = static_cast<T *>(realloc(LUA_ALLOC_NULLPTR, 0, n * sizeof(T)))) {
#if defined(LUA_ALLOC_DEBUG)
          report(p, n);
#endif
          return p;
        }
      }
#if LUA_ALLOC_EXCEPTIONS
    }
    catch (...) {
    }

    throw std::bad_alloc();
#else
    return LUA_ALLOC_NULLPTR;
#endif
  }

  void deallocate(T *p, std::size_t n) LUA_ALLOC_NOEXCEPT {
    if (m_alloc != LUA_ALLOC_NULLPTR) {  // otherwise, allocate would have failed
#if defined(LUA_ALLOC_DEBUG)
      report(p, n, 0);
#endif
      realloc(static_cast<void *>(p), n, 0);
    }
  }

private:
#if defined(LUA_ALLOC_DEBUG)
  void report(T *p, std::size_t n, bool alloc = true) const {
    std::cout << (alloc ? "Alloc: " : "Dealloc: ") << sizeof(T) * n
              << " bytes at " << std::hex << std::showbase
              << reinterpret_cast<void *>(p) << std::dec << '\n';
  }
#endif
};

/// <summary>
/// A std::vector analogue that uses 'lua_Alloc' for all internal allocations.
/// Documentation is mirrored from https://en.cppreference.com/w/cpp/container/vector
/// </summary>
template<class T>
class Vector {
public:
  // Index Types
  using size_type = std::size_t;

  // Value Types
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using difference_type = ptrdiff_t;

  // Iterator Types
  using iterator = T *;
  using const_iterator = const T *;

private:
  static LUA_ALLOC_CONSTEXPR size_type grow_factor = 2;
  static const LUA_ALLOC_CONSTEXPR bool is_trivial_value = std::is_trivial<T>::value;

  lua_State *m_state;
  STLAllocator<T> m_alloc;
  T *m_data = LUA_ALLOC_NULLPTR;
  size_type m_size = 0;
  size_type m_capacity = 0;

  /* Allocator Utilities */

  inline size_t internal_capacity() {
    return m_capacity * sizeof(T);
  }

  /// <summary>
  /// Realloc wrapper.
  ///
  /// Helper methods exist to allow testing of this vector implementation
  /// outside of Lua environments with as few changes possible.
  /// </summary>
  inline void *realloc_(void *block, size_t osize, size_t nsize) {
    void *p = m_alloc.realloc(block, osize, nsize);
    if (p == LUA_ALLOC_NULLPTR) {
      //lua_checkstack(m_state, 1);
      lua_pushstring(m_state, "LuaVector allocation failure");
      lua_error(m_state);
    }
    return p;
  }

  /// <summary>
  /// lua_Alloc 'malloc' helper.
  /// </summary>
  inline void *malloc_(size_t size_) {
    void *p = m_alloc.realloc(LUA_ALLOC_NULLPTR, 0, size_);
    if (p == LUA_ALLOC_NULLPTR) {
      //lua_checkstack(m_state, 1);
      lua_pushstring(m_state, "LuaVector allocation failure");
      lua_error(m_state);
    }
    return p;
  }

  /// <summary>
  /// lua_Alloc 'free' helper.
  /// </summary>
  inline void free_(void *block, size_t osize) {
    m_alloc.realloc(block, osize, 0);
  }

  /* Type Utilities */

  inline void constructInPlace(iterator begin_, iterator end_) {
    while (begin_ != end_) {
      new (begin_) T;
      begin_++;
    }
  }

  inline void destroyInPlace(iterator begin_, iterator end_) {
    while (begin_ != end_) {
      destroy_at(begin_);
      begin_++;
    }
  }

  inline void copyInPlace(iterator begin_, iterator end_, iterator dest_) {
    while (begin_ != end_) {
      new (dest_) T(*begin_);
      begin_++;
      dest_++;
    }
  }

public:
  /* Constructors */
  Vector(lua_State *L, STLAllocator<T> &alloc)
    : m_state(L),
      m_alloc(alloc),
      m_data(LUA_ALLOC_NULLPTR),
      m_size(0),
      m_capacity(0) {
  }

  Vector(const Vector<T> &other)
    : m_state(other.m_state),
      m_alloc(other.m_alloc),
      m_data(LUA_ALLOC_NULLPTR),
      m_size(other.m_size),
      m_capacity(other.m_capacity) {

    m_data = static_cast<T *>(malloc_(other.m_capacity * sizeof(T)));
    LUA_ALLOC_IF_CONSTEXPR(is_trivial_value) {
      std::memcpy(static_cast<void *>(m_data), other.m_data, other.m_size);
    }
    else {
      copyInPlace(other.begin(), other.end(), m_data);
    }
  }

  Vector(Vector<T> &&other) LUA_ALLOC_NOEXCEPT
    : m_state(other.m_state),
      m_alloc(other.m_alloc),
      m_data(other.m_data),
      m_size(other.m_size),
      m_capacity(other.m_capacity) {

    other.m_data = LUA_ALLOC_NULLPTR;
    other.m_state = LUA_ALLOC_NULLPTR;
  }

  Vector<T> &operator=(const Vector<T> &other) {
    m_state = other.m_state;
    m_alloc = other.m_alloc;
    m_size = other.m_size;
    m_capacity = other.m_capacity;

    m_data = static_cast<T *>(malloc_(other.m_capacity * sizeof(T)));
    LUA_ALLOC_IF_CONSTEXPR(is_trivial_value) {
      std::memcpy(static_cast<void *>(m_data), other.m_data, other.m_size);
    }
    else {
      copyInPlace(other.begin(), other.end(), m_data);
    }

    return *this;
  }

  Vector<T> &operator=(Vector<T> &&other) LUA_ALLOC_NOEXCEPT {
    m_state = other.m_state;
    m_alloc = other.m_alloc;
    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;

    other.m_data = LUA_ALLOC_NULLPTR;
    other.m_state = LUA_ALLOC_NULLPTR;
    return *this;
  }

  ~Vector() LUA_ALLOC_NOEXCEPT {
    LUA_ALLOC_IF_CONSTEXPR(!is_trivial_value) {
      destroyInPlace(begin(), end());
    }

    free_(static_cast<void *>(m_data), internal_capacity());
    m_data = LUA_ALLOC_NULLPTR;
    m_state = LUA_ALLOC_NULLPTR;
    m_capacity = 0;
    m_size = 0;
  }

  /// <summary>
  /// Ensure the vector is consistent with the provided Lua state:
  ///   1. Update the internal allocator to ensure its allocation function and
  ///   opaque pointer are still (cache) coherent.
  /// </summary>
  void validate(lua_State *L) LUA_ALLOC_NOEXCEPT {
    m_state = L;
    m_alloc.validate(L);
  }

  /* Capacity */

  /// <summary>
  /// Checks if the container has no elements, i.e. whether begin() == end().
  /// </summary>
  bool empty() const LUA_ALLOC_NOEXCEPT {
    return m_size == 0;
  }

  /// <summary>
  /// Returns the number of elements in the container, i.e. std::distance(begin(), end()).
  /// </summary>
  typename Vector<T>::size_type size() const LUA_ALLOC_NOEXCEPT {
    return m_size;
  }

  /// <summary>
  /// Increase the capacity of the vector to a value that's greater or equal to
  /// new_cap. If new_cap is greater than the current capacity(), new storage is
  /// allocated, otherwise the function does nothing.
  /// </summary>
  void reserve(Vector<T>::size_type new_cap) {
    if (new_cap <= m_capacity)
      return;

    LUA_ALLOC_IF_CONSTEXPR(is_trivial_value) {
      void *data_ = realloc_(static_cast<void *>(m_data), internal_capacity(), new_cap * sizeof(T));
      m_data = static_cast<T *>(data_);
      assert(m_data != LUA_ALLOC_NULLPTR && "Reallocation failed");
    }
    else {
      T *new_data = static_cast<T *>(malloc_(new_cap * sizeof(T)));
      assert(new_data != LUA_ALLOC_NULLPTR && "Allocation failed");

      copyInPlace(begin(), end(), new_data);
      destroyInPlace(begin(), end());

      free_(static_cast<void *>(m_data), internal_capacity());
      m_data = new_data;
    }

    m_capacity = new_cap;
  }

  /// <summary>
  /// Returns the number of elements that the container has currently allocated
  /// space for.
  /// </summary>
  typename Vector<T>::size_type capacity() const LUA_ALLOC_NOEXCEPT {
    return m_capacity;
  }

  /// <summary>
  /// Requests the removal of unused capacity. (C++11)
  ///
  /// If reallocation occurs, all iterators, including the past the end iterator,
  /// and all references to the elements are invalidated. If no reallocation
  /// takes place, no iterators or references are invalidated.
  /// </summary>
  void shrink_to_fit() {
    if (m_size == m_capacity)
      return;

    LUA_ALLOC_IF_CONSTEXPR(is_trivial_value) {
      m_data = static_cast<T *>(realloc_(static_cast<void *>(m_data), internal_capacity(), m_size * sizeof(T)));
      assert(m_data != LUA_ALLOC_NULLPTR && "Reallocation failed");
    }
    else {
      T *new_data = static_cast<T *>(malloc_(m_size * sizeof(T)));
      assert(new_data != LUA_ALLOC_NULLPTR && "Allocation failed");

      copyInPlace(begin(), end(), new_data);
      destroyInPlace(begin(), end());

      free_(static_cast<void *>(m_data), internal_capacity());
      m_data = new_data;
    }

    m_capacity = m_size;
  }

  /* Iterators */

  /// <summary>
  /// Returns an iterator to the first element of the vector.
  /// </summary>
  iterator begin() LUA_ALLOC_NOEXCEPT {
    return m_data;
  }

  /// <summary>
  /// Returns an iterator to the first element of the vector.
  /// </summary>
  const_iterator begin() const LUA_ALLOC_NOEXCEPT {
    return m_data;
  }

  /// <summary>
  /// Returns an iterator to the element following the last element of the
  /// vector.
  /// </summary>
  iterator end() LUA_ALLOC_NOEXCEPT {
    return m_data + m_size;
  }

  /// <summary>
  /// Returns an iterator to the element following the last element of the
  /// vector.
  /// </summary>
  const_iterator end() const LUA_ALLOC_NOEXCEPT {
    return m_data + m_size;
  }

  /* Element Access */

  /// <summary>
  /// Returns a reference to the element at specified location pos. No bounds
  /// checking is performed.
  /// </summary>
  reference operator[](Vector<T>::size_type pos) {
    assert(pos < m_size && "Position is out of bounds");
    return m_data[pos];
  }

  /// <summary>
  /// Returns a reference to the element at specified location pos. No bounds
  /// checking is performed.
  /// </summary>
  const_reference operator[](Vector<T>::size_type pos) const {
    assert(pos < m_size && "Position is out of bounds");
    return m_data[pos];
  }

  /// <summary>
  /// Returns a reference to the element at specified location pos, with bounds
  /// checking. (C++20)
  ///
  /// @NOTE: std::out_of_range exception is avoided for the time being.
  /// </summary>
  reference at(Vector<T>::size_type pos) {
    assert(pos < m_size && "Position is out of bounds");
    return m_data[pos];
  }

  /// <summary>
  /// Returns a reference to the element at specified location pos, with bounds
  /// checking. (C++20)
  /// </summary>
  const_reference at(Vector<T>::size_type pos) const {
    assert(pos < m_size && "Position is out of bounds");
    return m_data[pos];
  }

  /// <summary>
  /// Returns a reference to the first element in the container.
  /// </summary>
  reference front() {
    assert(m_size > 0 && "Container is empty");
    return m_data[0];
  }

  /// <summary>
  /// Returns a reference to the first element in the container.
  /// </summary>
  const_reference front() const {
    assert(m_size > 0 && "Container is empty");
    return m_data[0];
  }

  /// <summary>
  /// Returns a reference to the last element in the container.
  /// </summary>
  reference back() {
    assert(m_size > 0 && "Container is empty");
    return m_data[m_size - 1];
  }

  /// <summary>
  /// Returns a reference to the last element in the container.
  /// </summary>
  const_reference back() const {
    assert(m_size > 0 && "Container is empty");
    return m_data[m_size - 1];
  }

  /// <summary>
  /// Returns pointer to the underlying array serving as element storage. The
  /// pointer is such that range [data(); data() + size()) is always a valid
  /// range.
  /// </summary>
  T *data() LUA_ALLOC_NOEXCEPT {
    return m_data;
  }

  /// <summary>
  /// Returns pointer to the underlying array serving as element storage. The
  /// pointer is such that range [data(); data() + size()) is always a valid
  /// range.
  /// </summary>
  const T *data() const LUA_ALLOC_NOEXCEPT {
    return m_data;
  }

  /* Modifiers */

  /// <summary>
  /// Erases all elements from the container. After this call, size() returns
  /// zero.
  /// </summary>
  void clear() LUA_ALLOC_NOEXCEPT {
    LUA_ALLOC_IF_CONSTEXPR(!is_trivial_value) {
      destroyInPlace(begin(), end());
    }

    m_size = 0;
  }

  /// <summary>
  /// Appends the given element value to the end of the container.
  ///   1) The new element is initialized as a copy of value.
  ///   2) value is moved into the new element.
  ///
  /// If the new size() is greater than capacity() then all iterators and
  /// references (including the past-the-end iterator) are invalidated.
  /// Otherwise only the past-the-end iterator is invalidated.
  /// </summary>
  void push_back(const_reference value) {
    if (m_size == m_capacity) {
      reserve(m_capacity * grow_factor + 1);
    }

    LUA_ALLOC_IF_CONSTEXPR(is_trivial_value) {
      m_data[m_size] = value;
    }
    else {
      new (m_data + m_size) T(value);
    }

    m_size++;
  }

  /// <summary>
  /// Appends the given element value to the end of the container.
  /// </summary>
  void push_back(T &&value) {
    if (m_size == m_capacity) {
      reserve(m_capacity * grow_factor + 1);
    }

    LUA_ALLOC_IF_CONSTEXPR(is_trivial_value) {
      m_data[m_size] = value;
    }
    else {
      new (m_data + m_size) T(std::move(value));
    }

    m_size++;
  }

  /// <summary>
  /// Appends a new element to the end of the container. The element is
  /// constructed through std::allocator_traits::construct, which typically uses
  /// placement-new to construct the element in-place at the location provided
  /// by the container.
  /// </summary>
  template<class... Args>
  void emplace_back(Args &&...args) {
    assert(!is_trivial_value);  // "Use push_back instead of emplace_back for trivial types"
    if (m_size == m_capacity) {
      reserve(m_capacity * grow_factor + 1);
    }

    new (m_data + m_size) T(std::forward<Args>(args)...);
    m_size++;
  }

  /// <summary>
  /// Removes the last element of the container.
  ///
  /// Calling pop_back on an empty container results in undefined behavior.
  ///
  /// Iterators and references to the last element, as well as the end()
  /// iterator, are invalidated.
  /// </summary>
  void pop_back() {
    assert(m_size > 0 && "Container is empty");
    LUA_ALLOC_IF_CONSTEXPR(!is_trivial_value) {
      m_data[m_size - 1].~T();
    }

    m_size--;
  }

  /// <summary>
  /// Resizes the container to contain count elements.
  ///
  /// If the current size is greater than count, the container is reduced to its
  /// first count elements.
  ///
  /// If the current size is less than count,
  ///   1) additional default-inserted elements are appended
  ///   2) additional copies of value are appended.
  /// </summary>
  void resize(Vector<T>::size_type count) {
    if (count == m_size)
      return;
    if (count > m_capacity)
      reserve(count);

    LUA_ALLOC_IF_CONSTEXPR(!is_trivial_value) {
      if (count > m_size)
        constructInPlace(m_data + m_size, m_data + count);
      else if (count < m_size)
        destroyInPlace(m_data + count, m_data + m_size);
    }

    m_size = count;
  }
};
}  // namespace lua

template<class T, class U>
inline bool operator==(const lua::STLAllocator<T> &, const lua::STLAllocator<U> &) {
  return true;
}

template<class T, class U>
inline bool operator!=(const lua::STLAllocator<T> &, const lua::STLAllocator<U> &) {
  return false;
}

#endif
