cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../include ../vendor/yaml-cpp/include)

project(exdir-cpp-tests)

add_executable(test
  tests_main.cpp
  array_tests.cpp
)

if(BUILD_SHARED)
  target_link_libraries(test PUBLIC exdir_shared yaml-cpp_shared gtest)
else()
  target_link_libraries(test PUBLIC exdir_static yaml-cpp_static gtest)
endif()