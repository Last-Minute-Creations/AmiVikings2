/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "tile.hpp"
#include <lmc/enum_value.hpp>

using namespace Lmc;

enum class tHeightTileKind: UBYTE {
	Empty,
	FloorFlat,
	FloorRamp22DownA,
	FloorRamp22DownB,
	FloorRamp45Down,
	FloorRamp45Up,
	FloorRamp22UpA,
	FloorRamp22UpB,
};

// Tile height per x pos, measured from top (full is 0, empty is 16)
static UBYTE s_pHeightTiles[][16] = {
	[enumValue(tHeightTileKind::Empty)] = {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
	[enumValue(tHeightTileKind::FloorFlat)] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	[enumValue(tHeightTileKind::FloorRamp22DownA)] = {1, 1, 2, 2, 3, 3, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8},
	[enumValue(tHeightTileKind::FloorRamp22DownB)] = {9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16},
	[enumValue(tHeightTileKind::FloorRamp45Down)] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
	[enumValue(tHeightTileKind::FloorRamp45Up)] = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
	[enumValue(tHeightTileKind::FloorRamp22UpA)] = {16, 16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10, 9, 9},
	[enumValue(tHeightTileKind::FloorRamp22UpB)] = {8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1},
};

static UBYTE s_pAttributeMap[TILE_MAP_SIZE][TILE_MAP_SIZE];

static constexpr UBYTE *tileAttributeToHeightMap(UBYTE ubAttribute)
{
	switch(ubAttribute) {
		case 0x01:
			return s_pHeightTiles[enumValue(tHeightTileKind::FloorFlat)];
		case 0x32:
			return s_pHeightTiles[enumValue(tHeightTileKind::FloorRamp22DownA)];
		case 0x33:
			return s_pHeightTiles[enumValue(tHeightTileKind::FloorRamp22DownB)];
		case 0x30:
			return s_pHeightTiles[enumValue(tHeightTileKind::FloorRamp45Down)];
		case 0x31:
			return s_pHeightTiles[enumValue(tHeightTileKind::FloorRamp45Up)];
		case 0x35:
			return s_pHeightTiles[enumValue(tHeightTileKind::FloorRamp22UpA)];
		case 0x34:
			return s_pHeightTiles[enumValue(tHeightTileKind::FloorRamp22UpB)];
		default:
			return s_pHeightTiles[enumValue(tHeightTileKind::Empty)];
	}
}

void tileReset(void) {
	for(UBYTE ubX = 0; ubX < TILE_MAP_SIZE; ++ubX) {
		for(UBYTE ubY = 0; ubY < TILE_MAP_SIZE; ++ubY) {
			s_pAttributeMap[ubX][ubY] = 0;
		}
	}
}

void tileSetAttribute(UWORD uwTileX, UWORD uwTileY, UBYTE ubAttribute) {
	s_pAttributeMap[uwTileX][uwTileY] = ubAttribute;
}

UWORD tileGetHeightAtPosX(UWORD uwX, UWORD uwY) {
	UWORD uwTileX = uwX >> TILE_SHIFT;
	UWORD uwTileY = uwY >> TILE_SHIFT;
	UBYTE ubX = uwX & ((1 << TILE_SHIFT) - 1);
	UBYTE ubAttribute = s_pAttributeMap[uwTileX][uwTileY];
	UBYTE ubHeight = tileAttributeToHeightMap(ubAttribute)[ubX];
	return (uwTileY << TILE_SHIFT) + ubHeight;
}

UBYTE tileIsEmpty(UWORD uwTileX, UWORD uwTileY) {
	UBYTE isEmpty = (s_pAttributeMap[uwTileX][uwTileY] == 0);
	return isEmpty;
}

const UBYTE *tileGetHeightmap(UWORD uwTileX, UWORD uwTileY) {
	return tileAttributeToHeightMap(s_pAttributeMap[uwTileX][uwTileY]);
}
