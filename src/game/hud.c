/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hud.h"
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/utils/palette.h>
#include <ace/utils/string.h>
#include <entity/entity_viking.h>

#define HUD_PORTRAIT_WIDTH 32
#define HUD_PORTRAIT_HEIGHT 24
#define HUD_TILE_WIDTH 32
#define HUD_TILE_HEIGHT 6
#define HUD_TILE_SIZE 8
#define HUD_ITEM_SIZE 16
#define HUD_PORTRAIT_OFFSET_Y 16
#define HUD_SELECTED_SLOT_NONE 255

typedef enum tHudIcon {
	HUD_ICON_ERIK_ACTIVE,
	HUD_ICON_BAELOG_ACTIVE,
	HUD_ICON_OLAF_ACTIVE,
	HUD_ICON_FANG_ACTIVE,
	HUD_ICON_SCORCH_ACTIVE,

	HUD_ICON_ERIK_INACTIVE,
	HUD_ICON_BAELOG_INACTIVE,
	HUD_ICON_OLAF_INACTIVE,
	HUD_ICON_FANG_INACTIVE,
	HUD_ICON_SCORCH_INACTIVE,

	HUD_ICON_ERIK_DEAD,
	HUD_ICON_BAELOG_DEAD,
	HUD_ICON_OLAF_DEAD,
	HUD_ICON_FANG_DEAD,
	HUD_ICON_SCORCH_DEAD,

	HUD_ICON_LOCKED,
} tHudIcon;

static tVPort *s_pVpHud;
static tSimpleBufferManager *s_pBufferHud;
static tBitMap *s_pPortraits;
static tBitMap *s_pItems;
static tBitMap *s_pCursor;
static tBitMap *s_pCursorMask;

static const UBYTE s_pPortraitOffsetsX[VIKING_ENTITY_MAX] = {16, 88, 160};
static const tUbCoordYX s_pItemOffsets[] = {
	{.ubX = HUD_PORTRAIT_WIDTH, .ubY = 0},
	{.ubX = HUD_PORTRAIT_WIDTH + HUD_ITEM_SIZE, .ubY = 0},
	{.ubX = HUD_PORTRAIT_WIDTH, .ubY = HUD_ITEM_SIZE},
	{.ubX = HUD_PORTRAIT_WIDTH + HUD_ITEM_SIZE, .ubY = HUD_ITEM_SIZE},
};

// [y][x], using reduced tileset
// TODO: extract from ROM properly, then remap to reduced tileset
static const UBYTE s_pHudTilemap[HUD_TILE_HEIGHT][HUD_TILE_WIDTH] = {
	{ 0,  1,  2,  3,  4,  4,  4,  5,  6,  7,  8,  2,  3,  4,  4,  4,  5,  6,  7,  8,  2,  3,  4,  4,  4,  5,  6,  7,  8,  2,  9,  7},
	{10, 11, 12, 13, 14, 14, 14, 15, 16, 17, 18, 12, 13, 14, 14, 14, 15, 16, 17, 18, 12, 13, 14, 14, 14, 15, 16, 17, 19, 12, 20, 21},
	{22, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 24, 24, 24, 24, 25, 25, 25, 25, 26, 24, 24, 24, 24, 25, 25, 25, 25, 27, 28, 29, 21},
	{22, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 24, 24, 24, 24, 25, 25, 25, 25, 26, 24, 24, 24, 24, 25, 25, 25, 25, 30, 31, 32, 21},
	{22, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 24, 24, 24, 24, 25, 25, 25, 25, 26, 24, 24, 24, 24, 25, 25, 25, 25,  0,  6, 33, 21},
	{34, 35, 36, 36, 36, 37, 25, 25, 25, 25, 38, 36, 36, 36, 37, 25, 25, 25, 25, 38, 36, 36, 36, 37, 25, 25, 25, 25, 34, 39, 40, 41},
};

static void hudDrawItemSlot(UBYTE ubVikingIdx, UBYTE ubSlotIdx, UBYTE isSelected) {
	const tUbCoordYX *pItemSlotOffs = &s_pItemOffsets[ubSlotIdx];

	const tEntity *pEntity = (tEntity*)playerControllerGetVikingByIndex(ubVikingIdx);
	tItemKind eItem;
	if(pEntity) {
		const tEntityVikingData *pVikingData = (tEntityVikingData *)pEntity->pData;
		eItem = pVikingData->pInventory[ubSlotIdx];
	}
	else {
		eItem = ITEM_KIND_NONE;
	}

	blitCopy( // s_pItems first icon is trashcan, so start with one further
		s_pItems, 0, (1 + eItem) * HUD_ITEM_SIZE,
		s_pBufferHud->pBack,
		s_pPortraitOffsetsX[ubVikingIdx] + pItemSlotOffs->ubX,
		HUD_PORTRAIT_OFFSET_Y + pItemSlotOffs->ubY,
		HUD_ITEM_SIZE, HUD_ITEM_SIZE, MINTERM_COOKIE
	);

	if(isSelected) {
		blitCopyMask(
			s_pCursor, 0, 0, s_pBufferHud->pBack,
			s_pPortraitOffsetsX[ubVikingIdx] + pItemSlotOffs->ubX,
			HUD_PORTRAIT_OFFSET_Y + pItemSlotOffs->ubY,
			HUD_ITEM_SIZE, HUD_ITEM_SIZE / 2, s_pCursorMask->Planes[0]
		);
	}
}

