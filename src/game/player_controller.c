#include "player_controller.h"
#include "entity.h"

typedef struct tPlayerState {
	UBYTE ubActiveVikingIndex;
	tSteer sSteer;
} tPlayerState;


static tPlayerState s_pPlayerStates[PLAYER_COUNT];
static tEntity *s_pVikingEntities[VIKING_ENTITY_MAX];

UBYTE playerIsActive(UBYTE ubPlayerIndex) {
	return s_pPlayerStates[ubPlayerIndex].sSteer.eKind != STEER_KIND_NULL;
}

void playerControllerReset(void) {
	steerReset(&s_pPlayerStates[PLAYER_1].sSteer, STEER_KIND_NULL);
	s_pPlayerStates[PLAYER_1].ubActiveVikingIndex = 0;

	steerReset(&s_pPlayerStates[PLAYER_2].sSteer, STEER_KIND_NULL);
	s_pPlayerStates[PLAYER_2].ubActiveVikingIndex = 0;

	for(UBYTE i = 0; i < VIKING_ENTITY_MAX; ++i) {
		s_pVikingEntities[i] = 0;
	}
}

void playerControllerSetSteerPresets(tSteerKind eSteerKindP1, tSteerKind eSteerKindP2) {
	steerReset(&s_pPlayerStates[PLAYER_1].sSteer, eSteerKindP1);
	steerReset(&s_pPlayerStates[PLAYER_2].sSteer, eSteerKindP2);
}

void playerControllerSetVikingEntity(UBYTE ubVikingIndex, tEntity *pEntity) {
	s_pVikingEntities[ubVikingIndex] = pEntity;
}

tEntity *playerControllerGetVikingByIndex(UBYTE ubVikingIndex) {
	return s_pVikingEntities[ubVikingIndex];
}

tEntity *playerControllerGetVikingByPlayer(tPlayerIdx ePlayerIdx) {
	return s_pVikingEntities[s_pPlayerStates[ePlayerIdx].ubActiveVikingIndex];
}

UBYTE playerControllerGetVikingIndexByPlayer(tPlayerIdx ePlayerIdx) {
	return s_pPlayerStates[ePlayerIdx].ubActiveVikingIndex;
}

void playerControllerSetControlledVikingIndex(tPlayerIdx ePlayerIdx, UBYTE ubNewVikingIndex) {
	UBYTE ubOldVikingIndex = s_pPlayerStates[ePlayerIdx].ubActiveVikingIndex;
	entitySetSteer(s_pVikingEntities[ubOldVikingIndex], steerGetNull());
	entitySetSteer(s_pVikingEntities[ubNewVikingIndex], &s_pPlayerStates[ePlayerIdx].sSteer);
	s_pPlayerStates[ePlayerIdx].ubActiveVikingIndex = ubNewVikingIndex;
}

void playerControllerSetDefaultSelection(void)
{
	s_pPlayerStates[PLAYER_1].ubActiveVikingIndex = 0;
	s_pPlayerStates[PLAYER_2].ubActiveVikingIndex = 1;

	entitySetSteer(s_pVikingEntities[s_pPlayerStates[PLAYER_1].ubActiveVikingIndex], &s_pPlayerStates[PLAYER_1].sSteer);
	entitySetSteer(s_pVikingEntities[s_pPlayerStates[PLAYER_2].ubActiveVikingIndex], &s_pPlayerStates[PLAYER_2].sSteer);
	// TODO: handle disabled 2nd viking
}

tSteer *playerControllerGetSteer(tPlayerIdx ePlayerIdx)
{
	return &s_pPlayerStates[ePlayerIdx].sSteer;
}
