#include <entity/entity_viking.hpp>
#include <tile.hpp>
#include <assets.hpp>

void entityVikingSetSteer(tEntity *pEntity, tSteer *pSteer) {
	auto &Data = pEntity->dataAs<tEntityVikingData>();
	Data.pSteer = pSteer;
}

#define ERIK_SIZE 32
#define ERIK_OFFS_RIGHT 9
#define ERIK_OFFS_LEFT 10

void entityVikingCreate(
	tEntity &Entity, UWORD uwPosX, UWORD uwPosY, UWORD uwCenterX, UWORD uwCenterY,
	UWORD uwParam1, UWORD uwParam2
)
{
	auto &Data = Entity.dataAs<tEntityVikingData>();

	Data.sPos.uwX = uwPosX;
	Data.sPos.uwY = uwPosY;
	Data.ubAnimFrameIdx = 0;
	Data.ubSelectedSlot = 0;

	for(UBYTE i = 0; i < VIKING_INVENTORY_SIZE; ++i) {
		Data.pInventory[i] = tItemKind::None;
	}

	bobInit(
		&Entity.sBob, ERIK_SIZE, ERIK_SIZE, 1,
		bobCalcFrameAddress(g_pBobBmErik, 0), bobCalcFrameAddress(g_pBobBmErikMask, 0),
		uwPosX, uwPosY
	);
	Data.eState = tVikingState::Alive;
	Data.eMoveState = tMoveState::Falling;
}

void entityVikingProcess(tEntity &Entity) {
	auto &Data = Entity.dataAs<tEntityVikingData>();

	tUwCoordYX sNewPos = {.ulYX = Data.sPos.ulYX};
	BYTE bMovingX = 0;

	if(Data.eMoveState == tMoveState::Falling) {
		if(Data.pSteer) {
			if(steerCheck(Data.pSteer, tSteerAction::Left)) {
				bMovingX = -1;
			}
			if(steerCheck(Data.pSteer, tSteerAction::Right)) {
				bMovingX = 1;
			}
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
			Data.eMoveState = tMoveState::Walking;
			Data.ubAnimFrameIdx = 0;
		}
	}
	else if(Data.eMoveState == tMoveState::Climbing) {

	}
	else if(Data.eMoveState == tMoveState::Walking) {
		// Check if it's possible to move left/right
		UBYTE ubSpeedX = 4;
		if(Data.pSteer) {
			if(steerCheck(Data.pSteer, tSteerAction::Left)) {
				bMovingX = -ubSpeedX;
			}
			if(steerCheck(Data.pSteer, tSteerAction::Right)) {
				bMovingX = ubSpeedX;
			}
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
			uwBottomY >= (uwMidTerrainPos = tileGetHeightAtPosX(sNewPos.uwX, uwBottomY + 0)) -2 * ubSpeedX ||
			uwBottomY >= (uwMidTerrainPos = tileGetHeightAtPosX(sNewPos.uwX, uwBottomY - ubSpeedX)) -2 * ubSpeedX ||
			uwBottomY >= (uwMidTerrainPos = tileGetHeightAtPosX(sNewPos.uwX, uwBottomY + ubSpeedX)) -2 * ubSpeedX
		) {
			// There's terrain at last character bob's row or directly below it
			sNewPos.uwY = uwMidTerrainPos - ERIK_SIZE;
			uwBottomY = uwMidTerrainPos - 1;
			if(bMovingX == 0) {
				Data.ubAnimFrameIdx = 0;
				bobSetFrame(
					&Entity.sBob,
					bobCalcFrameAddress(g_pBobBmErik, 17 * ERIK_SIZE),
					bobCalcFrameAddress(g_pBobBmErikMask, 17 * ERIK_SIZE)
				);
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
				bobSetFrame(
					&Entity.sBob,
					bobCalcFrameAddress(g_pBobBmErik, Data.ubAnimFrameIdx * ERIK_SIZE),
					bobCalcFrameAddress(g_pBobBmErikMask, Data.ubAnimFrameIdx * ERIK_SIZE)
				);
				Data.ubAnimFrameIdx = (Data.ubAnimFrameIdx + 1) & 7;
			}
		}
		else {
			// Fall only if there's at least 1px gap between ground and char
			Data.eMoveState = tMoveState::Falling;
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

	entityCheckForCollisionsWith(Entity);
	Data.sPos.ulYX = sNewPos.ulYX;
	BYTE bFrameOffsX = -6;
	Entity.sBob.sPos.uwX = Data.sPos.uwX -10 + bFrameOffsX;
	Entity.sBob.sPos.uwY = Data.sPos.uwY;
	bobPush(&Entity.sBob);
}

void entityVikingDestroy(tEntity &Entity) {

}

tVikingState entityVikingGetState(tEntity *pEntityViking) {
	const auto &VikingData = pEntityViking->dataAs<tEntityVikingData>();
	return VikingData.eState;
}

BYTE entityVikingGetFreeItemSlot(tEntity *pEntityViking) {
	const auto &VikingData = pEntityViking->dataAs<tEntityVikingData>();
	for(UBYTE i = 0; i < 4; ++i) {
		if(VikingData.pInventory[i] == tItemKind::None) {
			return i;
		}
	}
	return -1;
}
