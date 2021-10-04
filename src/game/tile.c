/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "tile.h"

#define TILEMAP_SIZE 128

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

UBYTE tileIsSolid(UWORD uwTileX, UWORD uwTileY) {
	return s_pTileMap[uwTileX][uwTileY] != TILE_EMPTY;
}
