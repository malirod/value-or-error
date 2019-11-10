// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "enum_util.h"

#include <catch2/catch.hpp>
#include <sstream>

namespace {

enum class EnumDefaultInit {
  Foo,
  Bar,
};

enum class EnumCustomInit {
  Foo = 1,
  Bar,
};

}  // namespace

using rms::util::enum_util::EnumFromStream;
using rms::util::enum_util::EnumStrings;
using rms::util::enum_util::EnumToChars;
using rms::util::enum_util::EnumToStream;
using rms::util::enum_util::EnumToString;
using rms::util::enum_util::FromIntegral;
using rms::util::enum_util::ToIntegral;

template <>
EnumStrings<EnumDefaultInit>::DataType EnumStrings<EnumDefaultInit>::data = {
    "FooDef", "BarDef"};

template <>
EnumStrings<EnumCustomInit>::DataType EnumStrings<EnumCustomInit>::data = {
    "Dummy", "FooCustom", "BarCustom"};

TEST_CASE("To intergal", "[EnumUtil]") {
  REQUIRE(ToIntegral(EnumDefaultInit::Foo) == 0);
  REQUIRE(ToIntegral(EnumDefaultInit::Bar) == 1);
  REQUIRE(ToIntegral(EnumCustomInit::Foo) == 1);
  REQUIRE(ToIntegral(EnumCustomInit::Bar) == 2);
}

TEST_CASE("From intergal", "[EnumUtil]") {
  REQUIRE(EnumDefaultInit::Foo == FromIntegral<EnumDefaultInit>(0));
  REQUIRE(EnumDefaultInit::Bar == FromIntegral<EnumDefaultInit>(1));
  REQUIRE(EnumCustomInit::Foo == FromIntegral<EnumCustomInit>(1));
  REQUIRE(EnumCustomInit::Bar == FromIntegral<EnumCustomInit>(2));
}

TEST_CASE("To string stream without init", "[EnumUtil]") {
  std::stringstream sstream;
  sstream << EnumToStream(EnumDefaultInit::Foo) << ","
          << EnumToStream(EnumDefaultInit::Bar);

  REQUIRE(std::string{"FooDef,BarDef"} == sstream.str());
}

TEST_CASE("To string stream with init", "[EnumUtil]") {
  std::stringstream sstream;
  sstream << EnumToStream(EnumCustomInit::Foo) << ","
          << EnumToStream(EnumCustomInit::Bar);

  REQUIRE(std::string{"FooCustom,BarCustom"} == sstream.str());
}

TEST_CASE("From string stream default init", "[EnumUtil]") {
  std::stringstream enum_string("BarDef");
  auto value = EnumCustomInit::Bar;
  enum_string >> EnumFromStream(value);

  REQUIRE(value == EnumCustomInit::Bar);
}

TEST_CASE("To string custom init", "[EnumUtil]") {
  REQUIRE(std::string{"FooDef"} == EnumToString(EnumDefaultInit::Foo));
  REQUIRE(std::string{"BarDef"} == EnumToString(EnumDefaultInit::Bar));
  REQUIRE(std::string{"FooDef"} ==
          std::string{EnumToChars(EnumDefaultInit::Foo)});
  REQUIRE(std::string{"BarDef"} ==
          std::string{EnumToChars(EnumDefaultInit::Bar)});
}

TEST_CASE("To string for invalid enum item", "[EnumUtil]") {
  REQUIRE(std::string{"Dummy"} ==
          EnumToString(FromIntegral<EnumCustomInit>(0)));
  REQUIRE(std::string{"Dummy"} ==
          std::string{EnumToChars(FromIntegral<EnumCustomInit>(0))});
  REQUIRE(EnumToString(FromIntegral<EnumCustomInit>(3)).empty());
  REQUIRE(std::string{EnumToChars(FromIntegral<EnumCustomInit>(3))}.empty());
}
