#include <entity/entity_info_box.hpp>
#include "assets.hpp"
#include "entity_info_box.hpp"

void entityInfoBoxCreate(
	tEntity &Entity, UWORD uwPosX, UWORD uwPosY,
	UWORD uwParam1, UWORD uwMessageIndex
)
{
	bobInit(
		&Entity.sBob, 16, 16, true,
		bobCalcFrameAddress(g_pBobBmHelpBox, 0),
		bobCalcFrameAddress(g_pBobBmMaskFull16, 0),
		uwPosX, uwPosY
	);

	auto &Data = Entity.dataAs<tEntityInfoBoxData>();
	Data.isTriggered = false;
	Data.isTriggeringOnCollision = true; // todo
	Data.uwMessageIndex = uwMessageIndex;
}

void entityInfoBoxProcess(tEntity &Entity)
{
	bobPush(&Entity.sBob);
}

void entityInfoBoxDestroy(tEntity &Entity)
{

}

bool entityInfoBoxCollided(tEntity &Entity, tEntity &Collider)
{
	auto &Data = Entity.dataAs<tEntityInfoBoxData>();
	if(!Data.isTriggeringOnCollision || Data.isTriggered) {
		return false;
	}

	const auto &ColliderKind = Collider.pDef->eKind;
	if(ColliderKind == tEntityKind::Erik) {
		logWrite("Would display message %hu\n", Data.uwMessageIndex);
		Data.isTriggered = true;
	}

	return false;
}
