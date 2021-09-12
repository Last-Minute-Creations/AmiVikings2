cmake_minimum_required(VERSION 2.8.5)
project(AmiLostVikings2)

# Adhere to GNU filesystem layout conventions
include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

#fmt
add_subdirectory(deps/fmt)
target_include_directories(fmt PUBLIC ${fmt_INCLUDE_DIRS})

# Asset-extracting tool
file(GLOB ASSET_EXTRACT_src src/asset_extract/*.cpp src/asset_extract/*.c src/asset_extract/*.h)
add_executable(asset_extract ${ASSET_EXTRACT_src})

if(MSVC)
	# MSVC needs a few defines to be GNU compatible
	add_compile_definitions(_USE_MATH_DEFINES=1 NOMINMAX=1)
endif()

if(NOT MSVC)
	target_compile_options(asset_extract PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-register>)
endif()

target_link_libraries(asset_extract -static fmt::fmt)
