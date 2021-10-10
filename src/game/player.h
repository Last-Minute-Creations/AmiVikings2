/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <ace/types.h>
#include <ace/macros.h>

typedef enum tSteerRequest {
	STEER_UP = BV(0),
	STEER_DOWN = BV(1),
	STEER_LEFT = BV(2),
	STEER_RIGHT = BV(3),
	STEER_A = BV(4),
	STEER_B = BV(5),
	STEER_X = BV(6),
	STEER_Y = BV(7),
	STEER_L = BV(8),
	STEER_R = BV(9),
} tSteerRequest;

tSteerRequest playerProcessSteer(UBYTE ubPlayerIdx);

#endif // _PLAYER_H_
