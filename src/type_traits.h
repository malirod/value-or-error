// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#pragma once

#include <type_traits>

namespace rms {

template <typename M, typename N>
class is_streamable {
 private:
  template <typename MM, typename NN>
  static auto test(int)  // NOLINT
      -> decltype(std::declval<MM&>() << std::declval<NN>(), std::true_type());

  template <typename, typename>
  static auto test(...) -> std::false_type;

 public:
  static const bool value = decltype(test<M, N>(0))::value;
};

}  // namespace rms
