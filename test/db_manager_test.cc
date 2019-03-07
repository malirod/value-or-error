// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "db_manager.h"
#include "db_error.h"

#include <catch2/catch.hpp>

TEST_CASE("Get customers without error", "DBManager") {
  const rms::DBManager db_manager;
  auto customers_or_error = db_manager.get_customers();
  REQUIRE(customers_or_error.has_value());
  const auto customers = customers_or_error.extract();
  REQUIRE(customers.size() == 2u);
  REQUIRE(customers[0] == "John");
  REQUIRE(customers[1] == "Steve");
}

TEST_CASE("Get customers with DB error", "DBManager") {
  // Arrange
  rms::DBManager db_manager;
  db_manager.set_current_error(
      rms::make_error_code(rms::DBError::QueryInterrupted));

  // Act
  auto customers_or_error = db_manager.get_customers();

  // Assert
  REQUIRE(!customers_or_error.has_value());
  REQUIRE(customers_or_error.error());
  REQUIRE(customers_or_error.error().message() == "Query Interrupted");
}

TEST_CASE("Get customers with standard error", "DBManager") {
  // Arrange
  rms::DBManager db_manager;
  db_manager.set_current_error(
      std::make_error_code(std::errc::device_or_resource_busy));

  // Act
  auto customers_or_error = db_manager.get_customers();

  // Assert
  REQUIRE(!customers_or_error.has_value());
  REQUIRE(customers_or_error.error() == std::errc::device_or_resource_busy);
}