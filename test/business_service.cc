// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "business_service.h"
#include "business_service_error.h"
#include "db_manager.h"

rms::BusinessService::BusinessService(DBManager& db_manager)
    : m_db_manager(db_manager) {}

rms::ValueOrError<std::string> rms::BusinessService::get_customer_by_id(
    uint32_t id) const {
  if (m_current_error) {
    return m_current_error;
  }

  auto customers_or_error = m_db_manager.get_customers();
  if (!customers_or_error) {
    return customers_or_error.error();
  }

  auto customers = customers_or_error.extract();
  // lets assume id == index
  if (id >= customers.size()) {
    return rms::BusinessServiceError::ItemNotFound;
  }

  return customers[id];
}

void rms::BusinessService::set_current_error(std::error_code error) {
  m_current_error = error;
}
