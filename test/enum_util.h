// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#pragma once

/*
 * Based on
 * http://codereview.stackexchange.com/questions/14309/conversion-between-enum-and-string-in-c-class-header
 */

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>

namespace rms {
namespace util {
namespace enum_util {

template <typename E>
constexpr inline auto ToIntegral(E e) noexcept ->
    typename std::underlying_type<E>::type {
  return static_cast<typename std::underlying_type<E>::type>(e);
}

template <typename E, typename T>
constexpr inline typename std::enable_if<
    std::is_enum<E>::value && std::is_integral<T>::value, E>::type
FromIntegral(T value) noexcept {
  return static_cast<E>(value);
}

/*
 * Reason for warning ignore.
 * In 3.9 clang introduced warning (-Wundefined-var-template) which
 * prevents main idea of enum util: split enum utils and filling Storage
 * in enum's cpp file via template specialization.
 */

#if defined(__clang__)
#pragma clang diagnostic push
#endif

#if defined(__clang__) && \
    (((__clang_major__ > 3) && (__clang_minor__ >= 9)) || __clang_major__ > 3)
#pragma clang diagnostic ignored "-Wundefined-var-template"
#endif

template <typename T>
struct Storage {
  template <typename... ArgTypes>
  // cppcheck-suppress noExplicitConstructor
  Storage(ArgTypes... args)  // NOLINT(runtime/explicit)
      : size(sizeof...(ArgTypes)) {
    static const char* static_data[sizeof...(ArgTypes)] = {args...};
    data = static_data;
  }

  char const** data;
  int const size;
};

// Holds all strings.
// Each enumeration must declare its own specialization.
template <typename T>
struct EnumStrings {
  using DataType = Storage<T>;
  static DataType data;
};

template <typename T>
struct EnumRefHolder {
  T& enum_value;
  explicit EnumRefHolder(T& enum_value) : enum_value(enum_value) {}
};

template <typename T>
struct EnumConstRefHolder {
  T const& enum_value;
  explicit EnumConstRefHolder(T const& enum_value) : enum_value(enum_value) {}
};

// Actual enum to string conversion
template <typename T>
std::ostream& operator<<(std::ostream& stream,
                         EnumConstRefHolder<T> const& data) {
  auto const index = ToIntegral(data.enum_value);
  auto const data_size = EnumStrings<T>::data.size;
  if (index >= 0 && index < data_size) {
    stream << EnumStrings<T>::data.data[index];
  }
  return stream;
}

// Actual enum from string conversion
template <typename T>
std::istream& operator>>(std::istream& stream, EnumRefHolder<T> const& data) {
  std::string value;
  stream >> value;

  static auto begin = EnumStrings<T>::data.data;
  static auto end = EnumStrings<T>::data.data + EnumStrings<T>::data.size;

  auto find = std::find(begin, end, value);
  if (find != end) {
    data.enum_value = static_cast<T>(std::distance(begin, find));
  }

  return stream;
}

// This is the public interface:
// use the ability of function to deuce their template type without
// being explicitly told to create the correct type of enumRefHolder<T>
template <typename T>
EnumConstRefHolder<T> EnumToStream(T const& e) {
  return EnumConstRefHolder<T>(e);
}

template <typename T>
EnumRefHolder<T> EnumFromStream(T& e) {
  return EnumRefHolder<T>(e);
}

template <typename T>
char const* EnumToChars(T const& e) {
  auto const index = ToIntegral(e);
  auto const dataSize = EnumStrings<T>::data.size;
  if (index >= 0 && index < dataSize) {
    return EnumStrings<T>::data.data[index];
  }
  return "";
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

template <typename T>
std::string EnumToString(T const& e) {
  return std::string(EnumToChars<T>(e));
}

// http://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
struct EnumClassHash {
  template <typename T>
  std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};

}  // namespace enum_util
}  // namespace util
}  // namespace rms
