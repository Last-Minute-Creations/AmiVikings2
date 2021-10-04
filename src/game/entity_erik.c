/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "entity_erik.h"
#include "assets.h"
#include <ace/managers/key.h>
#include "tile.h"

#define ERIK_SIZE 32
#define TILE_SHIFT 4

tEntityErik *entityErikCreate(void) {
	tEntityErik *pErik = memAllocFast(sizeof(*pErik));
	pErik->sBase.eType = ENTITY_TYPE_ERIK;

	pErik->sPos.uwX = 32;
	pErik->sPos.uwY = 32;
	bobNewInit(&pErik->sBase.sBob, ERIK_SIZE, ERIK_SIZE, 1, g_pBobBmErik, g_pBobBmErikMask, 32, 32);
	return pErik;
}

void entityErikProcess(tEntityErik *pEntity) {
	tUwCoordYX sNewPos = {.ulYX = pEntity->sPos.ulYX};
	UBYTE isMovingX = 0;
	if(keyCheck(KEY_UP)) {
		sNewPos.uwY -= 2;
	}
	if(keyCheck(KEY_DOWN)) {
		sNewPos.uwY += 1;
	}
	if(keyCheck(KEY_LEFT)) {
		sNewPos.uwX -= 1;
		isMovingX = 1;
	}
	if(keyCheck(KEY_RIGHT)) {
		sNewPos.uwX += 1;
		isMovingX = 1;
	}

	// Gravity - not proper
	sNewPos.uwY += 1;
	UWORD uwBottomTileY = (sNewPos.uwY + ERIK_SIZE) >> TILE_SHIFT;
	UWORD uwLeftTileX = (sNewPos.uwX - 10) >> TILE_SHIFT;
	UWORD uwRightTileX = (sNewPos.uwX + 9) >> TILE_SHIFT;
	UWORD uwMidTileX = sNewPos.uwX >> TILE_SHIFT;
	UBYTE isFallingLeft = !tileIsSolid(uwLeftTileX, uwBottomTileY);
	UBYTE isFallingRight = !tileIsSolid(uwRightTileX, uwBottomTileY);
	UBYTE isFallingMid = !tileIsSolid(uwMidTileX, uwBottomTileY);
	if(isFallingMid) {
		if(isFallingLeft && isFallingRight) {
			// Just fall through
		}
		else if(isFallingLeft) {
			// Check if there's a room to fall to the left
			UWORD uwNextLeftTile = (sNewPos.uwX - 20) >> TILE_SHIFT;
			if(
				!isMovingX && !tileIsSolid(uwNextLeftTile, uwBottomTileY) &&
				!tileIsSolid(uwNextLeftTile, uwBottomTileY - 1) &&
				!tileIsSolid(uwNextLeftTile, uwBottomTileY - 2)
			) {
				// Slide to the left
				--sNewPos.uwX;
			}

			// No falling now
			UWORD uwBottomPosY = uwBottomTileY << TILE_SHIFT;
			sNewPos.uwY = uwBottomPosY - ERIK_SIZE;
		}
		else { // isFallingRight
			// Check if there's a room to fall to the right
			UWORD uwNextRightTile = (sNewPos.uwX + 19) >> TILE_SHIFT;
			if(
				!isMovingX && !tileIsSolid(uwNextRightTile, uwBottomTileY) &&
				!tileIsSolid(uwNextRightTile, uwBottomTileY - 1) &&
				!tileIsSolid(uwNextRightTile, uwBottomTileY - 2)
			) {
				// Slide to the right
				++sNewPos.uwX;
			}

			// No falling now
			UWORD uwBottomPosY = uwBottomTileY << TILE_SHIFT;
			sNewPos.uwY = uwBottomPosY - ERIK_SIZE;
		}
	}
	else {
		// No falling
		UWORD uwBottomPosY = uwBottomTileY << TILE_SHIFT;
		sNewPos.uwY = uwBottomPosY - ERIK_SIZE;
	}

	pEntity->sPos = sNewPos;
	BYTE bFrameOffsX = -6;
	pEntity->sBase.sBob.sPos.uwX = pEntity->sPos.uwX -10 + bFrameOffsX;
	pEntity->sBase.sBob.sPos.uwY = pEntity->sPos.uwY;

	bobNewPush(&pEntity->sBase.sBob);
}

void entityErikDestroy(tEntityErik *pEntity) {
	memFree(pEntity, sizeof(*pEntity));
}
