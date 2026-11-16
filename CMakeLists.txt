cmake_minimum_required(VERSION 3.23)
project(nx-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # the library ends up inside a Python module

find_package(Threads REQUIRED)

# The C++ library: Graph + executors
add_library(nxcpp STATIC src/graph.cpp src/executor.cpp)
target_include_directories(nxcpp PUBLIC include)
target_link_libraries(nxcpp PUBLIC Threads::Threads)
target_compile_options(nxcpp PRIVATE -Wall -Wextra -Wpedantic)

if(SKBUILD)
  # `pip install .` -> build the Python module
  find_package(pybind11 CONFIG REQUIRED)
  pybind11_add_module(_core src/bindings.cpp)
  target_link_libraries(_core PRIVATE nxcpp)
  install(TARGETS _core DESTINATION nx_cpp)
else()
  # plain CMake -> build the C++ tests (Catch2 is installed by Conan)
  include(CTest)
  find_package(Catch2 3 REQUIRED)
  add_executable(nxcpp_tests tests/test_executors.cpp)
  target_link_libraries(nxcpp_tests PRIVATE nxcpp Catch2::Catch2WithMain)
  include(Catch)
  catch_discover_tests(nxcpp_tests)
endif()
