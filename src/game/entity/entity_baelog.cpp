/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "entity_baelog.hpp"
#include <ace/managers/key.h>
#include <entity/entity_viking.hpp>
#include "assets.hpp"
#include "tile.hpp"

using namespace Lmc;

static const tEntityVikingDefs s_DefsBaelog = {
	.AnimDefs = tEntityVikingDefs::tAnimDefArray()
		.withElement(
			enumValue(tEntityVikingAnimationKind::Stand),
			 tEntityVikingAnimDef {.ubFrameFirst = 0, .ubFrameLast = 0}
		)
		.withElement(
			enumValue(tEntityVikingAnimationKind::Walk),
			 tEntityVikingAnimDef {.ubFrameFirst = 2, .ubFrameLast = 9}
		)
};

void entityBaelogCreate(
	tEntity &Entity, UWORD uwPosX, UWORD uwPosY, UWORD uwCenterX, UWORD uwCenterY,
	UWORD uwParam1, UWORD uwParam2
)
{
	entityVikingCreate(Entity, uwPosX, uwPosY, uwCenterX, uwCenterY, uwParam1, uwParam2);

	auto &Data = Entity.dataAs<tEntityVikingData>();
	Data.pVikingDefs = &s_DefsBaelog;
	Data.pFrames[enumValue(tEntityVikingFacing::Left)] = g_pBobBmBaelogLeft;
	Data.pMasks[enumValue(tEntityVikingFacing::Left)] = g_pBobBmBaelogLeftMask;
	Data.pFrames[enumValue(tEntityVikingFacing::Right)] = g_pBobBmBaelogRight;
	Data.pMasks[enumValue(tEntityVikingFacing::Right)] = g_pBobBmBaelogRightMask;
}
