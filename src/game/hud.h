/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _HUD_H_
#define _HUD_H_

#include <ace/types.h>
#include <ace/utils/extview.h>
#include "entity.h"
#include "player_controller.h"

#define HUD_BPP 5

void hudCreate(tView *pView);

void hudDestroy(void);

void hudReset(void);

tEntity *hudProcessPlayerSteer(tPlayerIdx ePlayerIdx, tSteer *pSteer);

#endif // _HUD_H_
