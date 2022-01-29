/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "entity_erik.h"
#include "assets.h"
#include <ace/managers/key.h>
#include "tile.h"

#define ERIK_SIZE 32
#define ERIK_OFFS_RIGHT 9
#define ERIK_OFFS_LEFT 10

tEntityErik *entityErikCreate(UWORD uwPosX, UWORD uwPosY) {
	tEntityErik *pErik = memAllocFast(sizeof(*pErik));
	pErik->sBase.eType = ENTITY_TYPE_ERIK;

	pErik->sPos.uwX = uwPosX;
	pErik->sPos.uwY = uwPosY;
	pErik->ubAnimFrameIdx = 0;
	bobNewInit(
		&pErik->sBase.sBob, ERIK_SIZE, ERIK_SIZE, 1, g_pBobBmErik, g_pBobBmErikMask,
		uwPosX, uwPosY
	);
	pErik->eState = VIKING_STATE_ALIVE;
	pErik->eMoveState = MOVE_STATE_FALLING;
	return pErik;
}

void entityErikProcess(tEntityErik *pEntity) {
	tUwCoordYX sNewPos = {.ulYX = pEntity->sPos.ulYX};
	BYTE bMovingX = 0;

	if(pEntity->eMoveState == MOVE_STATE_FALLING) {
		if(pEntity->eSteer & STEER_LEFT) {
			bMovingX = -1;
		}
		if(pEntity->eSteer & STEER_RIGHT) {
			bMovingX = 1;
		}

		// Gravity - not proper
		sNewPos.uwY += 1;
		UWORD uwBottomY = (sNewPos.uwY + ERIK_SIZE);
		UWORD uwMidTerrainPos = tileGetHeightAtPosX(sNewPos.uwX, uwBottomY);
		if(uwBottomY < uwMidTerrainPos) {
			// still falling
		}
		else {
			// Fell to the ground
			sNewPos.uwY = uwMidTerrainPos - ERIK_SIZE;
			pEntity->eMoveState = MOVE_STATE_WALKING;
			pEntity->ubAnimFrameIdx = 0;
		}
	}
	else if(pEntity->eMoveState == MOVE_STATE_CLIMBING) {

	}
	else if(pEntity->eMoveState == MOVE_STATE_WALKING) {
		// Check if it's possible to move left/right
		if(pEntity->eSteer & STEER_LEFT) {
			bMovingX = -1;
		}
		if(pEntity->eSteer & STEER_RIGHT) {
			bMovingX = 1;
		}
		sNewPos.uwX += bMovingX;

		// Gravity - not proper
		UWORD uwLeftX = sNewPos.uwX - ERIK_OFFS_LEFT;
		UWORD uwLeftTileX = uwLeftX >> TILE_SHIFT;
		UWORD uwRightX = sNewPos.uwX + ERIK_OFFS_RIGHT;
		UWORD uwRightTileX = uwRightX >> TILE_SHIFT;
		UWORD uwBottomY = (sNewPos.uwY + ERIK_SIZE - 1); // Last row of character bob
		UWORD uwMidTileX = sNewPos.uwX >> TILE_SHIFT;
		UWORD uwMidTerrainPos;
		if(
			uwBottomY >= (uwMidTerrainPos = tileGetHeightAtPosX(sNewPos.uwX, uwBottomY + 0)) -2 ||
			uwBottomY >= (uwMidTerrainPos = tileGetHeightAtPosX(sNewPos.uwX, uwBottomY + 1)) -2 ||
			uwBottomY >= (uwMidTerrainPos = tileGetHeightAtPosX(sNewPos.uwX, uwBottomY + 2)) -2
		) {
			// There's terrain at last character bob's row or directly below it
			sNewPos.uwY = uwMidTerrainPos - ERIK_SIZE;
			uwBottomY = uwMidTerrainPos - 1;
			if(bMovingX == 0) {
				pEntity->ubAnimFrameIdx = 0;
				bobNewSetBitMapOffset(&pEntity->sBase.sBob, 17 * ERIK_SIZE);
				// Check if char is sliding to the side
				// UBYTE isFallingLeft = tileGetHeightAtPosX(uwLeftX, uwBottomY) > sNewPos.uwY;
				// UBYTE isFallingRight = tileGetHeightAtPosX(uwRightX, uwBottomY) > sNewPos.uwY;
				// if(isFallingLeft && isFallingRight) {
				// 	// Just fall through
				// }
				// else {
				// 	// No falling now, maybe sliding to the hole
				// 	sNewPos.uwY = uwMidTerrainPos - ERIK_SIZE;
				// 	UWORD uwBottomTileY = uwBottomY;

				// 	if(isFallingLeft) {
				// 		// Check if there's a room to fall to the left
				// 		UWORD uwNextLeftTile = (sNewPos.uwX - 20) >> TILE_SHIFT;
				// 		if(
				// 			!bMovingX && tileIsEmpty(uwNextLeftTile, uwBottomTileY) &&
				// 			tileIsEmpty(uwNextLeftTile, uwBottomTileY - 1) &&
				// 			tileIsEmpty(uwNextLeftTile, uwBottomTileY - 2)
				// 		) {
				// 			// Slide to the left
				// 			--sNewPos.uwX;
				// 		}
				// 	}
				// 	else { // isFallingRight
				// 		// Check if there's a room to fall to the right
				// 		UWORD uwNextRightTile = (sNewPos.uwX + 19) >> TILE_SHIFT;
				// 		if(
				// 			!bMovingX && tileIsEmpty(uwNextRightTile, uwBottomTileY) &&
				// 			tileIsEmpty(uwNextRightTile, uwBottomTileY - 1) &&
				// 			tileIsEmpty(uwNextRightTile, uwBottomTileY - 2)
				// 		) {
				// 			// Slide to the right
				// 			++sNewPos.uwX;
				// 		}
				// 	}
				// }
			}
			else {
				bobNewSetBitMapOffset(&pEntity->sBase.sBob, pEntity->ubAnimFrameIdx * ERIK_SIZE);
				pEntity->ubAnimFrameIdx = (pEntity->ubAnimFrameIdx + 1) & 7;
			}
		}
		else {
			// Fall only if there's at least 1px gap between ground and char
			pEntity->eMoveState = MOVE_STATE_FALLING;
		}


	}

	// {
	// 	// Collision with ceiling
	// 	UWORD uwLeftTileX = (sNewPos.uwX - ERIK_OFFS_LEFT) >> TILE_SHIFT;
	// 	UWORD uwRightTileX = (sNewPos.uwX + ERIK_OFFS_RIGHT) >> TILE_SHIFT;
	// 	UWORD uwMidTileX = sNewPos.uwX >> TILE_SHIFT;
	// 	UWORD uwTopTileY = sNewPos.uwY >> TILE_SHIFT;
	// 	UBYTE isCeilMid = tileIsSolid(uwMidTileX, uwTopTileY);
	// 	UBYTE isCeilLeft = tileIsSolid(uwLeftTileX, uwTopTileY);
	// 	UBYTE isCeilRight = tileIsSolid(uwRightTileX, uwTopTileY);
	// 	if(isCeilMid) {
	// 			// Stop on ceiling
	// 			UWORD uwTopPosY = (uwTopTileY + 1) << TILE_SHIFT;
	// 			sNewPos.uwY = uwTopPosY;
	// 	}
	// 	else if(isCeilLeft || isCeilRight) {
	// 		if(!isCeilLeft) {
	// 			// Stop on ceiling
	// 			UWORD uwTopPosY = (uwTopTileY + 1) << TILE_SHIFT;
	// 			sNewPos.uwY = uwTopPosY;
	// 			// Check if there's a room to slide to the left
	// 			UWORD uwNextLeftTile = (sNewPos.uwX - 20) >> TILE_SHIFT;
	// 			if(
	// 				!bMovingX && !tileIsSolid(uwNextLeftTile, uwTopTileY) &&
	// 				!tileIsSolid(uwNextLeftTile, uwTopTileY - 1) &&
	// 				!tileIsSolid(uwNextLeftTile, uwTopTileY - 2)
	// 			) {
	// 				// Slide to the left
	// 				--sNewPos.uwX;
	// 			}
	// 		}
	// 		else { // !isCeilRight
	// 			// Stop on ceiling
	// 			UWORD uwTopPosY = (uwTopTileY + 1) << TILE_SHIFT;
	// 			sNewPos.uwY = uwTopPosY;
	// 			// Check if there's a room to slide to the right
	// 			UWORD uwNextRightTile = (sNewPos.uwX + 19) >> TILE_SHIFT;
	// 			if(
	// 				!bMovingX && !tileIsSolid(uwNextRightTile, uwTopTileY) &&
	// 				!tileIsSolid(uwNextRightTile, uwTopTileY - 1) &&
	// 				!tileIsSolid(uwNextRightTile, uwTopTileY - 2)
	// 			) {
	// 				// Slide to the right
	// 				++sNewPos.uwX;
	// 			}
	// 		}
	// 	}
	// }

	// {
	// 	// X collision
	// 	sNewPos.uwX += bMovingX;
	// 	UWORD uwLeftTileX = (sNewPos.uwX - ERIK_OFFS_LEFT) >> TILE_SHIFT;
	// 	UWORD uwRightTileX = (sNewPos.uwX + ERIK_OFFS_RIGHT) >> TILE_SHIFT;
	// 	UWORD uwUpperTileY = (sNewPos.uwY) >> TILE_SHIFT;
	// 	UWORD uwMidTileY = (sNewPos.uwY + 16) >> TILE_SHIFT;
	// 	UWORD uwLowerTileY = (sNewPos.uwY + ERIK_SIZE - 1) >> TILE_SHIFT;
	// 	if(bMovingX) {
	// 		if(
	// 			tileIsSolid(uwRightTileX, uwUpperTileY) ||
	// 			tileIsSolid(uwRightTileX, uwMidTileY) ||
	// 			tileIsSolid(uwRightTileX, uwLowerTileY)
	// 		) {
	// 			sNewPos.uwX = (uwRightTileX << TILE_SHIFT) - (1 + ERIK_OFFS_RIGHT);
	// 		}
	// 		else if(
	// 			tileIsSolid(uwLeftTileX, uwUpperTileY) ||
	// 			tileIsSolid(uwLeftTileX, uwMidTileY) ||
	// 			tileIsSolid(uwLeftTileX, uwLowerTileY)
	// 		) {
	// 			sNewPos.uwX = ((uwLeftTileX + 1) << TILE_SHIFT) + ERIK_OFFS_LEFT;
	// 		}
	// 	}
	// }

	pEntity->eSteer = 0;
	pEntity->sPos.ulYX = sNewPos.ulYX;
	BYTE bFrameOffsX = -6;
	pEntity->sBase.sBob.sPos.uwX = pEntity->sPos.uwX -10 + bFrameOffsX;
	pEntity->sBase.sBob.sPos.uwY = pEntity->sPos.uwY;
	bobNewPush(&pEntity->sBase.sBob);
}

void entityErikDestroy(tEntityErik *pEntity) {
	memFree(pEntity, sizeof(*pEntity));
}
