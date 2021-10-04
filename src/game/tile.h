/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _TILE_H_
#define _TILE_H_

#include <ace/types.h>

typedef enum tTile {
	TILE_EMPTY,
	TILE_FLOOR_FLAT,
	TILE_FLOOR_RAMP16_LEFT,
	TILE_FLOOR_RAMP16_RIGHT,
	TILE_FLOOR_RAMP8_LEFT_START,
	TILE_FLOOR_RAMP8_LEFT_END,
	TILE_FLOOR_RAMP8_RIGHT_START,
	TILE_FLOOR_RAMP8_TIGHT_END,
} tTile;


void tileReset(void);

void tileSetType(UWORD uwTileX, UWORD uwTileY, tTile eTile);

UBYTE tileIsSolid(UWORD uwTileX, UWORD uwTileY);

#endif // _TILE_H_
