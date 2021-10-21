/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _HUD_H_
#define _HUD_H_

#include <ace/types.h>
#include <ace/utils/extview.h>
#include "entity.h"

#define HUD_BPP 5

typedef enum tPlayerIdx {
	PLAYER_1,
	PLAYER_2,
	PLAYER_COUNT,
	PLAYER_NONE = PLAYER_COUNT,
} tPlayerIdx;

void hudCreate(tView *pView);

void hudDestroy(void);

void hudReset(tEntity **pEntities);

tEntity *hudProcessPlayerSteer(UBYTE ubPlayerIdx, tSteerRequest eReq);

#endif // _HUD_H_
