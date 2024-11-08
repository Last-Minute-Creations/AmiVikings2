/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_GAME_STATE_GAME_HPP
#define INC_GAME_STATE_GAME_HPP

#include <ace/managers/state.h>
#include <ace/managers/bob.h>

extern tState g_sStateGame;

void gameSetPendingDialogue(UWORD uwDialogueId, UWORD uwBgColor);

void gameTryPushBob(tBob *pBob);

#endif // INC_GAME_STATE_GAME_HPP
