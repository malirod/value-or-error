// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "db_error.h"

#include "enum_util.h"

using rms::DBError;

template <>
rms::util::enum_util::EnumStrings<DBError>::DataType
    rms::util::enum_util::EnumStrings<DBError>::data = {"Success", "No Open DB",
                                                        "Query Interrupted"};

const std::error_category& rms::DBErrorCategory::get() {
  static DBErrorCategory instance;
  return instance;
}

std::error_condition rms::make_error_condition(DBError error) noexcept {
  using rms::util::enum_util::ToIntegral;
  return {ToIntegral(error), DBErrorCategory::get()};
}

std::error_code rms::make_error_code(DBError error) noexcept {
  using rms::util::enum_util::ToIntegral;
  return {ToIntegral(error), DBErrorCategory::get()};
}

const char* rms::DBErrorCategory::name() const noexcept { return "DBError"; }

std::string rms::DBErrorCategory::message(int error_value) const {
  using rms::util::enum_util::EnumToString;
  using rms::util::enum_util::FromIntegral;
  return EnumToString(FromIntegral<rms::DBError>(error_value));
}
