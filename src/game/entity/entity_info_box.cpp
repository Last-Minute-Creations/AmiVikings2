#include <entity/entity_info_box.hpp>
#include "assets.hpp"
#include "entity_info_box.hpp"
#include "state_game.hpp"

void entityInfoBoxCreate(
	tEntity &Entity, UWORD uwPosX, UWORD uwPosY, UWORD uwCenterX, UWORD uwCenterY,
	UWORD uwParam1, UWORD uwMessageId
)
{
	bobInit(
		&Entity.sBob, 16, 16, true,
		bobCalcFrameAddress(g_pBobBmHelpBox, 0),
		bobCalcFrameAddress(g_pBobBmMaskFull16, 0),
		uwPosX - uwCenterX, uwPosY - uwCenterY
	);

	auto &Data = Entity.dataAs<tEntityInfoBoxData>();
	Data.isTriggered = false;
	Data.isTriggeringOnCollision = true; // todo
	Data.uwMessageId = uwMessageId;
}

void entityInfoBoxProcess(tEntity &Self)
{
	bobPush(&Self.sBob);
}

bool entityInfoBoxCollided(tEntity &Self, tEntity &Collider)
{
	auto &Data = Self.dataAs<tEntityInfoBoxData>();
	if(!Data.isTriggeringOnCollision || Data.isTriggered) {
		return false;
	}

	const auto &ColliderKind = Collider.pDef->eKind;
	if(ColliderKind == tEntityKind::Erik) {
		gameSetPendingMessage(Data.uwMessageId, 0);
		Data.isTriggered = true;
	}

	return false;
}

void entityInfoBoxInteracted(tEntity &Self)
{
	auto &Data = Self.dataAs<tEntityInfoBoxData>();
	gameSetPendingMessage(Data.uwMessageId, 0);
	Data.isTriggered = true;
}