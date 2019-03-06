FROM ubuntu:18.10

ENV HOME /root
ENV PROJECT_ROOT $HOME/project

ADD . $PROJECT_ROOT

WORKDIR $PROJECT_ROOT

RUN apt update
RUN apt install -y git g++ ninja-build make cmake clang-7 clang-format-7 clang-tidy-7

# Install Conan
RUN apt install -y python-pip
RUN pip install conan
RUN conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
RUN conan remote add catchorg https://api.bintray.com/conan/catchorg/Catch2

# Pre-build all dependencies and cache them (they are release so just debug build with different compilers is sufficient)
RUN CXX=g++-8 CC=gcc-8 cmake -H. -Bbuild-gcc-release -DCMAKE_BUILD_TYPE=Release
RUN CXX=clang++-7 CC=clang-7 cmake -H. -Bbuild-clang-release -DCMAKE_BUILD_TYPE=Release

# Clean up all possible build dirs
RUN find . -maxdepth 1 -name "build*" -type d -exec rm -r {} +