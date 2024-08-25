/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "steer.h"
#include <ace/managers/key.h>

static tSteer s_sNullSteer = {
	.eKind = STEER_KIND_NULL,
	.pActions = {STEER_STATE_INACTIVE}
};

static inline void steerUpdateActionState(
	tSteerState *pSteerState, UBYTE isInputActive
) {
	if(!isInputActive) {
		*pSteerState = STEER_STATE_INACTIVE;
	}
	else if(*pSteerState == STEER_STATE_INACTIVE) {
		*pSteerState = STEER_STATE_ACTIVE;
	}
}

void steerReset(tSteer *pSteer, tSteerKind eKind)
{
	pSteer->eKind = eKind;
	for(UBYTE i = 0; i < STEER_ACTION_COUNT; ++i) {
		pSteer->pActions[i] = STEER_STATE_INACTIVE;
	}
}

void steerUpdate(tSteer *pSteer) {
	switch(pSteer->eKind) {
		case STEER_KIND_PRESET1:
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_UP], keyCheck(KEY_UP));
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_DOWN], keyCheck(KEY_DOWN));
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_LEFT], keyCheck(KEY_LEFT));
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_RIGHT], keyCheck(KEY_RIGHT));
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_ABILITY_1], keyCheck(KEY_Z));
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_ABILITY_2], keyCheck(KEY_X));
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_INTERACT], keyCheck(KEY_A));
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_USE_ITEM], keyCheck(KEY_S));
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_PAUSE], keyCheck(KEY_ESCAPE));
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_INVENTORY], keyCheck(KEY_SPACE) | keyCheck(KEY_RETURN));
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_PREV_VIKING], keyCheck(KEY_Q));
			steerUpdateActionState(&pSteer->pActions[STEER_ACTION_NEXT_VIKING], keyCheck(KEY_W));
			break;
		case STEER_KIND_PRESET2:
			break;
		case STEER_KIND_NULL:
			break;
	}
}

UBYTE steerCheck(tSteer *pSteer, tSteerAction eAction)
{
	return pSteer->pActions[eAction] != STEER_STATE_INACTIVE;
}

UBYTE steerUse(tSteer *pSteer, tSteerAction eAction)
{
	if(pSteer->pActions[eAction] == STEER_STATE_ACTIVE) {
		pSteer->pActions[eAction] = STEER_STATE_USED;
		return 1;
	}
	return 0;
}

tSteer *steerGetNull(void)
{
	return &s_sNullSteer;
}
