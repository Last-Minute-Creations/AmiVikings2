/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hud.h"
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/utils/palette.h>
#include <ace/utils/string.h>
#include "entity.h"
#include "entity_erik.h"

static tVPort *s_pVpHud;
static tSimpleBufferManager *s_pBufferHud;
static tBitMap *s_pPortaits[VIKING_ENTITY_MAX] = {0};
static tBitMap *s_pPortraitLocked;

static void hudDrawPortrait(UBYTE ubIdx) {
	static const UBYTE pOffsets[VIKING_ENTITY_MAX] = {16, 88, 168};
	UBYTE ubOffsY = 0;
	tEntityErik *pEntity = (tEntityErik*)playerControllerGetVikingByIndex(ubIdx);
	tBitMap *pPortrait = s_pPortaits[ubIdx];
	if(pEntity) {
		if(pEntity->eState == VIKING_STATE_ALIVE) {
			if(
				pEntity != playerControllerGetVikingByPlayer(PLAYER_1) &&
				(!playerIsActive(PLAYER_2) || ubIdx != playerControllerGetVikingIndexByPlayer(PLAYER_2))
			) {
				// Inactive portrait
				ubOffsY = 24;
			}
		}
		else if(pEntity->eState == VIKING_STATE_DEAD) {
			ubOffsY = 48;
		}
		else if(pEntity->eState == VIKING_STATE_LOCKED) {
			pPortrait = s_pPortraitLocked;
		}
	}
	else {
		pPortrait = s_pPortraitLocked;
	}
	blitCopy(
		pPortrait, 0, ubOffsY, s_pBufferHud->pBack, pOffsets[ubIdx], 16,
		32, 24, MINTERM_COOKIE
	);
}

void hudCreate(tView *pView) {
	UWORD pPaletteHud[32];
	paletteLoad("data/aminer.plt", pPaletteHud, 32);
	paletteLoad("data/vikings.plt", pPaletteHud, 24);

	s_pVpHud = vPortCreate(0,
		TAG_VPORT_BPP, HUD_BPP,
		TAG_VPORT_HEIGHT, 48,
		TAG_VPORT_PALETTE_PTR, pPaletteHud,
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

	s_pPortraitLocked = bitmapCreateFromFile("data/hud/unk.bm", 0);
}

void hudDestroy(void) {
	// vp and simplebuffer are destroyed by viewDestroy() so skip it

	bitmapDestroy(s_pPortraitLocked);
}

void hudReset(void) {
	for(UBYTE i = 0; i < VIKING_ENTITY_MAX; ++i) {
		if(s_pPortaits[i] != 0) {
			bitmapDestroy(s_pPortaits[i]);
			s_pPortaits[i] = 0;
		}
		s_pPortaits[i] = 0;
		tEntity *pVikingEntity = playerControllerGetVikingByIndex(i);
		if(pVikingEntity) {
			char szPath[25];
			char *pEnd = stringCopy("data/hud/", szPath);
			if(pVikingEntity->eType == ENTITY_KIND_ERIK) {
				pEnd = stringCopy("erik", pEnd);
			}
			else if(pVikingEntity->eType == ENTITY_KIND_BAELOG) {
				pEnd = stringCopy("baelog", pEnd);
			}
			else if(pVikingEntity->eType == ENTITY_KIND_OLAF) {
				pEnd = stringCopy("olaf", pEnd);
			}
			pEnd = stringCopy(".bm", pEnd);
			s_pPortaits[i] = bitmapCreateFromFile(szPath, 0);
		}
	}

	for(UBYTE i = 0; i < VIKING_ENTITY_MAX; ++i) {
		hudDrawPortrait(i);
	}
}

tEntity *hudProcessPlayerSteer(tPlayerIdx ePlayerIdx, tSteer *pSteer) {
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
			((tEntityErik *)pNewViking)->eState != VIKING_STATE_ALIVE
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
			((tEntityErik *)pNewViking)->eState != VIKING_STATE_ALIVE
		);
	}

	if(ubOldIndex != bNewIndex) {
		playerControllerSetControlledVikingIndex(ePlayerIdx, bNewIndex);
		hudDrawPortrait(ubOldIndex);
		hudDrawPortrait(bNewIndex);
	}

	return pNewViking;
}
