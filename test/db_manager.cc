// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "db_manager.h"

rms::ValueOrError<std::vector<std::string>> rms::DBManager::get_customers()
    const {
  if (m_current_error) {
    return m_current_error;
  }
  std::vector<std::string> result = {"John", "Steve"};
  return result;
}

void rms::DBManager::set_current_error(std::error_code error) {
  m_current_error = error;
}
