// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
// Outcome spec:
// http://ned14.github.io/outcome/tutorial/advanced/interop/value-or-error/
// ValueOrError proposal:
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0786r0.pdf
// LLVM ErrorOr: http://llvm.org/doxygen/ErrorOr_8h_source.html
// About std::variant: https://www.bfilipek.com/2018/06/variant.html
#pragma once

#include <cassert>
#include <functional>
#include <ostream>
#include <system_error>
#include <type_traits>

#include <boost/variant.hpp>

#include "type_traits.h"

namespace rms {

using std::make_error_code;

template <typename T>
class ValueOrError {
 private:
  using value_type = T;
  using storage_type =
      boost::variant<boost::blank, std::error_code, value_type>;

 public:
  template <typename E,
            typename std::enable_if<
                std::is_error_code_enum<E>::value ||
                std::is_error_condition_enum<E>::value>::type* = nullptr>
  // cppcheck-suppress noExplicitConstructor
  ValueOrError(E error_code)  // NOLINT
      : m_has_error(true), m_storage(make_error_code(error_code)) {}

  // cppcheck-suppress noExplicitConstructor
  ValueOrError(std::error_code error_code)  // NOLINT
      : m_has_error(true), m_storage(error_code) {}

  ValueOrError()
      : m_has_error(false), m_storage(boost::blank()), m_empty_value(true) {}

  template <typename U,
            typename std::enable_if<
                !std::is_error_code_enum<typename std::decay<U>::type>::value &&
                !std::is_error_condition_enum<
                    typename std::decay<U>::type>::value>::type* = nullptr>
  // cppcheck-suppress noExplicitConstructor
  ValueOrError(U&& value)  // NOLINT
      : m_has_error(false),
        m_storage(std::forward<U>(value)),
        m_empty_value(false) {}

  ValueOrError(ValueOrError const& other) { copy_construct(other); }

  template <typename OtherT, typename std::enable_if<std::is_convertible<
                                 OtherT, T>::value>::type* = nullptr>
  // cppcheck-suppress noExplicitConstructor
  ValueOrError(ValueOrError<OtherT> const& other) {
    copy_construct(other);
  }

  template <typename OtherT, typename std::enable_if<!std::is_convertible<
                                 OtherT, T const&>::value>::type* = nullptr>
  explicit ValueOrError(ValueOrError<OtherT> const& other) {
    copy_construct(other);
  }

  ValueOrError(ValueOrError&& other) { move_construct(std::move(other)); }

  template <typename OtherT, typename std::enable_if<std::is_convertible<
                                 OtherT, T>::value>::type* = nullptr>
  // cppcheck-suppress noExplicitConstructor
  ValueOrError(ValueOrError<OtherT>&& other) {
    move_construct(std::move(other));
  }

  template <typename OtherT, typename std::enable_if<!std::is_convertible<
                                 OtherT, T>::value>::type* = nullptr>
  explicit ValueOrError(ValueOrError<OtherT>&& other) {
    move_construct(std::move(other));
  }

  ValueOrError& operator=(ValueOrError const& other) {
    copy_assign(other);
    return *this;
  }

  ValueOrError& operator=(ValueOrError&& other) = default;

  ~ValueOrError() {
    if (m_has_error) {
      // The error should be handled or explicitly ignored
      if (!m_handled) {
        std::abort();
      }
    }
  }

  template <typename... ArgTypes>
  void emplace(ArgTypes&&... args) {
    m_storage = value_type{std::forward<ArgTypes>(args)...};
    m_empty_value = false;
  }

  constexpr explicit operator bool() const noexcept {
    return m_handled = true, !m_has_error;
  }

  std::error_code error() const {
    m_handled = true;
    return m_has_error ? boost::get<std::error_code>(m_storage)
                       : std::error_code();
  }

  bool has_value() const noexcept { return !m_has_error && !m_empty_value; }

  ValueOrError& ignore() noexcept {
    m_handled = true;
    return *this;
  }

  ValueOrError& unignore() noexcept {
    m_handled = false;
    return *this;
  }

  value_type& value() {
    validate_value();
    return boost::get<value_type>(m_storage);
  }

  const value_type& value() const {
    validate_value();
    return boost::get<value_type>(m_storage);
  }

  T&& extract() {
    T&& result = std::move(value());
    m_empty_value = true;
    return std::move(result);
  }

  T* operator->() { return &value(); }

  T const* operator->() const { return &value(); }

  T& operator*() { return value(); }

  T const& operator*() const { return value(); }

  template <typename U>
  friend std::ostream& operator<<(std::ostream& output,
                                  ValueOrError<U> const& obj);

 private:
  template <typename OtherT>
  friend class ValueOrError;

  template <typename OtherT>
  void copy_construct(ValueOrError<OtherT> const& other) {
    m_has_error = other.m_has_error;
    m_storage = other.m_storage;
  }

  template <typename T1>
  static bool compare_this_if_same_type(T1 const& a, T1 const& b) noexcept {
    return &a == &b;
  }

  template <typename T1, typename T2>
  static bool compare_this_if_same_type(T1 const&, T2 const&) noexcept {
    return false;
  }

  template <typename OtherT>
  void copy_assign(ValueOrError<OtherT> const& other) {
    if (compare_this_if_same_type(*this, other)) {
      return;
    }
    ValueOrError copy(other);
    std::swap(*this, copy);
  }

  template <typename OtherT>
  void move_construct(ValueOrError<OtherT>&& other) {
    m_storage = std::move(other.m_storage);
    m_has_error = other.m_has_error;
    m_handled = other.m_handled;
    m_empty_value = other.m_empty_value;
    // mark rhs as handled and without value to satisfy abort condition in the
    // dtor and disallow further usage.
    other.m_handled = true;
    other.m_empty_value = true;
  }

  template <typename OtherT,
            typename std::enable_if<is_streamable<
                std::stringstream, OtherT>::value>::type* = nullptr>
  void value_to_stream(std::ostream& os) const {
    os << value();
  }

  template <typename OtherT,
            typename std::enable_if<!is_streamable<
                std::stringstream, OtherT>::value>::type* = nullptr>
  static void value_to_stream(std::ostream& os) {
    os << "<value>";
  }

  void validate_value() const {
    if (m_has_error) {
      throw std::logic_error("Cannot get value. Error is already stored.");
    }
    const auto is_has_value = has_value();
    if (!is_has_value) {
      throw std::logic_error("Value is not stored.");
    }
  }

  mutable bool m_handled = false;
  bool m_has_error;
  storage_type m_storage;
  // TODO(malirod): query storage whether it's empty or not
  bool m_empty_value = true;
};

template <typename T, typename E>
typename std::enable_if<std::is_error_code_enum<E>::value ||
                            std::is_error_condition_enum<E>::value,
                        bool>::type
operator==(ValueOrError<T> const& error, E code) {
  return error.error() == code;
}

template <typename T, typename E>
typename std::enable_if<std::is_error_code_enum<E>::value ||
                            std::is_error_condition_enum<E>::value,
                        bool>::type
operator!=(ValueOrError<T> const& error, E code) {
  return error.error() != code;
}

template <typename U>
std::ostream& operator<<(std::ostream& output, ValueOrError<U> const& obj) {
  if (obj.m_has_error) {
    output << obj.error().message();
  } else {
    if (!obj.has_value()) {
      output << "<empty>";
    } else {
      obj.template value_to_stream<U>(output);
    }
  }
  return output;
}

}  // namespace rms
