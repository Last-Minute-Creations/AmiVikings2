/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hud.hpp"
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/utils/palette.h>
#include <ace/utils/string.h>
#include <lmc/enum_value.hpp>
#include <entity/entity_viking.hpp>

using namespace Lmc;

#define HUD_PORTRAIT_WIDTH 32
#define HUD_PORTRAIT_HEIGHT 24
#define HUD_TILE_WIDTH 32
#define HUD_TILE_HEIGHT 6
#define HUD_TILE_SIZE 8
#define HUD_ITEM_SIZE 16
#define HUD_PORTRAIT_1_OFFSET_X 16
#define HUD_PORTRAIT_2_OFFSET_X 88
#define HUD_PORTRAIT_3_OFFSET_X 160
#define HUD_PORTRAIT_OFFSET_Y 16
#define HUD_SELECTED_SLOT_NONE 255
#define HUD_ITEM_OWNER_TRASH 3
#define HUD_ITEM_BLINK_COOLDOWN 25

enum class tHudIcon: UBYTE {
	ErikActive,
	BaelogActive,
	OlafActive,
	FangActive,
	ScorchActive,

	ErikInactive,
	BaelogInactive,
	OlafInactive,
	FangInactive,
	ScorchInactive,

	ErikDead,
	BaelogDead,
	OlafDead,
	FangDead,
	ScorchDead,

	Locked,
};

static tVPort *s_pVpHud;
static tSimpleBufferManager *s_pBufferHud;
static tBitMap *s_pPortraits;
static tBitMap *s_pItems;
static tBitMap *s_pCursor;
static tBitMap *s_pCursorMask;
static tItemKind s_eHeldItem;
static UBYTE s_ubHeldItemOwner; // 0..2 are vikings, 3 is trashcan
static UBYTE s_ubHeldItemSlot;
static UBYTE s_ubBlinkCooldown;
static UBYTE s_ubIsBlinkShow;

static const UBYTE s_pPortraitOffsetsX[VIKING_ENTITY_MAX] = {
	HUD_PORTRAIT_1_OFFSET_X,
	HUD_PORTRAIT_2_OFFSET_X,
	HUD_PORTRAIT_3_OFFSET_X,
};

