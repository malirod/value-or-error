// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "db_manager.h"
#include "db_error.h"

rms::DBManager::DBManager(bool do_interrupt) : m_do_interrupt(do_interrupt) {}

rms::ValueOrError<std::vector<std::string>> rms::DBManager::get_customers()
    const {
  if (m_do_interrupt) {
    return DBError::QueryInterrupted;
  }
  std::vector<std::string> result = {"John", "Steve"};
  return result;
}
