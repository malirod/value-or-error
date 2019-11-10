// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "value_or_error.h"

#include <catch2/catch.hpp>
#include <sstream>

namespace {

class Foo {
 public:
  explicit Foo(int m_data) : m_data(m_data) { ++CTorCnt; }

  Foo() { ++CTorCnt; }

  Foo(const Foo& other) {
    ++CopyCTorCnt;
    m_data = other.m_data;
  }

  ~Foo() { ++DTorCnt; }

  Foo(Foo&& rhs) noexcept : m_data(rhs.m_data) {
    rhs.m_data = 0;
    ++MoveCTorCnt;
  }

  Foo& operator=(Foo&& rhs) noexcept {
    m_data = rhs.m_data;
    rhs.m_data = 0;
    ++MoveAssignCnt;
    return *this;
  }

  Foo& operator=(const Foo& rhs) {
    if (this == &rhs) {
      return *this;
    }
    m_data = rhs.m_data;
    ++CopyAssignCnt;
    return *this;
  }

  void set_data(int value) { m_data = value; }

  int get_data() const { return m_data; }

  static std::size_t CTorCnt;
  static std::size_t CopyCTorCnt;
  static std::size_t MoveCTorCnt;
  static std::size_t MoveAssignCnt;
  static std::size_t CopyAssignCnt;
  static std::size_t DTorCnt;

 private:
  int m_data = 0;
};

using ErrorOrFoo = rms::ValueOrError<Foo>;
using UniqueFoo = std::unique_ptr<Foo>;
using UniqueFoos = std::vector<UniqueFoo>;
using ErrorOrUniqueFoo = rms::ValueOrError<UniqueFoo>;
using ErrorOrUniqueFoos = rms::ValueOrError<UniqueFoos>;
using SharedFoo = std::shared_ptr<Foo>;
using ErrorOrSharedFoo = rms::ValueOrError<SharedFoo>;

constexpr int DefaultValue = 10;

std::size_t Foo::CTorCnt = 0;
std::size_t Foo::CopyCTorCnt = 0;
std::size_t Foo::MoveCTorCnt = 0;
std::size_t Foo::MoveAssignCnt = 0;
std::size_t Foo::CopyAssignCnt = 0;
std::size_t Foo::DTorCnt = 0;

rms::ValueOrError<int> get_int_data(int value, bool make_error) {
  if (make_error) {
    return make_error_code(std::errc::no_such_file_or_directory);
  }
  return value;
}

}  // namespace

