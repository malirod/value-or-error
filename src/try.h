// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#pragma once

#define VOE_TRY_GLUE2(x, y) x##y
#define VOE_TRY_GLUE(x, y) VOE_TRY_GLUE2(x, y)
#define VOE_TRY_UNIQUE_NAME \
  VOE_TRY_GLUE(value_or_error_try_unique_name_temporary, __COUNTER__)

#define VOE_TRY_EXTRACT_IMPL(unique, v, ...) \
  auto&& unique = (__VA_ARGS__);             \
  if (!(unique).has_value()) {               \
    return (unique).error();                 \
  }                                          \
  auto&& v = std::move((unique).extract());

#define VOE_TRY_EXTRACT(v, ...) \
  VOE_TRY_EXTRACT_IMPL(VOE_TRY_UNIQUE_NAME, v, __VA_ARGS__)
