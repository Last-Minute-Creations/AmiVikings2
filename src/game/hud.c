/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hud.h"
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/utils/palette.h>
#include <ace/utils/string.h>
#include "entity.h"
#include "entity_erik.h"

#define HUD_CHARACTER_SLOT_COUNT 3
#define HUD_BPP 5

static tVPort *s_pVpHud;
static tSimpleBufferManager *s_pBufferHud;
static tBitMap *s_pPortaits[HUD_CHARACTER_SLOT_COUNT] = {0};
static tEntity *s_pCharacters[HUD_CHARACTER_SLOT_COUNT];
static UBYTE s_pCharSelectedByPlayer[PLAYER_COUNT];
static tBitMap *s_pPortraitLocked;

static void hudDrawPortrait(UBYTE ubIdx) {
	static const UBYTE pOffsets[HUD_CHARACTER_SLOT_COUNT] = {16, 88, 168};
	UBYTE ubOffsY = 0;
	tEntityErik *pEntity = (tEntityErik*)s_pCharacters[ubIdx];
	tBitMap *pPortrait = s_pPortaits[ubIdx];
	if(pEntity) {
		if(pEntity->eState == VIKING_STATE_ALIVE) {
			if(
				ubIdx != s_pCharSelectedByPlayer[0] &&
				ubIdx != s_pCharSelectedByPlayer[1]
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
		TAG_SIMPLEBUFFER_VPORT, s_pVpHud,
	TAG_END);

	s_pPortraitLocked = bitmapCreateFromFile("data/hud/unk.bm", 0);
}

void hudDestroy(void) {
	// vp and simplebuffer are destroyed by viewDestroy() so skip it

	bitmapDestroy(s_pPortraitLocked);
}

void hudReset(tEntity **pEntities) {
	for(UBYTE i = 0; i < HUD_CHARACTER_SLOT_COUNT; ++i) {
		if(s_pPortaits[i] != 0) {
			bitmapDestroy(s_pPortaits[i]);
			s_pPortaits[i] = 0;
		}
		s_pCharacters[i] = pEntities[i];
		s_pPortaits[i] = 0;
		if(pEntities[i]) {
			char szPath[25];
			char *pEnd = stringCopy("data/hud/", szPath);
			if(pEntities[i]->eType == ENTITY_TYPE_ERIK) {
				pEnd = stringCopy("erik", pEnd);
			}
			else if(pEntities[i]->eType == ENTITY_TYPE_BAELOG) {
				pEnd = stringCopy("baelog", pEnd);
			}
			else if(pEntities[i]->eType == ENTITY_TYPE_OLAF) {
				pEnd = stringCopy("olaf", pEnd);
			}
			pEnd = stringCopy(".bm", pEnd);
			s_pPortaits[i] = bitmapCreateFromFile(szPath, 0);
		}
	}

	// TODO: handle gap when middle char is inactive
	s_pCharSelectedByPlayer[PLAYER_1] = 0;
	s_pCharSelectedByPlayer[PLAYER_2] = 1;

	for(UBYTE i = 0; i < HUD_CHARACTER_SLOT_COUNT; ++i) {
		hudDrawPortrait(i);
	}
}

tEntity *hudProcessPlayerSteer(UBYTE ubPlayerIdx, tSteerRequest eReq) {
	UBYTE ubOld = s_pCharSelectedByPlayer[ubPlayerIdx];
	// TODO: process L/R
	UBYTE ubNew = s_pCharSelectedByPlayer[ubPlayerIdx];
	if(ubOld != ubNew) {
		hudDrawPortrait(ubOld);
		hudDrawPortrait(ubNew);
	}
	return s_pCharacters[s_pCharSelectedByPlayer[ubPlayerIdx]];
}
