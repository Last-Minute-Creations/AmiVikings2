/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "item.h"
#include <ace/managers/log.h>

UBYTE itemIsDiscardable(tItemKind eItemKind)
{
	if(eItemKind == ITEM_KIND_NONE) {
		logWrite("ERR: itemIsDiscardable(ITEM_NONE)\n");
	}

	if(ITEM_KIND_W1_RED_KEY <= eItemKind && eItemKind <= ITEM_KIND_TORCH) {
		return 1;
	}
	return 0;
}
