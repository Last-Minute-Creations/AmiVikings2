#ifndef INC_GAME_ENTITY_ENTITY_VIKING_HPP
#define INC_GAME_ENTITY_ENTITY_VIKING_HPP

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
		tEntity::tData sEntityData;
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

static_assert(sizeof(tEntityVikingData) <= tEntity::tData::uwSize);

void entityVikingSetSteer(tEntity *pEntityViking, tSteer *pSteer);

tVikingState entityVikingGetState(tEntity *pEntityViking);

BYTE entityVikingGetFreeItemSlot(tEntity *pEntityViking);

void entityVikingCreate(
	tEntity &Entity, UWORD uwPosX, UWORD uwPosY, UWORD uwCenterX, UWORD uwCenterY,
	UWORD uwParam1, UWORD uwParam2
);

void entityVikingProcess(tEntity &Entity);

void entityVikingDestroy(tEntity &Entity);

template<>
struct tEntityLookup<tEntityVikingData> { static constexpr auto getKind() {return tEntityKind::Erik;} };

#endif // INC_GAME_ENTITY_ENTITY_VIKING_HPP
