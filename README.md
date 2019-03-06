# Phone highlight

[![Build Status](https://travis-ci.com/malirod/value-or-error.svg?branch=master)](https://travis-ci.com/malirod/value-or-error)

Implementation of ValueOrError concept.

For details check:

- [Outcome documentation](http://ned14.github.io/outcome/tutorial/advanced/interop/value-or-error/)
- [ValueOrError proposal](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0786r0.pdf)
- [LLVM ErrorOr](http://llvm.org/doxygen/ErrorOr_8h_source.html)

## Platform

Ubuntu 18.10: Clang 7.0, GCC 8.2, Cmake 3.12, Conan

C++14 Standard is used.

Google [style](https://google.github.io/styleguide/cppguide.html) is used.

See `tools/Dockerfile` for details how to setup development environment

## Setup

Submodules are used. Initial command after clone is `git submodule update --init --recursive`.

Assuming all further commands are executed from project root.

### Setup git hook

Run `tools/install_hooks.py`

This will allow to perform some code checks locally before posting changes to server.

### Dependencies

Libs: Boost 1.69, Catch2

Project uses [Conan Package Manager](https://github.com/conan-io/conan)

Install conan with

`sudo -H pip install conan`

CMake will try to automatically setup dependencies.

Add additional repositories to conan:

`conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan`

`conan remote add catchorg https://api.bintray.com/conan/catchorg/Catch2`

Cmake will automatically check required dependencies and setup them taking into account current compiler (clang or gcc).

### Draw deps with cmake

```bash
cmake --graphviz=graph ..
dot graph -T png -o graph.png
```

## Install doxygen

`sudo apt install -y doxygen graphviz`

## Build

### Build commands

By default used clang compiler and debug mode.

Run in project root to build debug version with clang

`mkdir build-clang-debug && cd build-clang-debug && cmake .. && make -j$(nproc)`

To build release version with gcc run the following command

`RUN mkdir build-gcc-release && cd build-gcc-release && CXX=g++ cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(nproc)`

### Build with sanitizers (clang)

You can enable sanitizers with `SANITIZE_ADDRESS`, `SANITIZE_MEMORY`, `SANITIZE_THREAD` or `SANITIZE_UNDEFINED` options in your CMake configuration. You can do this by passing e.g. `-DSANITIZE_ADDRESS=On` in your command line.

## Run

Run from build directory

`ctest`

or

`build/testrunner`

## Coverage report

To enable coverage support in general, you have to enable `ENABLE_COVERAGE` option in your CMake configuration. You can do this by passing `-DENABLE_COVERAGE=On` on your command line or with your graphical interface.

If coverage is supported by your compiler, the specified targets will be build with coverage support. If your compiler has no coverage capabilities (I assume Intel compiler doesn't) you'll get a warning but CMake will continue processing and coverage will simply just be ignored.

Collect coverage in Debug mode. Tested with gcc 8 and clang 7 compiler.

### Sample commands to get coverage html report

```bash
CXX=g++ cmake -DENABLE_COVERAGE=On -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
make test
make lcov-capture
make lcov-genhtml
xdg-open lcov/html/selected_targets/index.html

```

## Integration

### Create docker image

**Dockerfile**: image, which contains basic environment setup (compiler, build tools)

`docker build -t value-or-error -f tools/Dockerfile .`

### Clang static analyzer

Sample command to run analyzer. By default report is stored in `/tmp/scan-build*`

```bash
mkdir build-debug
cd build-debug
scan-build --use-analyzer=/usr/bin/clang++ cmake ..
scan-build --use-analyzer=/usr/bin/clang++ make -j$(nproc)
```

or

```bash
cmake ..
make clang-static-analyzer
```

### Clang-tidy

Setting are stored in `.clang-tidy`.

Run

```bash
mkdir build
cd build
cmake ..
make clang-tidy
```

### Documentation

Code contains doxygen. To generate html documentation run command `make doc`. Output will be in `<build dir>\doc\html`