/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "steer.hpp"
#include <ace/managers/key.h>
#include <lmc/enum_value.hpp>

using namespace Lmc;

static tSteer s_sNullSteer = {
	.eKind = tSteerKind::Null,
	.pActions = {tSteerState::Inactive}
};

static inline void steerUpdateActionState(
	tSteerState *pSteerState, UBYTE isInputActive
) {
	if(!isInputActive) {
		*pSteerState = tSteerState::Inactive;
	}
	else if(*pSteerState == tSteerState::Inactive) {
		*pSteerState = tSteerState::Active;
	}
}

void steerReset(tSteer *pSteer, tSteerKind eKind)
{
	pSteer->eKind = eKind;
	for(UBYTE i = 0; i < enumValue(tSteerAction::Count); ++i) {
		pSteer->pActions[i] = tSteerState::Inactive;
	}
}

void steerUpdate(tSteer *pSteer) {
	switch(pSteer->eKind) {
		case tSteerKind::Preset1:
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::Up)], keyCheck(KEY_UP));
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::Down)], keyCheck(KEY_DOWN));
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::Left)], keyCheck(KEY_LEFT));
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::Right)], keyCheck(KEY_RIGHT));
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::Ability1)], keyCheck(KEY_Z));
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::Ability2)], keyCheck(KEY_X));
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::Interact)], keyCheck(KEY_A));
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::UseItem)], keyCheck(KEY_S));
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::Pause)], keyCheck(KEY_ESCAPE));
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::Inventory)], keyCheck(KEY_SPACE) | keyCheck(KEY_RETURN));
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::PrevViking)], keyCheck(KEY_Q));
			steerUpdateActionState(&pSteer->pActions[enumValue(tSteerAction::NextViking)], keyCheck(KEY_W));
			break;
		case tSteerKind::Preset2:
			break;
		case tSteerKind::Null:
			break;
	}
}

UBYTE steerCheck(tSteer *pSteer, tSteerAction eAction)
{
	return pSteer->pActions[enumValue(eAction)] != tSteerState::Inactive;
}

UBYTE steerUse(tSteer *pSteer, tSteerAction eAction)
{
	if(pSteer->pActions[enumValue(eAction)] == tSteerState::Active) {
		pSteer->pActions[enumValue(eAction)] = tSteerState::Used;
		return 1;
	}
	return 0;
}

tSteer *steerGetNull(void)
{
	return &s_sNullSteer;
}
