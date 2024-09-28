/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_GAME_TILE_HPP
#define INC_GAME_TILE_HPP

#include <ace/types.h>

#define TILE_SHIFT 4

enum class tTile: UBYTE {
	Empty,
	FloorFlat,
	FloorRamp22DownA,
	FloorRamp22DownB,
	FloorRamp45Down,
	FloorRamp45Up,
	FloorRamp22UpA,
	FloorRamp22UpB,
};


void tileReset(void);

void tileSetType(UWORD uwTileX, UWORD uwTileY, tTile eTile);

UWORD tileGetHeightAtPosX(UWORD uwX, UWORD uwY);

UBYTE tileIsEmpty(UWORD uwTileX, UWORD uwTileY);

const UBYTE *tileGetHeightmap(UWORD uwTileX, UWORD uwTileY);

#endif // INC_GAME_TILE_HPP
