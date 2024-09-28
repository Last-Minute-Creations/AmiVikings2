#ifndef _PLAYER_CONTROLLER_H_
#define _PLAYER_CONTROLLER_H_

#include <ace/types.h>
#include "steer.hpp"
#include <entity/entity.hpp>

#define VIKING_ENTITY_MAX 3

enum class tPlayerIdx: UBYTE {
	First,
	Second,
	Count,
	None = Count,
};

constexpr tPlayerIdx playerGetOther(tPlayerIdx eIdx) {
	if(eIdx == tPlayerIdx::First) {
		return tPlayerIdx::Second;
	}
	else {
		return tPlayerIdx::First;
	}
}

UBYTE playerIsActive(tPlayerIdx ePlayerIdx);

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

