cmake_minimum_required(VERSION 3.0.0)
project(AmiVikigs2)

# Adhere to GNU filesystem layout conventions
include(GNUInstallDirs)

# C/C++ standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

# Build options
SET(BUILD_GAME ON)
# SET(BUILD_ASSET_EXTRACT ON)

if(BUILD_ASSET_EXTRACT)
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
		target_link_libraries(asset_extract -static)
	endif()

	target_link_libraries(asset_extract fmt::fmt)
endif()

# game
if(AMIGA AND BUILD_GAME)
	add_subdirectory(deps/ace)

	file(GLOB_RECURSE GAME_src src/game/*.cpp src/game/*.c src/game/*.h)

	if(ELF2HUNK)
		set(LV2_EXECUTABLE AmiVikings2.elf)
		set(LV2_OUTPUT_EXECUTABLE AmiVikings2.exe)
		add_executable(${LV2_EXECUTABLE} ${GAME_src})
		target_link_libraries(${LV2_EXECUTABLE} -Wl,-Map=AmiVikings2.map)
		add_custom_command(
			TARGET ${LV2_EXECUTABLE} POST_BUILD
			COMMAND ${ELF2HUNK} ${LV2_EXECUTABLE} ${LV2_OUTPUT_EXECUTABLE}
		)
		add_custom_command(
			TARGET ${LV2_EXECUTABLE} POST_BUILD
			COMMAND ${OBJDUMP} --disassemble -S ${LV2_EXECUTABLE} > AmiVikings2.s
		)
	else()
		SET(LV2_EXECUTABLE AmiVikings2)
		SET(LV2_OUTPUT_EXECUTABLE AmiVikings2)
		add_executable(${LV2_EXECUTABLE} ${GAME_src})
	endif()

	target_compile_options(${LV2_EXECUTABLE} PUBLIC -Wall -Wextra)
	target_include_directories(${LV2_EXECUTABLE} PRIVATE ${PROJECT_SOURCE_DIR}/src/game)
	target_link_libraries(${LV2_EXECUTABLE} ace)

	if(GAME_DEBUG)
		target_compile_definitions(${LV2_EXECUTABLE} PRIVATE GAME_DEBUG)
		target_compile_definitions(ace PUBLIC ACE_DEBUG_UAE)
		# target_compile_definitions(ace PUBLIC ACE_DEBUG_PTPLAYER)
	endif()

	set(DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/data)
	file(MAKE_DIRECTORY ${DATA_DIR} ${DATA_DIR}/hud)

	# Temporary assets
	file(COPY tmp/aminer.plt DESTINATION ${DATA_DIR})
	file(COPY tmp/vikings.plt DESTINATION ${DATA_DIR})
	convertBitmaps(
		TARGET ${LV2_EXECUTABLE} PALETTE ${DATA_DIR}/aminer.plt INTERLEAVED
		SOURCES
			tmp/hud/erik.png tmp/hud/baelog.png tmp/hud/olaf.png
			tmp/hud/scorch.png tmp/hud/fang.png tmp/hud/unk.png
			tmp/tiles.png
		DESTINATIONS
		${DATA_DIR}/hud/erik.bm ${DATA_DIR}/hud/baelog.bm ${DATA_DIR}/hud/olaf.bm
		${DATA_DIR}/hud/scorch.bm ${DATA_DIR}/hud/fang.bm ${DATA_DIR}/hud/unk.bm
		${DATA_DIR}/tiles.bm
	)
	convertBitmaps(
		TARGET ${LV2_EXECUTABLE} PALETTE ${DATA_DIR}/vikings.plt EHB INTERLEAVED
		MASK_COLOR "#FF00FF"
		SOURCES
			tmp/erik.png tmp/baelog.png tmp/olaf.png tmp/fang.png tmp/scorch.png
		DESTINATIONS
			${DATA_DIR}/erik.bm ${DATA_DIR}/baelog.bm ${DATA_DIR}/olaf.bm
			${DATA_DIR}/fang.bm ${DATA_DIR}/scorch.bm
		MASKS
			${DATA_DIR}/erik_mask.bm ${DATA_DIR}/baelog_mask.bm ${DATA_DIR}/olaf_mask.bm
			${DATA_DIR}/fang_mask.bm ${DATA_DIR}/scorch_mask.bm
	)

endif()