TEST_CASE("Tests of ValueOrError class", "ValueOrError") {
  Foo::CTorCnt = 0;
  Foo::CopyCTorCnt = 0;
  Foo::MoveCTorCnt = 0;
  Foo::MoveAssignCnt = 0;
  Foo::DTorCnt = 0;

  SECTION("Create with error") {
    {
      auto result = ErrorOrFoo(
          std::make_error_code(std::errc::no_such_file_or_directory));
      REQUIRE(!result);
      REQUIRE(result.has_value() == false);
      REQUIRE(result.error() == std::errc::no_such_file_or_directory);
    }

    REQUIRE(0U == Foo::CTorCnt);
    REQUIRE(0U == Foo::CopyCTorCnt);
    REQUIRE(0U == Foo::MoveCTorCnt);
    REQUIRE(0U == Foo::MoveAssignCnt);
    REQUIRE(0U == Foo::CopyAssignCnt);
    REQUIRE(0U == Foo::DTorCnt);
  }

  SECTION("Implicit error code compare") {
    {
      auto result = ErrorOrFoo(
          std::make_error_code(std::errc::no_such_file_or_directory));
      REQUIRE(!result);
      REQUIRE(!result.has_value());
      REQUIRE(result != std::errc::invalid_argument);
      REQUIRE(result == std::errc::no_such_file_or_directory);
    }

    REQUIRE(0U == Foo::CTorCnt);
    REQUIRE(0U == Foo::CopyCTorCnt);
    REQUIRE(0U == Foo::MoveCTorCnt);
    REQUIRE(0U == Foo::MoveAssignCnt);
    REQUIRE(0U == Foo::CopyAssignCnt);
    REQUIRE(0U == Foo::DTorCnt);
  }

  SECTION("Create with value") {
    {
      auto result = ErrorOrFoo(Foo(DefaultValue));
      REQUIRE(result);
      REQUIRE(!result.error());

      REQUIRE(result.has_value());
      REQUIRE(DefaultValue == result.value().get_data());
      REQUIRE(DefaultValue == result->get_data());
      REQUIRE(DefaultValue == (*result).get_data());
    }

    REQUIRE(1U == Foo::CTorCnt);  // Was created explicitly
    REQUIRE(0U == Foo::CopyCTorCnt);
    REQUIRE(1U == Foo::MoveCTorCnt);  // Was created implicitly
    REQUIRE(0U == Foo::MoveAssignCnt);
    REQUIRE(0U == Foo::CopyAssignCnt);
    REQUIRE(2U == Foo::DTorCnt);  // All were destroyed
  }

  SECTION("Create with help of emplace") {
    {
      auto result = ErrorOrFoo();
      REQUIRE(result);
      REQUIRE(!result.error());
      REQUIRE(!result.has_value());

      result.emplace(DefaultValue);
      REQUIRE(result.has_value());
      REQUIRE(DefaultValue == result.value().get_data());
      REQUIRE(DefaultValue == result->get_data());
    }

    REQUIRE(1U == Foo::CTorCnt);
    REQUIRE(0U == Foo::CopyCTorCnt);
    REQUIRE(2U == Foo::MoveCTorCnt);
    REQUIRE(0U == Foo::MoveAssignCnt);
    REQUIRE(0U == Foo::CopyAssignCnt);
    REQUIRE(3U == Foo::DTorCnt);
  }

  SECTION("Implicit create") {
    auto value1 = get_int_data(DefaultValue, false);
    REQUIRE(value1);
    REQUIRE(value1.has_value());
    REQUIRE(!value1.error());
    REQUIRE(DefaultValue == value1.value());

    auto value2 = get_int_data(DefaultValue, true);
    REQUIRE(!value2);
    REQUIRE(!value2.has_value());
    REQUIRE(value2.error());
  }

  SECTION("Create empty and assign value with implcit convertion") {
    {
      auto result = ErrorOrFoo();
      REQUIRE(result);
      REQUIRE(!result.error());
      REQUIRE(!result.has_value());

      REQUIRE(0U == Foo::CTorCnt);
      REQUIRE(0U == Foo::CopyCTorCnt);
      REQUIRE(0U == Foo::MoveCTorCnt);
      REQUIRE(0U == Foo::MoveAssignCnt);
      REQUIRE(0U == Foo::CopyAssignCnt);
      REQUIRE(0U == Foo::DTorCnt);

      result = Foo(DefaultValue);

      REQUIRE(result.has_value());
      REQUIRE(DefaultValue == result.value().get_data());
      REQUIRE(DefaultValue == result->get_data());
    }

    REQUIRE(1U == Foo::CTorCnt);  // Was emplaced explicitly
    REQUIRE(0U == Foo::CopyCTorCnt);
    REQUIRE(2U == Foo::MoveCTorCnt);
    REQUIRE(0U == Foo::MoveAssignCnt);
    REQUIRE(0U == Foo::CopyAssignCnt);
    REQUIRE(3U == Foo::DTorCnt);  // All were destroyed
  }

  SECTION("Create with unique value") {
    {
      auto result = ErrorOrUniqueFoo(std::make_unique<Foo>(DefaultValue));
      REQUIRE(result);
      REQUIRE(!result.error());
      REQUIRE(result.has_value());

      REQUIRE(DefaultValue == result.value()->get_data());
    }

    REQUIRE(1U == Foo::CTorCnt);
    REQUIRE(0U == Foo::CopyCTorCnt);
    REQUIRE(0U == Foo::MoveCTorCnt);
    REQUIRE(0U == Foo::MoveAssignCnt);
    REQUIRE(0U == Foo::CopyAssignCnt);
    REQUIRE(1U == Foo::DTorCnt);
  }

  SECTION("Create with unique values") {
    std::size_t const cnt = 10U;
    {
      UniqueFoos foos;
      foos.reserve(cnt);
      for (std::size_t i = 0U; i < cnt; ++i) {
        auto item = std::make_unique<Foo>(DefaultValue);
        foos.emplace_back(std::move(item));
      }
      auto result = ErrorOrUniqueFoos(std::move(foos));
      REQUIRE(result);
      REQUIRE(!result.error());

      for (auto&& item : result.value()) {
        REQUIRE(DefaultValue == item->get_data());
      }
    }

    REQUIRE(1U * cnt == Foo::CTorCnt);
    REQUIRE(0U == Foo::CopyCTorCnt);
    REQUIRE(0U == Foo::MoveCTorCnt);
    REQUIRE(0U == Foo::MoveAssignCnt);
    REQUIRE(0U == Foo::CopyAssignCnt);
    REQUIRE(1U * cnt == Foo::DTorCnt);
  }

  SECTION("Extract unique value") {
    {
      auto result = ErrorOrUniqueFoo(std::make_unique<Foo>(DefaultValue));
      REQUIRE(result);
      REQUIRE(!result.error());
      REQUIRE(result.has_value());

      REQUIRE(DefaultValue == result.value()->get_data());
      UniqueFoo another = result.extract();
      REQUIRE(another);
      REQUIRE(!result.has_value());
      REQUIRE(DefaultValue == another->get_data());
    }

    REQUIRE(1U == Foo::CTorCnt);
    REQUIRE(0U == Foo::CopyCTorCnt);
    REQUIRE(0U == Foo::MoveCTorCnt);
    REQUIRE(0U == Foo::MoveAssignCnt);
    REQUIRE(0U == Foo::CopyAssignCnt);
    REQUIRE(1U == Foo::DTorCnt);
  }

  SECTION("Error or bool") {
    rms::ValueOrError<bool> value{std::error_code()};

    REQUIRE(!value.has_value());
    REQUIRE(!value);
    REQUIRE(!value.error());

    rms::ValueOrError<bool> value2{true};
    REQUIRE(value2.has_value());
    REQUIRE(value2);
    REQUIRE(!value2.error());
    REQUIRE(value2.value());
  }

  SECTION("Error or move with value") {
    auto value = ErrorOrUniqueFoo(std::make_unique<Foo>(DefaultValue));

    REQUIRE(value.has_value());
    REQUIRE(value);
    REQUIRE(DefaultValue == (*value)->get_data());
    REQUIRE(!value.error());

    auto moved = std::move(value);

    REQUIRE(moved.has_value());
    REQUIRE(moved);
    REQUIRE(DefaultValue == (*moved)->get_data());
    REQUIRE(!moved.error());
  }

  SECTION("Error or move with error") {
    auto value = ErrorOrUniqueFoo(
        std::make_error_code(std::errc::no_such_file_or_directory));

    auto moved = std::move(value);

    REQUIRE(!moved.has_value());
    REQUIRE(!moved);
    REQUIRE(moved.error());
  }

  SECTION("To stream") {
    rms::ValueOrError<std::string> value1;
    REQUIRE(!value1.has_value());
    std::stringstream ss;
    ss << value1;
    REQUIRE("<empty>" == ss.str());

    ss.str("");
    ss.clear();
    value1 = "DATA";
    REQUIRE(value1.has_value());
    ss << value1;
    REQUIRE("DATA" == ss.str());

    ss.str("");
    ss.clear();
    auto value2 = ErrorOrFoo(Foo(DefaultValue));
    REQUIRE(value2.has_value());
    ss << value2;
    REQUIRE("<value>" == ss.str());

    ss.str("");
    ss.clear();
    auto value3 =
        ErrorOrFoo(std::make_error_code(std::errc::no_such_file_or_directory));
    ss << value3.ignore();
    REQUIRE(
        std::make_error_code(std::errc::no_such_file_or_directory).message() ==
        ss.str());
  }

  SECTION("Double extract unique value") {
    auto action = []() {
      auto result = ErrorOrUniqueFoo(std::make_unique<Foo>(DefaultValue));
      REQUIRE(result);
      REQUIRE(!result.error());
      REQUIRE(result.has_value());

      REQUIRE(DefaultValue == result.value()->get_data());
      UniqueFoo another_first = result.extract();
      REQUIRE(another_first);
      REQUIRE(!result.has_value());
      REQUIRE(DefaultValue == another_first->get_data());
      UniqueFoo another_second = result.extract();  // this should fail
    };
    REQUIRE_THROWS_AS(action(), std::logic_error);
  }

  SECTION("Extract value") {
    {
      auto result = ErrorOrFoo{Foo{DefaultValue}};
      REQUIRE(result);
      REQUIRE(!result.error());
      REQUIRE(result.has_value());

      REQUIRE(DefaultValue == result->get_data());
      Foo another = result.extract();
      REQUIRE(DefaultValue == another.get_data());
      REQUIRE(!result.has_value());
    }

    REQUIRE(1U == Foo::CTorCnt);
    REQUIRE(0U == Foo::CopyCTorCnt);
    REQUIRE(2U == Foo::MoveCTorCnt);
    REQUIRE(0U == Foo::MoveAssignCnt);
    REQUIRE(0U == Foo::CopyAssignCnt);
    REQUIRE(3U == Foo::DTorCnt);
  }

  SECTION("Extract shared value") {
    {
      auto result = ErrorOrSharedFoo(std::make_shared<Foo>(DefaultValue));
      REQUIRE(result);
      REQUIRE(!result.error());
      REQUIRE(result.has_value());

      REQUIRE(DefaultValue == result.value()->get_data());
      SharedFoo another = result.extract();
      REQUIRE(another);
      REQUIRE(!result.has_value());
    }

    REQUIRE(1U == Foo::CTorCnt);
    REQUIRE(0U == Foo::CopyCTorCnt);
    REQUIRE(0U == Foo::MoveCTorCnt);
    REQUIRE(0U == Foo::MoveAssignCnt);
    REQUIRE(0U == Foo::CopyAssignCnt);
    REQUIRE(1U == Foo::DTorCnt);
  }

  SECTION("UnInitialized Value") {
    auto action = []() {
      auto result = ErrorOrFoo(
          std::make_error_code(std::errc::no_such_file_or_directory));
      REQUIRE(std::errc::no_such_file_or_directory == result.error());
      result.value();  // throws
    };
    REQUIRE_THROWS_AS(action(), std::logic_error);
  }

  SECTION("Handled error via operator bool. Case 1.") {
    auto result = ErrorOrFoo(std::error_code());
    if (!result) {
      REQUIRE(std::error_code() == result.error());
    } else {
      REQUIRE(false);  // not expected to go this way
    }
  }

  SECTION("Handled error via operator bool. Case 2.") {
    auto result =
        ErrorOrFoo(std::make_error_code(std::errc::no_such_file_or_directory));
    if (!result) {
      REQUIRE(std::errc::no_such_file_or_directory == result.error());
    } else {
      REQUIRE(false);  // not expected to go this way
    }
  }

  SECTION("Then: handling by T. Sequential positive calls.") {
    // Arrange
    auto action1 = []() { return ErrorOrFoo{Foo{DefaultValue}}; };

    auto action2 = [](Foo const& foo) {
      return ErrorOrFoo{Foo{foo.get_data() + 1}};
    };
    auto action3 = [](Foo const& foo) {
      return ErrorOrFoo(Foo{foo.get_data() + 2});
    };

    // Act
    auto result = action1().then(action2).then(action3);

    // Assert
    REQUIRE(result.has_value());
    REQUIRE(result.value().get_data() == DefaultValue + 3);
  }

  SECTION("Then: handling by T. Second fails.") {
    // Arrange
    auto action1 = []() { return ErrorOrFoo{Foo{DefaultValue}}; };

    auto action2 = [](Foo const& /*unused*/) {
      return ErrorOrFoo{std::make_error_code(std::errc::invalid_argument)};
    };
    auto action3 = [](Foo const& foo) {
      return ErrorOrFoo(Foo{foo.get_data() + 2});
    };

    // Act
    auto result = action1().then(action2).then(action3);

    // Assert
    REQUIRE(!result.has_value());
    REQUIRE(result.error() ==
            std::make_error_code(std::errc::invalid_argument));
  }

  SECTION("Then: handling by ValueOrError. Sequential calls.") {
    // Arrange
    auto action1 = []() { return ErrorOrFoo{Foo{DefaultValue}}; };

    auto action2 = [](ErrorOrFoo const& error_or_foo) {
      REQUIRE(error_or_foo.has_value());
      return ErrorOrFoo{Foo{(*error_or_foo).get_data() + 1}};
    };
    auto action3 = [](ErrorOrFoo const& error_or_foo) {
      REQUIRE(error_or_foo.has_value());
      return ErrorOrFoo{Foo{(*error_or_foo).get_data() + 2}};
    };

    // Act
    auto result = action1().then(action2).then(action3);

    // Assert
    REQUIRE(result.has_value());
    REQUIRE(result.value().get_data() == DefaultValue + 3);
  }
}
