/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ace/generic/main.h>
#include <ace/managers/state.h>
#include <ace/managers/key.h>
#include <ace/managers/joy.h>
#include "state_game.hpp"

tStateManager *s_pGameStateManager;

void genericCreate(void) {
	keyCreate();
	joyOpen();
	s_pGameStateManager = stateManagerCreate();
	stateChange(s_pGameStateManager, &g_sStateGame);
}

void genericProcess(void) {
	keyProcess();
	joyProcess();
	stateProcess(s_pGameStateManager);
}

void genericDestroy(void) {
	joyClose();
	keyDestroy();
	stateManagerDestroy(s_pGameStateManager);
}
