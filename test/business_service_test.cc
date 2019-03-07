// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "business_service.h"
#include "business_service_error.h"
#include "db_error.h"
#include "db_manager.h"

#include <catch2/catch.hpp>

TEST_CASE("Test for BusinessService", "BusinessService") {
  // Global Arrange
  rms::DBManager db_manager;
  rms::BusinessService business_service(db_manager);

  SECTION("Get customer by id without errors") {
    // Arrange
    // Act
    auto customer_or_error = business_service.get_customer_by_id(1u);
    // Assert
    REQUIRE(customer_or_error.has_value());
    auto customer = customer_or_error.extract();
    REQUIRE(customer == "Steve");
  }

  SECTION(
      "Get customer by id with custom error (operation canceled) from "
      "BusinessService layer") {
    // Arrange
    business_service.set_current_error(
        rms::make_error_code(rms::BusinessServiceError::OperationCanceled));
    // Act
    auto customer_or_error = business_service.get_customer_by_id(1u);
    // Assert
    REQUIRE(!customer_or_error.has_value());
    REQUIRE(customer_or_error.error() ==
            rms::BusinessServiceError::OperationCanceled);
  }

  SECTION(
      "Get customer by id with custom error (not found) from BusinessService "
      "layer") {
    // Arrange
    // Act
    auto customer_or_error = business_service.get_customer_by_id(8u);
    // Assert
    REQUIRE(!customer_or_error.has_value());
    REQUIRE(customer_or_error.error() ==
            rms::BusinessServiceError::ItemNotFound);
  }

  SECTION(
      "Get customer by id with standard error from BusinessService "
      "layer") {
    // Arrange
    business_service.set_current_error(
        std::make_error_code(std::errc::device_or_resource_busy));
    // Act
    auto customer_or_error = business_service.get_customer_by_id(8u);
    // Assert
    REQUIRE(!customer_or_error.has_value());
    REQUIRE(customer_or_error.error() == std::errc::device_or_resource_busy);
  }

  SECTION("Get customer by id with custom error from DBManager layer") {
    // Arrange
    db_manager.set_current_error(rms::make_error_code(rms::DBError::NoOpenDB));
    // Act
    auto customer_or_error = business_service.get_customer_by_id(1u);
    // Assert
    REQUIRE(!customer_or_error.has_value());
    REQUIRE(customer_or_error.error() == rms::DBError::NoOpenDB);
  }

  SECTION("Get customer by id with standard error from DBManager layer") {
    // Arrange
    db_manager.set_current_error(
        std::make_error_code(std::errc::device_or_resource_busy));
    // Act
    auto customer_or_error = business_service.get_customer_by_id(1u);
    // Assert
    REQUIRE(!customer_or_error.has_value());
    REQUIRE(customer_or_error.error() == std::errc::device_or_resource_busy);
  }
}