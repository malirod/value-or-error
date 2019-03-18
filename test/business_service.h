// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#pragma once

#include <string>
#include <vector>

#include "value_or_error.h"

namespace rms {

class DBManager;

class BusinessService {
 public:
  explicit BusinessService(DBManager& db_manager);

  rms::ValueOrError<std::string> get_customer_by_id(uint32_t id) const;

  rms::ValueOrError<bool> is_current_customer_auth() const;

  rms::ValueOrError<bool> is_current_customer_admin() const;

  void set_current_error(std::error_code error);

 private:
  std::error_code m_current_error;

  rms::DBManager& m_db_manager;
};

}  // namespace rms
