// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#pragma once

#include <string>
#include <vector>

#include "value_or_error.h"

namespace rms {

class DBManager {
 public:
  rms::ValueOrError<std::vector<std::string>> get_customers() const;

  rms::ValueOrError<std::string> get_active_customer() const;

  rms::ValueOrError<bool> is_auth(std::string const& customer) const;

  rms::ValueOrError<bool> is_admin(std::string const& customer) const;

  void set_current_error(std::error_code error);

 private:
  std::error_code m_current_error;
};

}  // namespace rms
