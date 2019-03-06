// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "db_manager.h"

#include <catch2/catch.hpp>

TEST_CASE("Get customers without error", "DBManager") {
  const rms::DBManager db_manager{false};
  auto customers_or_error = db_manager.get_customers();
  REQUIRE(customers_or_error.has_value());
  const auto customers = customers_or_error.extract();
  REQUIRE(customers.size() == 2u);
  REQUIRE(customers[0] == "John");
  REQUIRE(customers[1] == "Steve");
}

TEST_CASE("Get customers with error", "DBManager") {
  const rms::DBManager db_manager{true};
  auto customers_or_error = db_manager.get_customers();
  REQUIRE(!customers_or_error.has_value());
  REQUIRE(customers_or_error.error());
  REQUIRE(customers_or_error.error().message() == "Query Interrupted");
}