static void hudDrawPortrait(UBYTE ubIdx) {
	tHudIcon eIcon = HUD_ICON_ERIK_ACTIVE;
	tEntity *pEntity = (tEntity*)playerControllerGetVikingByIndex(ubIdx);
	UBYTE ubSelectedSlot = HUD_SELECTED_SLOT_NONE;
	if(pEntity) {
		const tEntityVikingData *pVikingData = (tEntityVikingData *)pEntity->pData;
		if(pVikingData->eState == VIKING_STATE_ALIVE) {
			if(
				pEntity != playerControllerGetVikingByPlayer(PLAYER_1) &&
				(!playerIsActive(PLAYER_2) || ubIdx != playerControllerGetVikingIndexByPlayer(PLAYER_2))
			) {
				// Inactive portrait
				eIcon += HUD_ICON_ERIK_INACTIVE;
			}
			ubSelectedSlot = pVikingData->ubSelectedSlot;
		}
		else if(pVikingData->eState == VIKING_STATE_DEAD) {
			eIcon += HUD_ICON_ERIK_DEAD;
		}
		else if(pVikingData->eState == VIKING_STATE_LOCKED) {
			eIcon = HUD_ICON_LOCKED;
		}
	}
	else {
		eIcon = HUD_ICON_LOCKED;
	}
	blitCopy(
		s_pPortraits, 0, eIcon * HUD_PORTRAIT_HEIGHT, s_pBufferHud->pBack, s_pPortraitOffsetsX[ubIdx], HUD_PORTRAIT_OFFSET_Y,
		HUD_PORTRAIT_WIDTH, HUD_PORTRAIT_HEIGHT, MINTERM_COOKIE
	);

	for(UBYTE i = 0; i < 4; ++i) {
		hudDrawItemSlot(ubIdx, i, i == ubSelectedSlot);
	}
}

// TODO: 4bpp on hud, 6bpp on playfield
void hudCreate(tView *pView) {
	UWORD pPaletteMain[32];
	UWORD pPaletteHud[16];
	paletteLoad("data/aminer.plt", pPaletteMain, 32);
	paletteLoad("data/vikings.plt", pPaletteMain, 24);
	paletteLoad("data/hud.plt", pPaletteMain, 16);

	s_pVpHud = vPortCreate(0,
		TAG_VPORT_BPP, HUD_BPP,
		TAG_VPORT_HEIGHT, 48,
		TAG_VPORT_PALETTE_PTR, pPaletteMain,
		TAG_VPORT_PALETTE_SIZE, 32,
		TAG_VPORT_VIEW, pView,
	TAG_END);

	s_pBufferHud = simpleBufferCreate(0,
		TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR | BMF_INTERLEAVED,
		TAG_SIMPLEBUFFER_IS_DBLBUF, 0,
		TAG_SIMPLEBUFFER_USE_X_SCROLLING, 0,
		TAG_SIMPLEBUFFER_COPLIST_OFFSET, 0,
		TAG_SIMPLEBUFFER_VPORT, s_pVpHud,
	TAG_END);

	tBitMap *pBorderTiles = bitmapCreateFromFile("data/hud_border.bm", 0);
	for(UBYTE ubY = 0; ubY < HUD_TILE_HEIGHT; ++ubY) {
		for(UBYTE ubX = 0; ubX < HUD_TILE_WIDTH; ++ubX) {
			UBYTE ubTileIndex = s_pHudTilemap[ubY][ubX];
			UBYTE ubTileOffsX = (ubTileIndex & 1) * HUD_TILE_SIZE;
			UBYTE ubTileOffsY = (ubTileIndex / 2) * HUD_TILE_SIZE;
			blitCopy(
				pBorderTiles, ubTileOffsX, ubTileOffsY, s_pBufferHud->pBack,
				ubX * HUD_TILE_SIZE, ubY * HUD_TILE_SIZE,
				HUD_TILE_SIZE, HUD_TILE_SIZE, MINTERM_COOKIE
			);
		}
	}
	bitmapDestroy(pBorderTiles);

	s_pPortraits = bitmapCreateFromFile("data/hud_portraits.bm", 0);
	s_pItems = bitmapCreateFromFile("data/hud_items.bm", 0);
	s_pCursor = bitmapCreateFromFile("data/hud_cursor.bm", 0);
	s_pCursorMask = bitmapCreateFromFile("data/hud_cursor_mask.bm", 0);
}

