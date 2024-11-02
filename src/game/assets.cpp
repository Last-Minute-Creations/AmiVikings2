/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "assets.hpp"

void assetsGlobalCreate(void) {
	g_pBobBmErikLeft = bitmapCreateFromFile("data/erik_left.bm", 0);
	g_pBobBmErikLeftMask = bitmapCreateFromFile("data/erik_left_mask.bm", 0);
	g_pBobBmErikRight = bitmapCreateFromFile("data/erik_right.bm", 0);
	g_pBobBmErikRightMask = bitmapCreateFromFile("data/erik_right_mask.bm", 0);

	g_pBobBmOlafLeft = bitmapCreateFromFile("data/olaf_left.bm", 0);
	g_pBobBmOlafLeftMask = bitmapCreateFromFile("data/olaf_left_mask.bm", 0);
	g_pBobBmOlafRight = bitmapCreateFromFile("data/olaf_right.bm", 0);
	g_pBobBmOlafRightMask = bitmapCreateFromFile("data/olaf_right_mask.bm", 0);

	g_pBobBmBaelogLeft = bitmapCreateFromFile("data/baelog_left.bm", 0);
	g_pBobBmBaelogLeftMask = bitmapCreateFromFile("data/baelog_left_mask.bm", 0);
	g_pBobBmBaelogRight = bitmapCreateFromFile("data/baelog_right.bm", 0);
	g_pBobBmBaelogRightMask = bitmapCreateFromFile("data/baelog_right_mask.bm", 0);

	g_pBobBmHelpBox = bitmapCreateFromFile("data/help_box.bm", 0);
	g_pBobBmMaskFull16 = bitmapCreateFromFile("data/mask_full_16.bm", 0);
}

void assetsGlobalDestroy(void) {
	bitmapDestroy(g_pBobBmErikLeft);
	bitmapDestroy(g_pBobBmErikLeftMask);

	bitmapDestroy(g_pBobBmHelpBox);
	bitmapDestroy(g_pBobBmMaskFull16);
}

// Global assets
tBitMap *g_pBobBmErikLeft, *g_pBobBmErikLeftMask, *g_pBobBmErikRight, *g_pBobBmErikRightMask;
tBitMap *g_pBobBmOlafLeft, *g_pBobBmOlafLeftMask, *g_pBobBmOlafRight, *g_pBobBmOlafRightMask;
tBitMap *g_pBobBmBaelogLeft, *g_pBobBmBaelogLeftMask, *g_pBobBmBaelogRight, *g_pBobBmBaelogRightMask;
tBitMap *g_pBobBmFangLeft, *g_pBobBmFangLeftMask, *g_pBobBmFangRight, *g_pBobBmFangRightMask;
tBitMap *g_pBobBmScorchLeft, *g_pBobBmScorchLeftMask, *g_pBobBmScorchRight, *g_pBobBmScorchRightMask;
tBitMap *g_pBobBmHelpBox, *g_pBobBmMaskFull16;
