/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "item.hpp"
#include <lmc/enum_value.hpp>
#include <ace/managers/log.h>

using namespace Lmc;

UBYTE itemIsDiscardable(tItemKind eItemKind)
{
	if(eItemKind == tItemKind::None) {
		logWrite("ERR: itemIsDiscardable(ITEM_NONE)\n");
	}

	if(enumValue(tItemKind::W1RedKey) <= enumValue(eItemKind) && enumValue(eItemKind) <= enumValue(tItemKind::Torch)) {
		return 1;
	}
	return 0;
}
