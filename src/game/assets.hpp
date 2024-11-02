/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_GAME_ASSETS_HPP
#define INC_GAME_ASSETS_HPP

#include <ace/utils/bitmap.h>

void assetsGlobalCreate(void);

void assetsGlobalDestroy(void);

// Global assets
extern tBitMap *g_pBobBmErikLeft, *g_pBobBmErikLeftMask, *g_pBobBmErikRight, *g_pBobBmErikRightMask;
extern tBitMap *g_pBobBmOlafLeft, *g_pBobBmOlafLeftMask, *g_pBobBmOlafRight, *g_pBobBmOlafRightMask;
extern tBitMap *g_pBobBmBaelogLeft, *g_pBobBmBaelogLeftMask, *g_pBobBmBaelogRight, *g_pBobBmBaelogRightMask;
extern tBitMap *g_pBobBmHelpBox, *g_pBobBmMaskFull16;

#endif // INC_GAME_ASSETS_HPP
