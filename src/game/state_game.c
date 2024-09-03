/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "state_game.h"
#include <ace/managers/viewport/tilebuffer.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/system.h>
#include <ace/managers/game.h>
#include <ace/managers/key.h>
#include <ace/managers/blit.h>
#include <ace/managers/bob.h>
#include <ace/utils/palette.h>
#include <ace/utils/chunky.h>
#include <ace/utils/font.h>
#include <ace/utils/string.h>
#include <entity/entity_erik.h>
#include "assets.h"
#include "tile.h"
#include "steer.h"
#include "hud.h"
#include "player_controller.h"

#define MAIN_BPP 6
#define BOB_COUNT 4

static tView *s_pView;
static tVPort *s_pVpMain;
static tTileBufferManager *s_pBufferMain;

static tBitMap *s_pTileset;
static tBob s_pBobs[BOB_COUNT];
// static tFont *s_pFont;
// static tTextBitMap *s_pTextTile;

// [y][x]
static UBYTE s_pTilesStrt[][32] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 2, 3, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 5, 4, 5, 1, 1, 6, 7, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 9, 8, 9, 1, 1, 10, 11, 1},
	{12, 13, 14, 15, 16, 17, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 26},
	{27, 28, 29, 30, 31, 32, 19, 33, 34, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 35, 36, 37, 36, 37, 38, 37, 36, 39, 40, 36},
	{37, 36, 36, 37, 36, 37, 36, 37, 36, 41, 42, 43, 1, 1, 1, 1, 1, 1, 44, 45, 46, 47, 48, 49, 50, 49, 48, 50, 51, 52, 53, 48},
	{50, 49, 50, 48, 50, 49, 50, 50, 48, 54, 55, 56, 57, 58, 14, 15, 1, 18, 59, 60, 61, 48, 50, 62, 63, 64, 65, 66, 67, 68, 69, 48},
	{48, 50, 48, 62, 51, 70, 71, 53, 48, 48, 50, 72, 41, 73, 29, 30, 19, 35, 74, 48, 50, 75, 50, 49, 76, 77, 78, 50, 50, 79, 69, 62},
	{50, 66, 53, 50, 69, 68, 80, 81, 52, 63, 64, 48, 54, 55, 37, 37, 36, 47, 49, 62, 65, 71, 66, 53, 48, 50, 51, 70, 82, 65, 83, 48},
	{84, 85, 86, 87, 71, 52, 48, 50, 63, 88, 77, 50, 75, 48, 50, 50, 62, 78, 50, 48, 50, 89, 51, 67, 50, 49, 69, 49, 90, 91, 92, 48},
	{93, 49, 48, 94, 89, 50, 49, 48, 76, 77, 50, 68, 81, 53, 49, 48, 50, 49, 49, 50, 49, 69, 81, 70, 53, 50, 95, 96, 97, 50, 49, 50},
};

void loadMap(void) {
	tileReset();

	for(UBYTE y = 0; y < 13; ++y) {
		for(UBYTE x = 0; x < 32; ++x) {
			UBYTE ubTileIdx = s_pTilesStrt[y][x] - 1;
			s_pBufferMain->pTileData[x][y] = ubTileIdx;
			if(
				(35 <= ubTileIdx && ubTileIdx <= 39) || ubTileIdx == 54 ||
				ubTileIdx == 71 || ubTileIdx == 46 || ubTileIdx == 73 ||
				ubTileIdx == 59
			) {
				tileSetType(x, y, TILE_FLOOR_FLAT);
			}
			else if(ubTileIdx == 40) {
				tileSetType(x, y, TILE_FLOOR_RAMP22_DOWN_A);
			}
			else if(ubTileIdx == 41 || ubTileIdx == 72) {
				tileSetType(x, y, TILE_FLOOR_RAMP22_DOWN_B);
			}
			else if(ubTileIdx == 55) {
				tileSetType(x, y, TILE_FLOOR_RAMP45_DOWN);
			}
			else if(ubTileIdx == 34 || ubTileIdx == 58) {
				tileSetType(x, y, TILE_FLOOR_RAMP45_UP);
			}
			else if(ubTileIdx == 44) {
				tileSetType(x, y, TILE_FLOOR_RAMP22_UP_A);
			}
			else if(ubTileIdx == 45) {
				tileSetType(x, y, TILE_FLOOR_RAMP22_UP_B);
			}
			else {
				tileSetType(x, y, TILE_EMPTY);
			}
		}
	}
	cameraReset(s_pBufferMain->pCamera, 0, 0, 32*16, 13 * 16, 1);
}

void onTileDraw(
	UWORD uwTileX, UWORD uwTileY,
	tBitMap *pBitMap, UWORD uwBitMapX, UWORD uwBitMapY
) {
	// const UBYTE *pTileHeightmap = tileGetHeightmap(uwTileX, uwTileY);
	// for(UBYTE x = 0; x < 16; ++x) {
	// 	chunkyToPlanar(8, uwBitMapX + x, uwBitMapY + pTileHeightmap[x], pBitMap);
	// }
	// UBYTE ubTileIdx = s_pTilesStrt[uwTileY][uwTileX] - 1;
	// if(ubTileIdx) {
	// 	char szTileIdx[4];
	// 	stringDecimalFromULong(ubTileIdx, szTileIdx);
	// 	fontDrawStr(
	// 		s_pFont, pBitMap, uwBitMapX + 8, uwBitMapY + 8, szTileIdx, 8,
	// 		FONT_COOKIE | FONT_SHADOW | FONT_CENTER, s_pTextTile
	// 	);
	// }
}