static const tUbCoordYX s_pItemOffsets[4][4] = {
		{
			tUbCoordYX()
				.withX(HUD_PORTRAIT_1_OFFSET_X + HUD_PORTRAIT_WIDTH)
				.withY(HUD_PORTRAIT_OFFSET_Y + 0),
			tUbCoordYX()
				.withX(HUD_PORTRAIT_1_OFFSET_X + HUD_PORTRAIT_WIDTH + HUD_ITEM_SIZE)
				.withY(HUD_PORTRAIT_OFFSET_Y + 0),
			tUbCoordYX()
				.withX(HUD_PORTRAIT_1_OFFSET_X + HUD_PORTRAIT_WIDTH)
				.withY(HUD_PORTRAIT_OFFSET_Y + HUD_ITEM_SIZE),
			tUbCoordYX()
				.withX(HUD_PORTRAIT_1_OFFSET_X + HUD_PORTRAIT_WIDTH + HUD_ITEM_SIZE)
				.withY(HUD_PORTRAIT_OFFSET_Y + HUD_ITEM_SIZE),
		},
		{
			tUbCoordYX()
				.withX(HUD_PORTRAIT_2_OFFSET_X + HUD_PORTRAIT_WIDTH)
				.withY(HUD_PORTRAIT_OFFSET_Y + 0),
			tUbCoordYX()
				.withX(HUD_PORTRAIT_2_OFFSET_X + HUD_PORTRAIT_WIDTH + HUD_ITEM_SIZE)
				.withY(HUD_PORTRAIT_OFFSET_Y + 0),
			tUbCoordYX()
				.withX(HUD_PORTRAIT_2_OFFSET_X + HUD_PORTRAIT_WIDTH)
				.withY(HUD_PORTRAIT_OFFSET_Y + HUD_ITEM_SIZE),
			tUbCoordYX()
				.withX(HUD_PORTRAIT_2_OFFSET_X + HUD_PORTRAIT_WIDTH + HUD_ITEM_SIZE)
				.withY(HUD_PORTRAIT_OFFSET_Y + HUD_ITEM_SIZE),
		},
		{
			tUbCoordYX()
				.withX(HUD_PORTRAIT_3_OFFSET_X + HUD_PORTRAIT_WIDTH)
				.withY(HUD_PORTRAIT_OFFSET_Y + 0),
			tUbCoordYX()
				.withX(HUD_PORTRAIT_3_OFFSET_X + HUD_PORTRAIT_WIDTH + HUD_ITEM_SIZE)
				.withY(HUD_PORTRAIT_OFFSET_Y + 0),
			tUbCoordYX()
				.withX(HUD_PORTRAIT_3_OFFSET_X + HUD_PORTRAIT_WIDTH)
				.withY(HUD_PORTRAIT_OFFSET_Y + HUD_ITEM_SIZE),
			tUbCoordYX()
				.withX(HUD_PORTRAIT_3_OFFSET_X + HUD_PORTRAIT_WIDTH + HUD_ITEM_SIZE)
				.withY(HUD_PORTRAIT_OFFSET_Y + HUD_ITEM_SIZE),
		},
		{
			tUbCoordYX()
				.withY(HUD_PORTRAIT_OFFSET_Y + 0)
				.withX(HUD_PORTRAIT_3_OFFSET_X + HUD_PORTRAIT_WIDTH + 2 * HUD_ITEM_SIZE),
		}
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

static void hudDrawItemAtPos(tItemKind eItem, UWORD uwX, UBYTE ubY, UBYTE isSelected) {
	blitCopy( // s_pItems first icon is trashcan, so start with one further
		s_pItems, 0, (1 + enumValue(eItem)) * HUD_ITEM_SIZE,
		s_pBufferHud->pBack, uwX, ubY,
		HUD_ITEM_SIZE, HUD_ITEM_SIZE, MINTERM_COOKIE
	);

	if(isSelected) {
		blitCopyMask(
			s_pCursor, 0, 0, s_pBufferHud->pBack, uwX, ubY,
			HUD_ITEM_SIZE, HUD_ITEM_SIZE / 2, s_pCursorMask->Planes[0]
		);
	}
}

static void hudDrawItemSlot(UBYTE ubVikingIdx, UBYTE ubSlotIdx) {
	tUbCoordYX sItemSlotOffs = s_pItemOffsets[ubVikingIdx][ubSlotIdx];

	tEntity *pViking = playerControllerGetVikingByIndex(ubVikingIdx);
	tItemKind eItem;
	UBYTE isSelected = 0;
	if(pViking) {
		const auto *pVikingData = pViking->dataAs<tEntityVikingData>();
		eItem = pVikingData->pInventory[ubSlotIdx];
		isSelected = pVikingData->ubSelectedSlot == ubSlotIdx;
	}
	else {
		eItem = tItemKind::None;
	}

	hudDrawItemAtPos(eItem, sItemSlotOffs.ubX, sItemSlotOffs.ubY, isSelected);
}

static void hudDrawPortrait(UBYTE ubIdx) {
	tHudIcon eIcon = tHudIcon::ErikActive;
	tEntity *pEntity = (tEntity*)playerControllerGetVikingByIndex(ubIdx);
	if(pEntity) {
		const auto *pVikingData = pEntity->dataAs<tEntityVikingData>();
		if(pVikingData->eState == tVikingState::Alive) {
			if(
				pEntity != playerControllerGetVikingByPlayer(tPlayerIdx::First) &&
				(!playerIsActive(tPlayerIdx::Second) || ubIdx != playerControllerGetVikingIndexByPlayer(tPlayerIdx::Second))
			) {
				// Inactive portrait
				eIcon = static_cast<tHudIcon>(enumValue(eIcon) + enumValue(tHudIcon::ErikInactive));
			}
		}
		else if(pVikingData->eState == tVikingState::Dead) {
			eIcon = static_cast<tHudIcon>(enumValue(eIcon) + enumValue(tHudIcon::ErikDead));
		}
		else if(pVikingData->eState == tVikingState::Locked) {
			eIcon = tHudIcon::Locked;
		}
	}
	else {
		eIcon = tHudIcon::Locked;
	}
	blitCopy(
		s_pPortraits, 0, enumValue(eIcon) * HUD_PORTRAIT_HEIGHT, s_pBufferHud->pBack, s_pPortraitOffsetsX[ubIdx], HUD_PORTRAIT_OFFSET_Y,
		HUD_PORTRAIT_WIDTH, HUD_PORTRAIT_HEIGHT, MINTERM_COOKIE
	);

	for(UBYTE i = 0; i < 4; ++i) {
		hudDrawItemSlot(ubIdx, i);
	}
}

static tEntity *hudProcessVikingCycling(tPlayerIdx ePlayerIdx, tSteer *pSteer) {
	UBYTE ubOldIndex = playerControllerGetVikingIndexByPlayer(ePlayerIdx);
	BYTE bNewIndex = ubOldIndex;
	tEntity *pNewViking = playerControllerGetVikingByIndex(bNewIndex);

	// TODO: move somewhere else?
	if(steerUse(pSteer, tSteerAction::PrevViking)) {
		do {
			if(--bNewIndex < 0) {
				bNewIndex = VIKING_ENTITY_MAX - 1;
			}
			pNewViking = playerControllerGetVikingByIndex(bNewIndex);
		} while(
			pNewViking == 0 ||
			(playerIsActive(playerGetOther(ePlayerIdx)) && pNewViking == playerControllerGetVikingByPlayer(playerGetOther(ePlayerIdx))) ||
			(pNewViking->dataAs<tEntityVikingData>())->eState != tVikingState::Alive
		);
	}

	if(steerUse(pSteer, tSteerAction::NextViking)) {
		do {
			if(++bNewIndex >= VIKING_ENTITY_MAX) {
				bNewIndex = 0;
			}
			pNewViking = playerControllerGetVikingByIndex(bNewIndex);
		} while(
			pNewViking == 0 ||
			(playerIsActive(playerGetOther(ePlayerIdx)) && pNewViking == playerControllerGetVikingByPlayer(playerGetOther(ePlayerIdx))) ||
			(pNewViking->dataAs<tEntityVikingData>())->eState != tVikingState::Alive
		);
	}

	if(ubOldIndex != bNewIndex) {
		playerControllerSetControlledVikingIndex(ePlayerIdx, bNewIndex);
		hudDrawPortrait(ubOldIndex);
		hudDrawPortrait(bNewIndex);
	}

	return pNewViking;
}

static void dragItemToNextViking(BYTE bDelta) {
	UBYTE ubPrevOwner = s_ubHeldItemOwner;
	UBYTE ubPrevSlot = s_ubHeldItemSlot;
	BYTE bNewOwner = ubPrevOwner;
	do {
		bNewOwner += bDelta;
		if(bNewOwner < 0) {
			bNewOwner = HUD_ITEM_OWNER_TRASH;
		}
		else if(bNewOwner > HUD_ITEM_OWNER_TRASH) {
			bNewOwner = 0;
		}

		if(bNewOwner == HUD_ITEM_OWNER_TRASH) {
			s_ubHeldItemOwner = bNewOwner;
			s_ubHeldItemSlot = 0;
			break;
		}

		tEntity *pNewViking = playerControllerGetVikingByIndex(bNewOwner);
		if(!pNewViking || entityVikingGetState(pNewViking) != tVikingState::Alive) {
			continue;
		}
		BYTE bSlot = entityVikingGetFreeItemSlot(pNewViking);
		if(bSlot >= 0) {
			s_ubHeldItemOwner = bNewOwner;
			s_ubHeldItemSlot = bSlot;
		}
	} while(bNewOwner != s_ubHeldItemOwner);

	if(ubPrevOwner == HUD_ITEM_OWNER_TRASH) {
		tUbCoordYX sPos = s_pItemOffsets[ubPrevOwner][0];
		hudDrawItemAtPos(tItemKind::Invalid, sPos.ubX, sPos.ubY, 0);
	}
	else {
		hudDrawItemSlot(ubPrevOwner, ubPrevSlot);
	}
	s_ubBlinkCooldown = 1;
	s_ubIsBlinkShow = 1;
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
	tEntity *pEntity1 = (tEntity*)playerControllerGetVikingByIndex(0);
	auto *pVikingData1 = pEntity1->dataAs<tEntityVikingData>();
	pVikingData1->pInventory[0] = tItemKind::Banana;
	pVikingData1->pInventory[3] = tItemKind::Meatloaf;

	tEntity *pEntity2 = (tEntity*)playerControllerGetVikingByIndex(1);
	auto *pVikingData2 = pEntity2->dataAs<tEntityVikingData>();
	pVikingData2->pInventory[0] = tItemKind::Bomb;
	pVikingData2->pInventory[1] = tItemKind::Garlic;

	s_eHeldItem = tItemKind::None;
	for(UBYTE i = 0; i < VIKING_ENTITY_MAX; ++i) {
		hudDrawPortrait(i);
	}
}

tEntity *hudProcessPlay(tPlayerIdx ePlayerIdx, tSteer *pSteer) {
	return hudProcessVikingCycling(ePlayerIdx, pSteer);
}

void hudProcessInventory(tPlayerIdx ePlayerIdx, tSteer *pSteer) {
	if(s_eHeldItem != tItemKind::None) {
		if(steerUse(pSteer, tSteerAction::Left)) {
			dragItemToNextViking(-1);
		}
		if(steerUse(pSteer, tSteerAction::Right)) {
			dragItemToNextViking(+1);
		}
		else if(steerUse(pSteer, tSteerAction::Ability1)) {
			if(s_ubHeldItemOwner == HUD_ITEM_OWNER_TRASH) {
				tUbCoordYX sPos = s_pItemOffsets[s_ubHeldItemOwner][s_ubHeldItemSlot];
				hudDrawItemAtPos(tItemKind::Invalid, sPos.ubX, sPos.ubY, 0);
			}
			else {
				tEntity *pTargetViking = playerControllerGetVikingByIndex(s_ubHeldItemOwner);
				auto *pTargetVikingData = pTargetViking->dataAs<tEntityVikingData>();
				pTargetVikingData->pInventory[s_ubHeldItemSlot] = s_eHeldItem;
				hudDrawItemSlot(s_ubHeldItemOwner, s_ubHeldItemSlot);
			}
			s_eHeldItem = tItemKind::None;
		}
		else {
			if(--s_ubBlinkCooldown == 0) {
				tEntity *pViking = playerControllerGetVikingByPlayer(ePlayerIdx);
				auto *pVikingData = pViking->dataAs<tEntityVikingData>();

				tUbCoordYX sPos = s_pItemOffsets[s_ubHeldItemOwner][s_ubHeldItemSlot];
				hudDrawItemAtPos(
					s_ubIsBlinkShow ? s_eHeldItem : tItemKind::None,
					sPos.ubX, sPos.ubY, pVikingData->ubSelectedSlot == s_ubHeldItemSlot
				);
				s_ubBlinkCooldown = HUD_ITEM_BLINK_COOLDOWN;
				s_ubIsBlinkShow = !s_ubIsBlinkShow;
			}
		}
	}
	else {
		tEntity *pViking = hudProcessVikingCycling(ePlayerIdx, pSteer);
		auto *pVikingData = pViking->dataAs<tEntityVikingData>();
		UBYTE ubVikingIndex = playerControllerGetVikingIndexByPlayer(ePlayerIdx);

		UBYTE ubPrevActiveSlot = pVikingData->ubSelectedSlot;
		if(steerUse(pSteer, tSteerAction::Right)) {
			if(!(pVikingData->ubSelectedSlot & 1)) {
				++pVikingData->ubSelectedSlot;
			}
		}
		else if(steerUse(pSteer, tSteerAction::Left)) {
			if((pVikingData->ubSelectedSlot & 1)) {
				--pVikingData->ubSelectedSlot;
			}
		}
		else if(steerUse(pSteer, tSteerAction::Down)) {
			if(!(pVikingData->ubSelectedSlot & 2)) {
				pVikingData->ubSelectedSlot += 2;
			}
		}
		else if(steerUse(pSteer, tSteerAction::Up)) {
			if((pVikingData->ubSelectedSlot & 2)) {
				pVikingData->ubSelectedSlot -= 2;
			}
		}

		if(ubPrevActiveSlot != pVikingData->ubSelectedSlot) {
			hudDrawItemSlot(ubVikingIndex, ubPrevActiveSlot);
			hudDrawItemSlot(ubVikingIndex, pVikingData->ubSelectedSlot);
		}

		if(steerUse(pSteer, tSteerAction::Ability1)) {
			tItemKind eSelectedItem = pVikingData->pInventory[pVikingData->ubSelectedSlot];
			if(eSelectedItem != tItemKind::None) {
				s_eHeldItem = eSelectedItem;
				s_ubHeldItemOwner = ubVikingIndex;
				s_ubHeldItemSlot = pVikingData->ubSelectedSlot;
				pVikingData->pInventory[pVikingData->ubSelectedSlot] = tItemKind::None;
				hudDrawItemSlot(ubVikingIndex, pVikingData->ubSelectedSlot);
				s_ubBlinkCooldown = HUD_ITEM_BLINK_COOLDOWN;
				s_ubIsBlinkShow = 1;
			}
		}
	}
}
