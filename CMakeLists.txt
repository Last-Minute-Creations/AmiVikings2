cmake_minimum_required(VERSION 3.14.0)
project(AmiVikigs2)

# Adhere to GNU filesystem layout conventions
include(GNUInstallDirs)

# C/C++ standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 23)

# Build options
SET(BUILD_GAME ON)
# SET(BUILD_ASSET_EXTRACT ON)

# TODO: https://github.com/Cons-Cat/libCat/tree/b5290853afe5d26c896ba5a635a18ec86f85ebd3
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

	file(GLOB_RECURSE GAME_src src/game/*.cpp src/game/*.c src/game/*.hpp src/game/*.h)

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

	# Semi-final assets
	set(HUD_PLT assets/extracted/converted/hud.plt)
	file(COPY ${HUD_PLT} DESTINATION ${DATA_DIR})
	convertBitmaps(
		TARGET ${LV2_EXECUTABLE} PALETTE ${HUD_PLT}
		SOURCES
			assets/extracted/converted/hud_items.png
			assets/extracted/converted/hud_portraits.png
			assets/extracted/converted/hud_border.png
		INTERLEAVED EHB DESTINATIONS # TODO: remove EHB flag, store 4bpp bitmaps
			${DATA_DIR}/hud_items.bm
			${DATA_DIR}/hud_portraits.bm
			${DATA_DIR}/hud_border.bm
	)
	convertBitmaps(
		TARGET ${LV2_EXECUTABLE} PALETTE ${HUD_PLT} MASK_COLOR "#000000"
		SOURCES
			assets/extracted/converted/hud_cursor.png
		INTERLEAVED EHB DESTINATIONS # TODO: remove EHB flag, store 4bpp bitmaps
			${DATA_DIR}/hud_cursor.bm
		MASKS
			${DATA_DIR}/hud_cursor_mask.bm
	)

	# Temporary assets
	file(COPY tmp/aminer.plt DESTINATION ${DATA_DIR})
	file(COPY tmp/vikings.plt DESTINATION ${DATA_DIR})
	convertBitmaps(
		TARGET ${LV2_EXECUTABLE} PALETTE ${DATA_DIR}/aminer.plt INTERLEAVED
		SOURCES
			tmp/tiles.png
		DESTINATIONS
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
