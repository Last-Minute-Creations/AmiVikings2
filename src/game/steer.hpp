/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_GAME_STEER_HPP
#define INC_GAME_STEER_HPP

#include <ace/types.h>
#include <ace/macros.h>
#include <lmc/enum_value.hpp>

#ifdef __cplusplus
extern "C" {
#endif

enum class tSteerKind: UBYTE {
	Null,
	Preset1,
	Preset2,
	// TODO: playback from array
};

enum class tSteerAction: UBYTE {
	Up,
	Down,
	Left,
	Right,
	Ability1,
	Ability2,
	UseItem,
	Interact,
	Pause,
	Inventory,
	PrevViking,
	NextViking,
	Count
};

enum class tSteerState: UBYTE {
	Inactive,
	Active,
	Used,
};

typedef struct tSteer {
	tSteerKind eKind;
	tSteerState pActions[Lmc::enumValue(tSteerAction::Count)];
} tSteer;

void steerReset(tSteer *pSteer, tSteerKind eKind);

void steerUpdate(tSteer *pSteer);

UBYTE steerCheck(tSteer *pSteer, tSteerAction eAction);

UBYTE steerUse(tSteer *pSteer, tSteerAction eAction);

tSteer *steerGetNull(void);

#ifdef __cplusplus
}
#endif

#endif // INC_GAME_STEER_HPP
