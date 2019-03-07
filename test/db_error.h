// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#pragma once

#include <string>
#include <system_error>

namespace rms {

enum class DBError { Success, NoOpenDB, QueryInterrupted };

class DBErrorCategory : public std::error_category {
 public:
  const char* name() const noexcept override;

  std::string message(int error_value) const override;

  static const std::error_category& get();

 protected:
  DBErrorCategory() = default;
};

std::error_condition make_error_condition(DBError error) noexcept;
std::error_code make_error_code(DBError error) noexcept;

}  // namespace rms

// Register for implicit conversion to error_condition
namespace std {
template <>
struct is_error_condition_enum<rms::DBError> : public true_type {};
}  // namespace std
