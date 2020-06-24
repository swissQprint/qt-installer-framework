#pragma once

#include <cstdint>
#include "globals.h"

namespace QInstaller {
namespace sqp {
namespace core {

class NullOpt_t {
};

INSTALLER_EXPORT extern NullOpt_t nullopt;

/*
 * Optional can either hold a value of type T or nothing.
 * Specialization is used for references, as it is difficult to handle references as ordinary data types (see below)
 *
 * Since we have no knowledge of how T is constructed, or are able to do so in any reasonable way, we store data in
 * an array instead. With two consequences:
 *
 *  * The compiler recognizes the array as second alias to the data, hence we disable the strict aliasing in value()
 *  * We must construct the data with a placement new into the memory region of m_data. Destructor is also called explicitly.
 *
 * This Optional is similar, but not identical with c++17 std::optional.
 */
template<typename T>
class Optional final {
public:
  using Contained = T;
  Optional(const NullOpt_t &) {}
  Optional& operator = (const NullOpt_t&) { reset(); return *this; }

  template<typename X, std::enable_if_t<!std::is_same<std::decay_t<X>, Optional<T>>::value, void*> ptr=nullptr>
  Optional(X&& data) {
    set(std::forward<X>(data));
  }
  Optional() = default;

  T &operator*() noexcept { return value(); }
  const T &operator*() const noexcept { return value(); }

  bool operator == (const Optional& other) {
    if(hasValue()!=other.hasValue()) {
      return false;
    }
    return hasValue()?value()==other.value():true;
  }

#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
  T &value() noexcept { return *static_cast<T*>(static_cast<void*>(&m_data[0])); }
  const T& value() const noexcept { return *static_cast<const T*>(static_cast<const void*>(&m_data[0])); }
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif
  bool hasValue() const noexcept { return m_valid; }

  void set(const NullOpt_t&) {
    reset();
  }
  template<typename X>
  auto set(X&& v) -> typename std::enable_if<!std::is_same<typename std::decay<X>::type, NullOpt_t>::value, void>::type {
    static_assert(!std::is_same<X, Optional<T>>::value, "Cannot pass optional to set method to set an optional");
    if(m_valid) {
      reset();
    }
    new(&m_data)T(std::forward<X>(v));
    m_valid=true;
  }
  void reset() {
    if(m_valid) {
      value().~T();
      m_valid = false;
    }
  }

  Optional(const Optional& other) {
    if(other.hasValue()) {
      set(other.value());
    } else {
      reset();
    }
  }
  Optional& operator = (const Optional& other) {
    if(other.hasValue()) {
      set(other.value());
    } else {
      reset();
    }
    return *this;
  }
  bool operator == (const Optional& other) const {
    if(m_valid ^ other.m_valid) {
      return false;
    }
    if(m_valid) {
      return eq(value(), other.value(), std::is_floating_point<T>());
    }
    return true;
  }
  bool operator != (const Optional& other) const { return !(*this==other); }
  Optional(Optional&& other) {
    if(other.hasValue()) {
      set(std::move(other.value()));
      other.reset();
    }
  }
  Optional& operator = (Optional&& other) {
    if(other.hasValue()) {
      set(std::move(other.value()));
      other.reset();
    }
    else {
      reset();
    }
    return *this;
  }

  ~Optional() {
    reset();
  }

  bool operator == (const NullOpt_t& ) const noexcept { return !m_valid; }

private:
  char m_data[sizeof(T)]{};
  bool m_valid = false;

  constexpr bool eq (const T& a, const T& b, std::true_type) const { return a == b; }
  constexpr bool eq (const T& a, const T& b, std::false_type) const { return a == b; }
};

/*
 * Optional for references. The reference is translated into a pointer for storage as references cannot be
 * uninitialized.
 */
template<typename T>
class Optional<T&> final {
public:
  using Contained = T&;
  operator bool() const noexcept { return m_valid; }

  Optional(const NullOpt_t &) {}
  Optional& operator = (const NullOpt_t&) { reset(); return *this; }

  template<typename P, std::enable_if_t<std::is_const<T>::value && std::is_same<std::decay_t<P>,Optional<std::remove_const_t<T>&>>::value, void*> n=nullptr>
  Optional(const P& data) {
    if (data.hasValue()) {
      set(data.value());
    };
  }

  Optional(T& data) { set(data); }
  Optional() = default;

  T &operator*() noexcept { return value(); }
  const T &operator*() const noexcept { return value(); }

  T &value() noexcept { return *m_reference; }
  const T& value() const noexcept { return *m_reference; }

  bool hasValue() const noexcept { return m_valid; }

  void set(const NullOpt_t&) {
    reset();
  }
  auto set(T& t) {
    m_reference = &t;
    m_valid=true;
  }
  void reset() {
    m_valid = false;
  }
  T* get() const noexcept { return m_reference; }

private:
  T* m_reference = nullptr;
  bool m_valid = false;
};

}
}
}
