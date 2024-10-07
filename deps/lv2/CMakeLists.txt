cmake_minimum_required(VERSION 3.14.0)
project(LibLv2)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE LV2_SOURCES inc/*.hpp)
add_library(lv2 INTERFACE ${LV2_SOURCES})
target_include_directories(lv2 INTERFACE inc)