void hudDestroy(void) {
	// vp and simplebuffer are destroyed by viewDestroy() so skip it

	bitmapDestroy(s_pPortraits);
	bitmapDestroy(s_pItems);
}

void hudReset(void) {
	// tEntity *pEntity1 = (tEntity*)playerControllerGetVikingByIndex(0);
	// tEntityVikingData *pVikingData1 = (tEntityVikingData *)pEntity1->pData;
	// pVikingData1->pInventory[0] = ITEM_KIND_BANANA;
	// pVikingData1->pInventory[3] = ITEM_KIND_MEATLOAF;

	// tEntity *pEntity2 = (tEntity*)playerControllerGetVikingByIndex(1);
	// tEntityVikingData *pVikingData2 = (tEntityVikingData *)pEntity2->pData;
	// pVikingData2->pInventory[0] = ITEM_KIND_BOMB;
	// pVikingData2->pInventory[1] = ITEM_KIND_GARLIC;

	for(UBYTE i = 0; i < VIKING_ENTITY_MAX; ++i) {
		hudDrawPortrait(i);
	}
}

tEntity *hudProcessPlay(tPlayerIdx ePlayerIdx, tSteer *pSteer) {
	UBYTE ubOldIndex = playerControllerGetVikingIndexByPlayer(ePlayerIdx);
	BYTE bNewIndex = ubOldIndex;
	tEntity *pNewViking = playerControllerGetVikingByIndex(bNewIndex);

	// TODO: move somewhere else?
	if(steerUse(pSteer, STEER_ACTION_PREV_VIKING)) {
		do {
			if(--bNewIndex < 0) {
				bNewIndex = VIKING_ENTITY_MAX - 1;
			}
			pNewViking = playerControllerGetVikingByIndex(bNewIndex);
		} while(
			pNewViking == 0 ||
			(playerIsActive(!ePlayerIdx) && pNewViking == playerControllerGetVikingByPlayer(!ePlayerIdx)) ||
			((tEntityVikingData*)pNewViking->pData)->eState != VIKING_STATE_ALIVE
		);
	}

	if(steerUse(pSteer, STEER_ACTION_NEXT_VIKING)) {
		do {
			if(++bNewIndex >= VIKING_ENTITY_MAX) {
				bNewIndex = 0;
			}
			pNewViking = playerControllerGetVikingByIndex(bNewIndex);
		} while(
			pNewViking == 0 ||
			(playerIsActive(!ePlayerIdx) && pNewViking == playerControllerGetVikingByPlayer(!ePlayerIdx)) ||
			((tEntityVikingData*)pNewViking->pData)->eState != VIKING_STATE_ALIVE
		);
	}

	if(ubOldIndex != bNewIndex) {
		playerControllerSetControlledVikingIndex(ePlayerIdx, bNewIndex);
		hudDrawPortrait(ubOldIndex);
		hudDrawPortrait(bNewIndex);
	}

	return pNewViking;
}

void hudProcessInventory(tPlayerIdx ePlayerIdx, tSteer *pSteer) {
	tEntity *pViking = playerControllerGetVikingByPlayer(ePlayerIdx);
	tEntityVikingData *pVikingData = (tEntityVikingData *)pViking->pData;

	if(0) { // s_isHoldingItem

	}
	else {
		UBYTE ubPrevActiveSlot = pVikingData->ubSelectedSlot;
		if(steerUse(pSteer, STEER_ACTION_RIGHT)) {
			if(!(pVikingData->ubSelectedSlot & 1)) {
				++pVikingData->ubSelectedSlot;
			}
		}
		else if(steerUse(pSteer, STEER_ACTION_LEFT)) {
			if((pVikingData->ubSelectedSlot & 1)) {
				--pVikingData->ubSelectedSlot;
			}
		}
		else if(steerUse(pSteer, STEER_ACTION_DOWN)) {
			if(!(pVikingData->ubSelectedSlot & 2)) {
				pVikingData->ubSelectedSlot += 2;
			}
		}
		else if(steerUse(pSteer, STEER_ACTION_UP)) {
			if((pVikingData->ubSelectedSlot & 2)) {
				pVikingData->ubSelectedSlot -= 2;
			}
		}

		if(ubPrevActiveSlot != pVikingData->ubSelectedSlot) {
			UBYTE ubVikingIndex = playerControllerGetVikingIndexByPlayer(ePlayerIdx);
			hudDrawItemSlot(ubVikingIndex, ubPrevActiveSlot, 0);
			hudDrawItemSlot(ubVikingIndex, pVikingData->ubSelectedSlot, 1);
		}
	}
}
