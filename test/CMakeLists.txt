cmake_minimum_required(VERSION 3.0)
project(rekt.string_enum.tests CXX)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(CMAKE_CXX_STANDARD 11)

add_library(Catch STATIC catch.cpp)

add_executable(tests tests.cpp)
target_link_libraries(tests Catch)
