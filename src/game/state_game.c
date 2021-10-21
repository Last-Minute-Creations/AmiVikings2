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
#include <ace/utils/palette.h>
#include "bob_new.h"
#include "entity_erik.h"
#include "assets.h"
#include "tile.h"
#include "player.h"
#include "hud.h"

#define MAIN_BPP 6
#define BOB_COUNT 4

static tView *s_pView;
static tVPort *s_pVpMain;
static tTileBufferManager *s_pBufferMain;

static tBitMap *s_pTileset;
static tBobNew s_pBobs[BOB_COUNT];

void loadMap(void) {
	tileReset();

	// Test 1 - stop at blocks on sides
	tileSetType(3, 5, TILE_FLOOR_FLAT);
	s_pBufferMain->pTileData[3][5] = TILE_FLOOR_FLAT;

	tileSetType(3, 7, TILE_FLOOR_FLAT);
	s_pBufferMain->pTileData[3][7] = TILE_FLOOR_FLAT;

	tileSetType(4, 7, TILE_FLOOR_FLAT);
	s_pBufferMain->pTileData[4][7] = TILE_FLOOR_FLAT;

	tileSetType(5, 7, TILE_FLOOR_FLAT);
	s_pBufferMain->pTileData[5][7] = TILE_FLOOR_FLAT;

	tileSetType(6, 7, TILE_FLOOR_FLAT);
	s_pBufferMain->pTileData[6][7] = TILE_FLOOR_FLAT;

	tileSetType(7, 7, TILE_FLOOR_FLAT);
	s_pBufferMain->pTileData[7][7] = TILE_FLOOR_FLAT;

	tileSetType(8, 7, TILE_FLOOR_FLAT);
	s_pBufferMain->pTileData[8][7] = TILE_FLOOR_FLAT;

	tileSetType(8, 6, TILE_FLOOR_FLAT);
	s_pBufferMain->pTileData[8][6] = TILE_FLOOR_FLAT;

	tileSetType(10, 6, TILE_FLOOR_FLAT);
	s_pBufferMain->pTileData[10][6] = TILE_FLOOR_FLAT;

	for(UBYTE i = 6; i < 10; ++i) {
		tileSetType(i, 2, TILE_FLOOR_FLAT);
		s_pBufferMain->pTileData[i][2] = TILE_FLOOR_FLAT;
	}

	// Text X
	// s_pBufferMain->pTileData[3][7] = TILE_FLOOR_FLAT;
	// s_pBufferMain->pTileData[4][7] = TILE_FLOOR_FLAT;
	// s_pBufferMain->pTileData[5][7] = TILE_FLOOR_FLAT;
	// s_pBufferMain->pTileData[6][7] = TILE_FLOOR_FLAT;
	// s_pBufferMain->pTileData[7][7] = TILE_FLOOR_RAMP16_RIGHT;
	// s_pBufferMain->pTileData[8][8] = TILE_FLOOR_FLAT;
	// s_pBufferMain->pTileData[9][8] = TILE_FLOOR_FLAT;
	// s_pBufferMain->pTileData[10][7] = TILE_FLOOR_RAMP16_LEFT;
	// s_pBufferMain->pTileData[11][7] = TILE_FLOOR_FLAT;
}

void stateGameCreate(void) {
	UWORD uwCopHudSize = simpleBufferGetRawCopperlistInstructionCount(HUD_BPP);
	UWORD uwCopMainStartSize = tileBufferGetRawCopperlistInstructionCountStart(MAIN_BPP);
	UWORD uwCopMainBreakSize = tileBufferGetRawCopperlistInstructionCountBreak(MAIN_BPP);
	UWORD uwCopListSize = uwCopHudSize + uwCopMainStartSize + uwCopMainBreakSize;
	s_pView = viewCreate(0,
		TAG_VIEW_GLOBAL_CLUT, 1,
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
		TAG_TILEBUFFER_BOUND_TILE_X, 25,
		TAG_TILEBUFFER_BOUND_TILE_Y, 25,
		TAG_TILEBUFFER_REDRAW_QUEUE_LENGTH, 15,
		TAG_TILEBUFFER_TILE_SHIFT, 4,
		TAG_TILEBUFFER_TILESET, s_pTileset,
		TAG_TILEBUFFER_VPORT, s_pVpMain,
		TAG_TILEBUFFER_COPLIST_OFFSET_START, uwCopHudSize,
		TAG_TILEBUFFER_COPLIST_OFFSET_BREAK, uwCopHudSize + uwCopMainStartSize,
	TAG_END);

	assetsGlobalCreate();

	bobNewManagerCreate(
		s_pBufferMain->pScroll->pFront, s_pBufferMain->pScroll->pBack,
		s_pBufferMain->pScroll->uwBmAvailHeight
	);

	// Init entities
	entityManagerReset();
	for(UBYTE i = 0; i < BOB_COUNT; ++i) {
		bobNewInit(&s_pBobs[i], 32, 32, 1, g_pBobBmErik, g_pBobBmErikMask, 32 + 48 * (i + 1), 32);
	}
	tEntity *pPlayerEntity1 = &entityErikCreate(32, 32)->sBase;
	entityAdd(pPlayerEntity1);
	tEntity *pPlayerEntity2 = &entityErikCreate(64, 32)->sBase;
	entityAdd(pPlayerEntity2);

	bobNewReallocateBgBuffers();

	systemUnuse();

	loadMap();
	hudReset((tEntity*[3]){pPlayerEntity1, pPlayerEntity2, 0});

	tileBufferRedrawAll(s_pBufferMain);
	viewLoad(s_pView);
}

void stateGameLoop(void) {
	if(keyUse(KEY_ESCAPE)) {
		gameExit();
		return;
	}

	for(UBYTE ubPlayerIdx = 0; ubPlayerIdx < 2; ++ubPlayerIdx) {
		if(ubPlayerIdx == 1) {
			// TODO: p2 turned off
			continue;
		}
		tSteerRequest eReq = playerProcessSteer(ubPlayerIdx);
		tEntity *pActiveEntity = hudProcessPlayerSteer(ubPlayerIdx, eReq);
		if(pActiveEntity) {
			entitySetSteer(pActiveEntity, eReq);
			if(ubPlayerIdx == 0) {
				cameraCenterAt(
					s_pBufferMain->pCamera,
					pActiveEntity->sBob.sPos.uwX, pActiveEntity->sBob.sPos.uwY
				);
			}
		}
	}

	bobNewBegin(s_pBufferMain->pScroll->pBack);
	tileBufferQueueProcess(s_pBufferMain);

	for(UBYTE i = 0; i < BOB_COUNT; ++i) {
		bobNewPush(&s_pBobs[i]);
	}
	entityManagerProcess();

	bobNewPushingDone();

	bobNewEnd();

	viewProcessManagers(s_pView);
	copProcessBlocks();

	systemIdleBegin();
	vPortWaitUntilEnd(s_pVpMain);
	systemIdleEnd();
}

void stateGameDestroy(void) {
	viewLoad(0);
	systemUse();
	bobNewManagerDestroy();
	assetsGlobalDestroy();
	bitmapDestroy(s_pTileset);
	hudDestroy();
	viewDestroy(s_pView);
}

tState g_sStateGame = {
	.cbCreate = stateGameCreate, .cbLoop = stateGameLoop,
	.cbDestroy = stateGameDestroy
};
