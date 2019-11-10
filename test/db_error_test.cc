// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "db_error.h"

#include <catch2/catch.hpp>

#include "enum_util.h"

using rms::DBError;
using rms::util::enum_util::EnumToString;
using rms::util::enum_util::ToIntegral;

TEST_CASE("Enum to string", "[GeneralErrorEnum]") {
  REQUIRE("Success" == EnumToString(DBError::Success));
  REQUIRE("No Open DB" == EnumToString(DBError::NoOpenDB));
  REQUIRE("Query Interrupted" == EnumToString(DBError::QueryInterrupted));
}

TEST_CASE("To integral", "[GeneralErrorEnum]") {
  REQUIRE(0 == ToIntegral(DBError::Success));
  REQUIRE(1 == ToIntegral(DBError::NoOpenDB));
  REQUIRE(2 == ToIntegral(DBError::QueryInterrupted));
}

TEST_CASE("Makers", "[GeneralErrorEnum]") {
  REQUIRE(make_error_condition(DBError::QueryInterrupted));
  REQUIRE(make_error_code(DBError::QueryInterrupted));
  REQUIRE("Query Interrupted" ==
          make_error_code(DBError::QueryInterrupted).message());
  const std::string name{
      make_error_code(DBError::QueryInterrupted).category().name()};
  REQUIRE("DBError" == name);
}
