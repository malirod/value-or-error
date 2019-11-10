FROM ubuntu:19.10

ENV HOME /root
ENV PROJECT_ROOT $HOME/project

ADD . $PROJECT_ROOT

WORKDIR $PROJECT_ROOT

RUN apt update
RUN apt install -y git g++ ninja-build make cmake clang clang-format clang-tidy

# Install Conan
RUN apt install -y python-pip
RUN pip install conan

# Pre-build all dependencies and cache them (they are release so just debug build with different compilers is sufficient)
RUN CXX=g++ CC=gcc cmake -H. -Bbuild-gcc-release -DCMAKE_BUILD_TYPE=Release
RUN CXX=clang++ CC=clang cmake -H. -Bbuild-clang-release -DCMAKE_BUILD_TYPE=Release

# Clean up all possible build dirs
RUN find . -maxdepth 1 -name "build*" -type d -exec rm -r {} +