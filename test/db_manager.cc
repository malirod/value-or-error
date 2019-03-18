// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "db_manager.h"

void rms::DBManager::set_current_error(std::error_code error) {
  m_current_error = error;
}

rms::ValueOrError<std::vector<std::string>> rms::DBManager::get_customers()
    const {
  if (m_current_error) {
    return m_current_error;
  }
  std::vector<std::string> result = {"John", "Steve"};
  return result;
}

rms::ValueOrError<std::string> rms::DBManager::get_active_customer() const {
  if (m_current_error) {
    return m_current_error;
  }
  std::vector<std::string> result = {"John", "Steve"};
  return "John";
}

rms::ValueOrError<bool> rms::DBManager::is_auth(
    std::string const& customer) const {
  if (m_current_error) {
    return m_current_error;
  }
  return customer == "John";
}

rms::ValueOrError<bool> rms::DBManager::is_admin(
    std::string const& customer) const {
  if (m_current_error) {
    return m_current_error;
  }
  return customer == "Steve";
}