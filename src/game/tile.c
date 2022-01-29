/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "tile.h"

#define TILEMAP_SIZE 128

// Tile height per x pos, measured from top (full is 0, empty is 16)
static UBYTE s_pTileHeights[][16] = {
	[TILE_EMPTY] = {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
	[TILE_FLOOR_FLAT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	[TILE_FLOOR_RAMP22_DOWN_A] = {1, 1, 2, 2, 3, 3, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8},
	[TILE_FLOOR_RAMP22_DOWN_B] = {9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16},
	[TILE_FLOOR_RAMP45_DOWN] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
	[TILE_FLOOR_RAMP45_UP] = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
	[TILE_FLOOR_RAMP22_UP_A] = {16, 16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10, 9, 9},
	[TILE_FLOOR_RAMP22_UP_B] = {8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1},
};

static tTile s_pTileMap[TILEMAP_SIZE][TILEMAP_SIZE];

void tileReset(void) {
	for(UBYTE ubX = 0; ubX < TILEMAP_SIZE; ++ubX) {
		for(UBYTE ubY = 0; ubY < TILEMAP_SIZE; ++ubY) {
			s_pTileMap[ubX][ubY] = TILE_EMPTY;
		}
	}
}

void tileSetType(UWORD uwTileX, UWORD uwTileY, tTile eTile) {
	s_pTileMap[uwTileX][uwTileY] = eTile;
}

UWORD tileGetHeightAtPosX(UWORD uwX, UWORD uwY) {
	UWORD uwTileX = uwX >> TILE_SHIFT;
	UWORD uwTileY = uwY >> TILE_SHIFT;
	UBYTE ubX = uwX & ((1 << TILE_SHIFT) - 1);
	tTile eTile = s_pTileMap[uwTileX][uwTileY];
	UBYTE ubHeight = s_pTileHeights[eTile][ubX];
	return (uwTileY << TILE_SHIFT) + ubHeight;
}

UBYTE tileIsEmpty(UWORD uwTileX, UWORD uwTileY) {
	UBYTE isEmpty = (s_pTileMap[uwTileX][uwTileY] == TILE_EMPTY);
	return isEmpty;
}

const UBYTE *tileGetHeightmap(UWORD uwTileX, UWORD uwTileY) {
	return s_pTileHeights[s_pTileMap[uwTileX][uwTileY]];
}
