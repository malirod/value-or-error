cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake/cmake-common-utils")
include(PreferClang)
include(PreferCcache)
include(DefaultDebug)
include(PreferGoldLinker)

project(ValueOrError VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Manage dependencies with Conan
include(SetupConan)

find_package(boost_variant REQUIRED)

include(CTest)

# Add possibility to sanitize code
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/tools/cmake/sanitizers-cmake/cmake/")
find_package(Sanitizers REQUIRED)

# Add possibility to get code coverage
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake/CMake-codecov/cmake")
find_package(codecov REQUIRED)

# Set global warnings configuration for all sub-projects
add_compile_options(-Wall -Wextra -pedantic -Werror)

set(LIB_NAME ValueOrError)
set(SRC_LIST
    "src/value_or_error.h"
    "src/type_traits.h")

add_library(${LIB_NAME} ${SRC_LIST})
add_library(rms::${LIB_NAME} ALIAS ${LIB_NAME})
# Since library has no cc files the following line should be added
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

add_sanitizers(${LIB_NAME})
add_coverage(${LIB_NAME} src)
target_include_directories(${LIB_NAME} PUBLIC src)
target_compile_features(${LIB_NAME} PRIVATE cxx_std_14)
target_link_libraries(${LIB_NAME} PUBLIC boost_variant::boost_variant)

if (BUILD_TESTING)
    find_package(Catch2 REQUIRED)

    set(TEST_LIB_NAME "${LIB_NAME}_test")

    set(TEST_SRC_LIST
        "test/value_or_error_test.cc"
        "test/db_error.h"
        "test/db_error.cc"
        "test/db_error_test.cc"
        "test/enum_util.h"
        "test/enum_util_test.cc"
        "test/db_manager.h"
        "test/db_manager.cc"
        "test/db_manager_test.cc"
        "test/business_service.h"
        "test/business_service.cc"
        "test/business_service_test.cc"
        "test/business_service_error.h"
        "test/business_service_error.cc"
        "test/type_traits_test.cc")

    add_library(${TEST_LIB_NAME} OBJECT ${TEST_SRC_LIST})
    add_library(rms::${TEST_LIB_NAME} ALIAS ${TEST_LIB_NAME})

    add_sanitizers(${TEST_LIB_NAME})
    list(APPEND LCOV_REMOVE_PATTERNS "'*/test/*'")
    add_coverage(${TEST_LIB_NAME})

    target_include_directories(${TEST_LIB_NAME} PRIVATE test)
    target_compile_features(${TEST_LIB_NAME} PRIVATE cxx_std_14)
    target_link_libraries(${TEST_LIB_NAME} PUBLIC rms::${LIB_NAME} Catch2::Catch2)

    # define test runner
    set(TEST_RUNNER_NAME testrunner)

    add_executable(${TEST_RUNNER_NAME} "test/testrunner.cc")

    target_compile_features(${TEST_RUNNER_NAME} PRIVATE cxx_std_14)
    target_link_libraries(${TEST_RUNNER_NAME} PRIVATE ${TEST_LIB_NAME})

    add_sanitizers(${TEST_RUNNER_NAME})

    add_test(NAME all COMMAND ${TEST_RUNNER_NAME})
endif()

include(ClangTidy)
include(PrepareDoxygen)
include(ClangStaticAnalyzer)