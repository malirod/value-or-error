// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#pragma once

#include <functional>
#include <type_traits>
#include <utility>

template <typename T>
auto operator<<(std::ostream &os, T const &obj)
    -> decltype(obj.serialize(os), os) {
  obj.serialize(os);
  return os;
}

namespace rms {

template <typename S, typename T>
class is_serializable {
 private:
  template <typename OtherS, typename OtherT>
  static auto test(int)
      -> decltype(std::declval<OtherT>().serialize(std::declval<OtherS &>()),
                  std::true_type());

  template <typename, typename>
  static auto test(...) -> std::false_type;

 public:
  static const bool value = decltype(test<S, T>(0))::value;
};

template <typename M, typename N>
class is_streamable {
 private:
  template <typename MM, typename NN>
  static auto test(int)  // NOLINT
      -> decltype(std::declval<MM &>() << std::declval<NN>(), std::true_type());

  template <typename, typename>
  static auto test(...) -> std::false_type;

 public:
  static const bool value = decltype(test<M, N>(0))::value;
};

// For generic types, directly use the result of the signature of its
// 'operator()'
template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())> {};

// Specialize for pointers to member function
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const> {
  typedef ReturnType return_type;

  constexpr static const size_t args_count = sizeof...(Args);

  template <size_t N>
  struct arg {
    // The i-th argument is equivalent to the i-th tuple element of a tuple
    // composed of those arguments.
    typedef typename std::decay<
        typename std::tuple_element<N, std::tuple<Args...>>::type>::type type;
  };
};

template <typename T>
using decay_t = typename std::decay<T>::type;
template <bool E, typename T = void>
using enable_if_t = typename std::enable_if<E, T>::type;

// std::invoke from C++17
// https://stackoverflow.com/questions/38288042/c11-14-invoke-workaround
template <typename Fn, typename... Args,
          typename = enable_if_t<std::is_member_pointer<decay_t<Fn>>{}>,
          int = 0>
constexpr auto invoke(Fn &&f, Args &&... args) noexcept(
    noexcept(std::mem_fn(f)(std::forward<Args>(args)...)))
    -> decltype(std::mem_fn(f)(std::forward<Args>(args)...)) {
  return std::mem_fn(f)(std::forward<Args>(args)...);
}

template <typename Fn, typename... Args,
          typename = enable_if_t<!std::is_member_pointer<decay_t<Fn>>{}>>
constexpr auto invoke(Fn &&f, Args &&... args) noexcept(
    noexcept(std::forward<Fn>(f)(std::forward<Args>(args)...)))
    -> decltype(std::forward<Fn>(f)(std::forward<Args>(args)...)) {
  return std::forward<Fn>(f)(std::forward<Args>(args)...);
}

// std::invoke_result from C++17
template <class F, class, class... Us>
struct invoke_result_impl;

template <class F, class... Us>
struct invoke_result_impl<
    F, decltype(invoke(std::declval<F>(), std::declval<Us>()...), void()),
    Us...> {
  using type = decltype(invoke(std::declval<F>(), std::declval<Us>()...));
};

template <class F, class... Us>
using invoke_result = invoke_result_impl<F, void, Us...>;

template <class F, class... Us>
using invoke_result_t = typename invoke_result<F, Us...>::type;

}  // namespace rms
