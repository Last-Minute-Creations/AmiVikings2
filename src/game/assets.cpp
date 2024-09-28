/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "assets.hpp"

void assetsGlobalCreate(void) {
	g_pBobBmErik = bitmapCreateFromFile("data/erik.bm", 0);
	g_pBobBmErikMask = bitmapCreateFromFile("data/erik_mask.bm", 0);
	g_pBobBmHelpBox = bitmapCreateFromFile("data/help_box.bm", 0);
	g_pBobBmMaskFull16 = bitmapCreateFromFile("data/mask_full_16.bm", 0);
}

void assetsGlobalDestroy(void) {
	bitmapDestroy(g_pBobBmErik);
	bitmapDestroy(g_pBobBmErikMask);

	bitmapDestroy(g_pBobBmHelpBox);
	bitmapDestroy(g_pBobBmMaskFull16);
}

// Global assets
tBitMap *g_pBobBmErik, *g_pBobBmErikMask;
tBitMap *g_pBobBmHelpBox, *g_pBobBmMaskFull16;
