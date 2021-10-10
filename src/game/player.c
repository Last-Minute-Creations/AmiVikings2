/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "player.h"
#include <ace/managers/key.h>

tSteerRequest playerProcessSteer(UBYTE ubPlayerIdx) {
	tSteerRequest eReq = 0;

	// TODO: read the preset for given player

	if(keyCheck(KEY_UP)) {
		eReq |= STEER_UP;
	}
	if(keyCheck(KEY_DOWN)) {
		eReq |= STEER_DOWN;
	}
	if(keyCheck(KEY_LEFT)) {
		eReq |= STEER_LEFT;
	}
	if(keyCheck(KEY_RIGHT)) {
		eReq |= STEER_RIGHT;
	}
	if(keyCheck(KEY_Z)) {
		eReq |= STEER_A;
	}
	if(keyCheck(KEY_X)) {
		eReq |= STEER_B;
	}
	if(keyCheck(KEY_A)) {
		eReq |= STEER_X;
	}
	if(keyCheck(KEY_S)) {
		eReq |= STEER_Y;
	}
	if(keyCheck(KEY_Q)) {
		eReq |= STEER_L;
	}
	if(keyCheck(KEY_W)) {
		eReq |= STEER_R;
	}

	return eReq;
}
