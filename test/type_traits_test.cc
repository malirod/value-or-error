// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#include "type_traits.h"

#include <sstream>

#include <catch2/catch.hpp>

namespace {

class Foo {};

class Bar {
 public:
  void serialize(std::ostream& os) const { os << "this-is-bar"; }
};

template <typename T>
class Wrapper {
 public:
  explicit Wrapper(T value) : m_value(std::move(value)) {}

  const T& get() const { return m_value; }

  template <typename OtherT,
            typename std::enable_if<rms::is_streamable<
                std::stringstream, OtherT>::value>::type* = nullptr>
  void to_stream(std::ostream& os) const {
    os << get();
  }

  template <typename OtherT,
            typename std::enable_if<!rms::is_streamable<
                std::stringstream, OtherT>::value>::type* = nullptr>
  void to_stream(std::ostream& os) const {
    os << "<value>";
  }

 private:
  T m_value;
};

template <typename U>
std::ostream& operator<<(std::ostream& output, const Wrapper<U>& obj) {
  obj.template to_stream<U>(output);
  return output;
}

}  // namespace

static_assert(rms::is_streamable<std::stringstream, Bar>::value,
              "Bar should be streamable");
static_assert(!rms::is_streamable<std::stringstream, Foo>::value,
              "Foo should be not streamable");
static_assert(rms::is_streamable<std::stringstream, std::string>::value,
              "std::string should be streamable");

static_assert(rms::is_serializable<std::stringstream, Bar>::value,
              "Bar should be serializable");
static_assert(!rms::is_serializable<std::stringstream, Foo>::value,
              "Foo should be not serializable");
static_assert(!rms::is_serializable<std::stringstream, std::string>::value,
              "std::string should be not serializable");

TEST_CASE("Test streamable", "Type Traits") {
  bool test = rms::is_streamable<std::stringstream, std::string>::value;
  REQUIRE(test);
}

TEST_CASE("Serializable is streamable", "Type Traits") {
  std::stringstream ss;
  ss << Bar();
  REQUIRE(ss.str() == "this-is-bar");
  bool test = rms::is_streamable<std::stringstream, Bar>::value;
  REQUIRE(test);
}

TEST_CASE("Test not streamable", "Type Traits") {
  bool test = rms::is_streamable<std::stringstream, Foo>::value;
  REQUIRE(!test);
}

TEST_CASE("Streamable Wrapper", "Type Traits") {
  Wrapper<std::string> value("DATA");
  REQUIRE(value.get() == "DATA");
  std::stringstream ss;
  ss << value;
  REQUIRE(ss.str() == "DATA");
}

TEST_CASE("Non Streamable Wrapper", "Type Traits") {
  Wrapper<Foo> value(Foo{});
  std::stringstream ss;
  ss << value;
  REQUIRE(ss.str() == "<value>");
}

TEST_CASE("Functional wrapper", "Type Traits") {
  auto lambdaNoArgs = []() { return int64_t(10); };
  auto lambda = [](int i, const Wrapper<double>& bar) {
    return int64_t(i * 10 * bar.get());
  };

  using LambdaNoArgs = rms::function_traits<decltype(lambdaNoArgs)>;
  using Traits = rms::function_traits<decltype(lambda)>;

  static_assert(std::is_same<int64_t, LambdaNoArgs::return_type>::value,
                "Wrong result type");
  static_assert(LambdaNoArgs::args_count == 0, "Wrong argument count");

  static_assert(std::is_same<int64_t, Traits::return_type>::value,
                "Wrong result type");
  static_assert(std::is_same<int, Traits::arg<0>::type>::value,
                "Wrong first argument type");
  static_assert(std::is_same<Wrapper<double>, Traits::arg<1>::type>::value,
                "Wrong second argument type");
  static_assert(Traits::args_count == 2, "Wrong second argument count");
}

TEST_CASE("Invoke", "Type Traits") {
  bool called1 = false;
  auto callable_without_params = [&called1]() {
    called1 = true;
    return called1;
  };

  auto const result1 = rms::invoke(callable_without_params);
  REQUIRE(result1);
  REQUIRE(called1);

  bool called2 = false;
  auto callable_with_params = [&called2](int a, int b) {
    called2 = true;
    return a + b;
  };
  auto const result2 = rms::invoke(callable_with_params, 3, 5);
  REQUIRE(result2 == 8);
  REQUIRE(called2);
}