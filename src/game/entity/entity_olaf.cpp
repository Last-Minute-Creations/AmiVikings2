/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "entity_olaf.hpp"
#include <ace/managers/key.h>
#include <entity/entity_viking.hpp>
#include "assets.hpp"
#include "tile.hpp"

using namespace Lmc;

static const tEntityVikingDefs s_DefsOlaf = {
	.AnimDefs = tEntityVikingDefs::tAnimDefArray()
		.withElement(
			enumValue(tEntityVikingAnimationKind::Stand),
			 tEntityVikingAnimDef {.ubFrameFirst = 0, .ubFrameLast = 0}
		)
		.withElement(
			enumValue(tEntityVikingAnimationKind::Walk),
			 tEntityVikingAnimDef {.ubFrameFirst = 1, .ubFrameLast = 8}
		)
};


void entityOlafCreate(
	tEntity &Entity, UWORD uwPosX, UWORD uwPosY, UWORD uwCenterX, UWORD uwCenterY,
	UWORD uwParam1, UWORD uwParam2
)
{
	entityVikingCreate(Entity, uwPosX, uwPosY, uwCenterX, uwCenterY, uwParam1, uwParam2);

	auto &Data = Entity.dataAs<tEntityVikingData>();
	Data.pVikingDefs = &s_DefsOlaf;
	Data.pFrames[enumValue(tEntityVikingFacing::Left)] = g_pBobBmOlafLeft;
	Data.pMasks[enumValue(tEntityVikingFacing::Left)] = g_pBobBmOlafLeftMask;
	Data.pFrames[enumValue(tEntityVikingFacing::Right)] = g_pBobBmOlafRight;
	Data.pMasks[enumValue(tEntityVikingFacing::Right)] = g_pBobBmOlafRightMask;
}
