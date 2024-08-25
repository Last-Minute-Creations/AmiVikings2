#ifndef _PLAYER_CONTROLLER_H_
#define _PLAYER_CONTROLLER_H_

#include <ace/types.h>
#include "steer.h"
#include "entity.h"

#define VIKING_ENTITY_MAX 3

typedef enum tPlayerIdx {
	PLAYER_1,
	PLAYER_2,
	PLAYER_COUNT,
	PLAYER_NONE = PLAYER_COUNT,
} tPlayerIdx;

UBYTE playerIsActive(UBYTE ubPlayerIndex);

void playerControllerReset(void);

void playerControllerSetSteerPresets(tSteerKind eSteerKindP1, tSteerKind eSteerKindP2);

void playerControllerSetVikingEntity(UBYTE ubVikingIndex, tEntity *pEntity);

tEntity *playerControllerGetVikingByIndex(UBYTE ubVikingIndex);

tEntity *playerControllerGetVikingByPlayer(tPlayerIdx ePlayerIdx);

UBYTE playerControllerGetVikingIndexByPlayer(tPlayerIdx ePlayerIdx);

void playerControllerSetControlledVikingIndex(tPlayerIdx ePlayerIdx, UBYTE ubNewVikingIndex);

void playerControllerSetDefaultSelection(void);

tSteer *playerControllerGetSteer(tPlayerIdx ePlayerIdx);

#endif // _PLAYER_CONTROLLER_H_

