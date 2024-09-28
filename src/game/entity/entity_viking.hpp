#ifndef ENTITY_ENTITY_VIKING_H
#define ENTITY_ENTITY_VIKING_H

#include <entity/entity.hpp>
#include <item.hpp>
#include <steer.hpp>

#define VIKING_INVENTORY_SIZE 4

enum class tVikingState: UBYTE {
	Locked,
	Alive,
	Dead,
};

enum class tMoveState: UBYTE {
	Walking,
	Falling,
	Climbing,
};

struct tEntityVikingData {
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
};

static_assert(sizeof(tEntityVikingData) <= sizeof(tEntityData));

void entityVikingSetSteer(tEntity *pEntityViking, tSteer *pSteer);

tVikingState entityVikingGetState(tEntity *pEntityViking);

BYTE entityVikingGetFreeItemSlot(tEntity *pEntityViking);

void entityVikingCreate(tEntity *pEntity, UWORD uwPosX, UWORD uwPosY);

void entityVikingProcess(tEntity *pEntity);

void entityVikingDestroy(tEntity *pEntity);

template<>
struct tEntityLookup<tEntityVikingData> { static constexpr auto getKind() {return tEntityKind::Erik;} };

#endif // ENTITY_ENTITY_VIKING_H
