#ifndef ENTITY_ENTITY_VIKING_H
#define ENTITY_ENTITY_VIKING_H

#include <entity/entity.h>
#include <item.h>
#include <steer.h>

#define VIKING_INVENTORY_SIZE 1

typedef enum tVikingState {
	VIKING_STATE_LOCKED,
	VIKING_STATE_ALIVE,
	VIKING_STATE_DEAD,
} tVikingState;

typedef enum tMoveState {
	MOVE_STATE_WALKING,
	MOVE_STATE_FALLING,
	MOVE_STATE_CLIMBING,
} tMoveState;

typedef struct tEntityVikingData {
	union {
		tEntityData sEntityData;
		struct {
			tSteer *pSteer;
			tItemKind pInventory[VIKING_INVENTORY_SIZE];
			tVikingState eState;
			tMoveState eMoveState;
			UBYTE ubAnimFrameIdx;
			UBYTE ubSelectedSlot;
			tUwCoordYX sPos;
		};
	};
} tEntityVikingData;

_Static_assert(sizeof(tEntityVikingData) <= sizeof(tEntityData));

void entityVikingSetSteer(tEntity *pEntityViking, tSteer *pSteer);

void entityVikingCreate(tEntity *pEntity, UWORD uwPosX, UWORD uwPosY);

void entityVikingProcess(tEntity *pEntity);

void entityVikingDestroy(tEntity *pEntity);

#endif // ENTITY_ENTITY_VIKING_H
