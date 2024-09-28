#include "player_controller.hpp"
#include <lmc/enum_value.hpp>
#include "entity/entity_viking.hpp"

using namespace Lmc;

typedef struct tPlayerState {
	UBYTE ubActiveVikingIndex;
	tSteer sSteer;
} tPlayerState;


static tPlayerState s_pPlayerStates[enumValue(tPlayerIdx::Count)];
static tEntity *s_pVikingEntities[VIKING_ENTITY_MAX];

UBYTE playerIsActive(tPlayerIdx ePlayerIndex) {
	return s_pPlayerStates[enumValue(ePlayerIndex)].sSteer.eKind != tSteerKind::Null;
}

void playerControllerReset(void) {
	steerReset(&s_pPlayerStates[enumValue(tPlayerIdx::First)].sSteer, tSteerKind::Null);
	s_pPlayerStates[enumValue(tPlayerIdx::First)].ubActiveVikingIndex = 0;

	steerReset(&s_pPlayerStates[enumValue(tPlayerIdx::Second)].sSteer, tSteerKind::Null);
	s_pPlayerStates[enumValue(tPlayerIdx::Second)].ubActiveVikingIndex = 0;

	for(UBYTE i = 0; i < VIKING_ENTITY_MAX; ++i) {
		s_pVikingEntities[i] = 0;
	}
}

void playerControllerSetSteerPresets(tSteerKind eSteerKindP1, tSteerKind eSteerKindP2) {
	steerReset(&s_pPlayerStates[enumValue(tPlayerIdx::First)].sSteer, eSteerKindP1);
	steerReset(&s_pPlayerStates[enumValue(tPlayerIdx::Second)].sSteer, eSteerKindP2);
}

void playerControllerSetVikingEntity(UBYTE ubVikingIndex, tEntity *pEntity) {
	s_pVikingEntities[ubVikingIndex] = pEntity;
}

tEntity *playerControllerGetVikingByIndex(UBYTE ubVikingIndex) {
	return s_pVikingEntities[ubVikingIndex];
}

tEntity *playerControllerGetVikingByPlayer(tPlayerIdx ePlayerIdx) {
	return s_pVikingEntities[s_pPlayerStates[enumValue(ePlayerIdx)].ubActiveVikingIndex];
}

UBYTE playerControllerGetVikingIndexByPlayer(tPlayerIdx ePlayerIdx) {
	return s_pPlayerStates[enumValue(ePlayerIdx)].ubActiveVikingIndex;
}

void playerControllerSetControlledVikingIndex(tPlayerIdx ePlayerIdx, UBYTE ubNewVikingIndex) {
	UBYTE ubOldVikingIndex = s_pPlayerStates[enumValue(ePlayerIdx)].ubActiveVikingIndex;
	entityVikingSetSteer(s_pVikingEntities[ubOldVikingIndex], steerGetNull());
	entityVikingSetSteer(s_pVikingEntities[ubNewVikingIndex], &s_pPlayerStates[enumValue(ePlayerIdx)].sSteer);
	s_pPlayerStates[enumValue(ePlayerIdx)].ubActiveVikingIndex = ubNewVikingIndex;
}

void playerControllerSetDefaultSelection(void)
{
	s_pPlayerStates[enumValue(tPlayerIdx::First)].ubActiveVikingIndex = 0;
	s_pPlayerStates[enumValue(tPlayerIdx::Second)].ubActiveVikingIndex = 1;

	entityVikingSetSteer(
		s_pVikingEntities[s_pPlayerStates[enumValue(tPlayerIdx::First)].ubActiveVikingIndex],
		&s_pPlayerStates[enumValue(tPlayerIdx::First)].sSteer
	);
	entityVikingSetSteer(
		s_pVikingEntities[s_pPlayerStates[enumValue(tPlayerIdx::Second)].ubActiveVikingIndex],
		&s_pPlayerStates[enumValue(tPlayerIdx::Second)].sSteer
	);
	// TODO: handle disabled 2nd viking
}

tSteer *playerControllerGetSteer(tPlayerIdx ePlayerIdx)
{
	return &s_pPlayerStates[enumValue(ePlayerIdx)].sSteer;
}
