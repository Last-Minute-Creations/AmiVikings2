/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_GAME_TILE_HPP
#define INC_GAME_TILE_HPP

#include <ace/types.h>

#define TILE_SHIFT 4
#define TILE_MAP_SIZE 128

void tileReset(void);

void tileSetAttribute(UWORD uwTileX, UWORD uwTileY, UBYTE ubAttribute);

UWORD tileGetHeightAtPosX(UWORD uwX, UWORD uwY);

UBYTE tileIsEmpty(UWORD uwTileX, UWORD uwTileY);

const UBYTE *tileGetHeightmap(UWORD uwTileX, UWORD uwTileY);

#endif // INC_GAME_TILE_HPP
