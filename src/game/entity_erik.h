/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _ENTITY_ERIK_H_
#define _ENTITY_ERIK_H_

#include "entity.h"

typedef enum tVikingState {
	VIKING_STATE_LOCKED,
	VIKING_STATE_ALIVE,
	VIKING_STATE_DEAD,
} tVikingState;

typedef enum tMoveState {
	MOVE_STATE_WALKING,
	MOVE_STATE_FALLING,
	MOVE_STATE_CLIMBING,
} tMoveState;

typedef struct tEntityErik {
	tEntity sBase;
	tSteerRequest eSteer;
	UBYTE ubJoy;
	tVikingState eState;
	tMoveState eMoveState;
	tUwCoordYX sPos;
} tEntityErik;

tEntityErik *entityErikCreate(UWORD uwPosX, UWORD uwPosY);

void entityErikProcess(tEntityErik *pEntity);

void entityErikDestroy(tEntityErik *pEntity);

#endif // _ENTITY_ERIK_H_
