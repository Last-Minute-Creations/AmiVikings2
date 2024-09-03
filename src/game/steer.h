/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _STEER_H_
#define _STEER_H_

#include <ace/types.h>
#include <ace/macros.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum tSteerKind {
	STEER_KIND_NULL,
	STEER_KIND_PRESET1,
	STEER_KIND_PRESET2,
	// TODO: playback from array
} tSteerKind;

typedef enum tSteerAction {
	STEER_ACTION_UP,
	STEER_ACTION_DOWN,
	STEER_ACTION_LEFT,
	STEER_ACTION_RIGHT,
	STEER_ACTION_ABILITY_1,
	STEER_ACTION_ABILITY_2,
	STEER_ACTION_USE_ITEM,
	STEER_ACTION_INTERACT,
	STEER_ACTION_PAUSE,
	STEER_ACTION_INVENTORY,
	STEER_ACTION_PREV_VIKING,
	STEER_ACTION_NEXT_VIKING,
	STEER_ACTION_COUNT
} tSteerAction;

typedef enum tSteerState {
	STEER_STATE_INACTIVE,
	STEER_STATE_ACTIVE,
	STEER_STATE_USED,
} tSteerState;

typedef struct tSteer {
	tSteerKind eKind;
	tSteerState pActions[STEER_ACTION_COUNT];
} tSteer;

void steerReset(tSteer *pSteer, tSteerKind eKind);

void steerUpdate(tSteer *pSteer);

UBYTE steerCheck(tSteer *pSteer, tSteerAction eAction);

UBYTE steerUse(tSteer *pSteer, tSteerAction eAction);

tSteer *steerGetNull(void);

#ifdef __cplusplus
}
#endif

#endif // _STEER_H_
