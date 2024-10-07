/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_LV2_LEVEL_LEVEL_DEF_HPP
#define INC_LV2_LEVEL_LEVEL_DEF_HPP

#include <stdint.h>
#include <vector>
#include <lv2/level/level_header.hpp>

namespace lv2::level {

// First section ended with FFFF - always empty?
// Second section ended with FFFF - always empty?
// Third section ended with FFFF - ???
struct tLevelObjectDef {
	int16_t wX; // set to FFFF to finish parsing list.
	int16_t wY;
	int16_t wCenterX;
	int16_t wCenterY;
	uint16_t uwEntityKind;
	uint16_t uwParam1;
	uint16_t uwParam2;
};

// Fourth section ended with FFFF - palette defs
struct tPaletteFragmentLoadDef {
	uint16_t uwFileIndex; // set to FFFF to finish parsing list.
	uint8_t ubPaletePosStart;
};

struct tColorCycleDef {
	uint8_t ubCooldown; // Set to zero to finish parsing list.
	uint8_t ubFirstIndex;
	uint8_t ubLastIndex;
	std::vector<std::uint16_t> vCycleColors;
};

struct tTileCycleDef {
	uint8_t ubCooldown; // Set to zero to finish parsing list.
	uint8_t ubFramesPerTile;
	uint8_t ubUnk1;
	uint8_t ubUnk2;
	uint16_t uwUnk3;
	uint16_t uwFileIndex;
};

struct tGfxPreloadDef {
	uint16_t uwFileIndex; // set to FFFF to finish parsing list.
	uint16_t uwUnk1;
	uint8_t ubBlockWidth;
	uint8_t ubBlockHeight;
};

struct tGfxAnimPreloadDef {
	uint16_t uwFileIndex; // set to FFFF to finish parsing list.
	uint8_t ubBlockWidth;
	uint8_t ubBlockHeight;
	uint8_t isCompressed;
};

struct tLevelDef {
	tLevelHeader Header;
	uint16_t uwUnk1; // always 0xFFFF
	uint16_t uwUnk2; // non-FFFF in DRNK 0V4L T1N3 Y0VR B3SV R3T0 intro_ship
	uint16_t uwUnk3;
	uint16_t uwUnk4;
	std::vector<tLevelObjectDef> vObjects;
	std::vector<tPaletteFragmentLoadDef> vPaletteFragments;
	uint16_t uwColorCycleActivationMask;
	std::vector<tColorCycleDef> vColorCycles;
	uint16_t uwTileCycleActivationMask;
	std::vector<tTileCycleDef> vTileCycles;
	std::vector<tGfxPreloadDef> vGfxPreloads;
	std::vector<tGfxAnimPreloadDef> vAnimPreloads;
};

} // namespace lv2::level

#endif // INC_LV2_LEVEL_LEVEL_DEF_HPP
