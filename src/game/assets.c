/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "assets.h"

void assetsGlobalCreate(void) {
	g_pBobBmErik = bitmapCreateFromFile("data/erik.bm", 0);
	g_pBobBmErikMask = bitmapCreateFromFile("data/erik_mask.bm", 0);
}

void assetsGlobalDestroy(void) {
	bitmapDestroy(g_pBobBmErik);
	bitmapDestroy(g_pBobBmErikMask);
}

// Global assets
tBitMap *g_pBobBmErik, *g_pBobBmErikMask;