void stateGameCreate(void) {
	UWORD uwCopHudSize = simpleBufferGetRawCopperlistInstructionCount(HUD_BPP);
	UWORD uwCopMainStartSize = tileBufferGetRawCopperlistInstructionCountStart(MAIN_BPP);
	UWORD uwCopMainBreakSize = tileBufferGetRawCopperlistInstructionCountBreak(MAIN_BPP);
	UWORD uwCopListSize = uwCopHudSize + uwCopMainStartSize + uwCopMainBreakSize;
	s_pView = viewCreate(0,
		TAG_VIEW_GLOBAL_PALETTE, 1,
		TAG_VIEW_COPLIST_MODE, COPPER_MODE_RAW,
		TAG_VIEW_COPLIST_RAW_COUNT, uwCopListSize,
	TAG_END);

	s_pTileset = bitmapCreateFromFile("data/tiles.bm", 0);

	hudCreate(s_pView);

	s_pVpMain = vPortCreate(0,
		TAG_VPORT_BPP, MAIN_BPP,
		TAG_VPORT_VIEW, s_pView,
	TAG_END);

	s_pBufferMain = tileBufferCreate(0,
		TAG_TILEBUFFER_BITMAP_FLAGS, BMF_CLEAR | BMF_INTERLEAVED,
		TAG_TILEBUFFER_IS_DBLBUF, 1,
		TAG_TILEBUFFER_BOUND_TILE_X, 40,
		TAG_TILEBUFFER_BOUND_TILE_Y, 40,
		TAG_TILEBUFFER_REDRAW_QUEUE_LENGTH, 15,
		TAG_TILEBUFFER_TILE_SHIFT, 4,
		TAG_TILEBUFFER_TILESET, s_pTileset,
		TAG_TILEBUFFER_VPORT, s_pVpMain,
		TAG_TILEBUFFER_COPLIST_OFFSET_START, uwCopHudSize,
		TAG_TILEBUFFER_COPLIST_OFFSET_BREAK, uwCopHudSize + uwCopMainStartSize,
		TAG_TILEBUFFER_CALLBACK_TILE_DRAW, onTileDraw,
	TAG_END);

	assetsGlobalCreate();
	// s_pFont = fontCreate("data/uni54.fnt");
	// s_pTextTile = fontCreateTextBitMap(320, 8);
	playerControllerReset();
	playerControllerSetSteerPresets(STEER_KIND_PRESET1, STEER_KIND_NULL);

	bobManagerCreate(
		s_pBufferMain->pScroll->pFront, s_pBufferMain->pScroll->pBack,
		s_pBufferMain->pScroll->uwBmAvailHeight
	);


	for(UBYTE i = 0; i < BOB_COUNT; ++i) {
		bobInit(&s_pBobs[i], 32, 32, 1, bobCalcFrameAddress(g_pBobBmErik, 0), bobCalcFrameAddress(g_pBobBmErikMask, 0), 32 + 48 * (i + 1), 32);
	}

	// Init entities
	entityManagerReset();
	tEntity *pPlayerEntity1 = entityManagerSpawnEntity(ENTITY_KIND_ERIK, 32, 32, 16, 16);
	tEntity *pPlayerEntity2 = entityManagerSpawnEntity(ENTITY_KIND_ERIK, 64, 32, 16, 16);

	playerControllerSetVikingEntity(0, pPlayerEntity1);
	playerControllerSetVikingEntity(1, pPlayerEntity2);
	playerControllerSetVikingEntity(2, 0);

	playerControllerSetDefaultSelection();

	bobReallocateBgBuffers();

	systemUnuse();

	loadMap();
	hudReset();

	tileBufferRedrawAll(s_pBufferMain);
	viewLoad(s_pView);
}

void stateGameLoop(void) {
	if(keyUse(KEY_ESCAPE)) {
		gameExit();
		return;
	}

	for(UBYTE ubPlayerIdx = 0; ubPlayerIdx < 2; ++ubPlayerIdx) {
		tSteer *pSteer = playerControllerGetSteer(ubPlayerIdx);
		steerUpdate(pSteer);
		tEntity *pActiveEntity = hudProcessPlayerSteer(ubPlayerIdx, pSteer);
		if(pActiveEntity) {
			if(ubPlayerIdx == 0) {
				cameraCenterAt(
					s_pBufferMain->pCamera,
					pActiveEntity->sBob.sPos.uwX, pActiveEntity->sBob.sPos.uwY
				);
			}
		}
	}

	bobBegin(s_pBufferMain->pScroll->pBack);
	tileBufferQueueProcess(s_pBufferMain);

	for(UBYTE i = 0; i < BOB_COUNT; ++i) {
		bobPush(&s_pBobs[i]);
	}
	entityManagerProcess();

	bobPushingDone();

	bobEnd();

	viewProcessManagers(s_pView);
	copProcessBlocks();

	systemIdleBegin();
	vPortWaitUntilEnd(s_pVpMain);
	systemIdleEnd();
}

void stateGameDestroy(void) {
	viewLoad(0);
	systemUse();
	bobManagerDestroy();
	assetsGlobalDestroy();
	// fontDestroy(s_pFont);
	// fontDestroyTextBitMap(s_pTextTile);
	bitmapDestroy(s_pTileset);
	hudDestroy();
	viewDestroy(s_pView);
}

tState g_sStateGame = {
	.cbCreate = stateGameCreate, .cbLoop = stateGameLoop,
	.cbDestroy = stateGameDestroy
};
