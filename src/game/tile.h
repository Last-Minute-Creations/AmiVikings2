/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _TILE_H_
#define _TILE_H_

#include <ace/types.h>

#define TILE_SHIFT 4

typedef enum tTile {
	TILE_EMPTY,
	TILE_FLOOR_FLAT,
	TILE_FLOOR_RAMP22_DOWN_A,
	TILE_FLOOR_RAMP22_DOWN_B,
	TILE_FLOOR_RAMP45_DOWN,
	TILE_FLOOR_RAMP45_UP,
	TILE_FLOOR_RAMP22_UP_A,
	TILE_FLOOR_RAMP22_UP_B,
} tTile;


void tileReset(void);

void tileSetType(UWORD uwTileX, UWORD uwTileY, tTile eTile);

UWORD tileGetHeightAtPosX(UWORD uwX, UWORD uwY);

UBYTE tileIsEmpty(UWORD uwTileX, UWORD uwTileY);

const UBYTE *tileGetHeightmap(UWORD uwTileX, UWORD uwTileY);

#endif // _TILE_H_
