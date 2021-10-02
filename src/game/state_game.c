#include "state_game.h"
#include <ace/managers/viewport/tilebuffer.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/system.h>
#include <ace/managers/game.h>
#include <ace/managers/key.h>
#include <ace/managers/blit.h>
#include <ace/utils/palette.h>
#include "bob_new.h"

#define HUD_BPP 5
#define MAIN_BPP 6
#define BOB_COUNT 6

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

static tView *s_pView;
static tVPort *s_pVpMain, *s_pVpHud;
static tTileBufferManager *s_pBufferMain;
static tSimpleBufferManager *s_pBufferHud;

static tBitMap *s_pTileset;
static tBitMap *s_pHudBaelog;
static tBitMap *s_pBobBmErik, *s_pBobBmErikMask;

static tBobNew s_pBobs[BOB_COUNT];

void loadMap(void) {
	// s_pBufferMain
	s_pBufferMain->pTileData[3][7] = TILE_FLOOR_FLAT;
	s_pBufferMain->pTileData[4][7] = TILE_FLOOR_FLAT;
	s_pBufferMain->pTileData[5][7] = TILE_FLOOR_FLAT;
	s_pBufferMain->pTileData[6][7] = TILE_FLOOR_FLAT;
	s_pBufferMain->pTileData[7][7] = TILE_FLOOR_RAMP16_RIGHT;
	s_pBufferMain->pTileData[8][8] = TILE_FLOOR_FLAT;
	s_pBufferMain->pTileData[9][8] = TILE_FLOOR_FLAT;
	s_pBufferMain->pTileData[10][7] = TILE_FLOOR_RAMP16_LEFT;
	s_pBufferMain->pTileData[11][7] = TILE_FLOOR_FLAT;
}

void stateGameCreate(void) {
	s_pView = viewCreate(0,
		TAG_VIEW_GLOBAL_CLUT, 1,
		TAG_VIEW_COPLIST_MODE, COPPER_MODE_BLOCK,
	TAG_END);

	UWORD pPaletteHud[32];
	paletteLoad("data/aminer.plt", pPaletteHud, 32);
	s_pTileset = bitmapCreateFromFile("data/tiles.bm", 0);

	s_pVpHud = vPortCreate(0,
		TAG_VPORT_BPP, HUD_BPP,
		TAG_VPORT_HEIGHT, 48,
		TAG_VPORT_PALETTE_PTR, pPaletteHud,
		TAG_VPORT_PALETTE_SIZE, 32,
		TAG_VPORT_VIEW, s_pView,
	TAG_END);

	s_pVpMain = vPortCreate(0,
		TAG_VPORT_BPP, MAIN_BPP,
		TAG_VPORT_HEIGHT, 224 - 48,
		TAG_VPORT_PALETTE_PTR, pPaletteHud,
		TAG_VPORT_PALETTE_SIZE, 32,
		TAG_VPORT_VIEW, s_pView,
	TAG_END);

	s_pBufferHud = simpleBufferCreate(0,
		TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR | BMF_INTERLEAVED,
		TAG_SIMPLEBUFFER_IS_DBLBUF, 0,
		TAG_SIMPLEBUFFER_USE_X_SCROLLING, 0,
		TAG_SIMPLEBUFFER_VPORT, s_pVpHud,
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
	TAG_END);

	s_pHudBaelog = bitmapCreateFromFile("data/hud/baelog.bm", 0);
	s_pBobBmErik = bitmapCreateFromFile("data/erik.bm", 0);
	s_pBobBmErikMask = bitmapCreateFromFile("data/erik_mask.bm", 0);

	bobNewManagerCreate(
		s_pBufferMain->pScroll->pFront, s_pBufferMain->pScroll->pBack,
		s_pBufferMain->pScroll->uwBmAvailHeight
	);

	for(UBYTE i = 0; i < BOB_COUNT; ++i) {
		bobNewInit(&s_pBobs[i], 32, 32, 1, s_pBobBmErik, s_pBobBmErikMask, 32 + 48 * i, 32);
	}

	bobNewReallocateBgBuffers();

	systemUnuse();

	// Erik
	blitCopy(s_pHudBaelog, 0, 0, s_pBufferHud->pBack, 16, 16, 32, 24, MINTERM_COOKIE);
	// Baelog
	blitCopy(s_pHudBaelog, 0, 0, s_pBufferHud->pBack, 88, 16, 32, 24, MINTERM_COOKIE);
	// Olaf
	blitCopy(s_pHudBaelog, 0, 0, s_pBufferHud->pBack, 168, 16, 32, 24, MINTERM_COOKIE);

	loadMap();

	tileBufferRedrawAll(s_pBufferMain);
	viewLoad(s_pView);
}

void stateGameLoop(void) {
	if(keyUse(KEY_ESCAPE)) {
		gameExit();
		return;
	}

	bobNewBegin(s_pBufferMain->pScroll->pBack);

	for(UBYTE i = 0; i < BOB_COUNT; ++i) {
		bobNewPush(&s_pBobs[i]);
	}

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
	bitmapDestroy(s_pHudBaelog);
	bitmapDestroy(s_pBobBmErik);
	bitmapDestroy(s_pBobBmErikMask);
	bitmapDestroy(s_pTileset);
	viewDestroy(s_pView);
}

tState g_sStateGame = {
	.cbCreate = stateGameCreate, .cbLoop = stateGameLoop,
	.cbDestroy = stateGameDestroy
};
