// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "business_service_error.h"

#include "enum_util.h"

using rms::BusinessServiceError;

template <>
rms::util::enum_util::EnumStrings<BusinessServiceError>::DataType
    rms::util::enum_util::EnumStrings<BusinessServiceError>::data = {
        "Success", "Item not found", "Operation canceled"};

const std::error_category& rms::BusinessServiceCategory::get() {
  static BusinessServiceCategory instance;
  return instance;
}

std::error_condition rms::make_error_condition(
    BusinessServiceError error) noexcept {
  using rms::util::enum_util::ToIntegral;
  return {ToIntegral(error), BusinessServiceCategory::get()};
}

std::error_code rms::make_error_code(BusinessServiceError error) noexcept {
  using rms::util::enum_util::ToIntegral;
  return {ToIntegral(error), BusinessServiceCategory::get()};
}

const char* rms::BusinessServiceCategory::name() const noexcept {
  return "BusinessServiceCategory";
}

std::string rms::BusinessServiceCategory::message(int error_value) const {
  using rms::util::enum_util::EnumToString;
  using rms::util::enum_util::FromIntegral;
  return EnumToString(FromIntegral<rms::BusinessServiceError>(error_value));
}
