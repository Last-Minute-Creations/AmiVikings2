cmake_minimum_required(VERSION 3.14.0)
project(LibLmc)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE LMC_SOURCES inc/*.hpp)
add_library(lmc INTERFACE ${LMC_SOURCES})
target_include_directories(lmc INTERFACE inc)
target_link_libraries(lmc INTERFACE ace)
