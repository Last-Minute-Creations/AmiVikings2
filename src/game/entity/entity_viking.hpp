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

enum class tEntityVikingAnimationKind: UBYTE {
	Stand,
	Walk,
	Attack,
	Hit,
	Die,

	Count
};

enum class tEntityVikingFacing: UBYTE {
	Right,
	Left,

	Count
};

struct tEntityVikingAnimDef {
	UBYTE ubFrameFirst;
	UBYTE ubFrameLast;
};

struct tEntityVikingDefs {
	using tAnimDefArray = Lmc::tArray<tEntityVikingAnimDef, Lmc::enumValue(tEntityVikingAnimationKind::Count)>;

	tAnimDefArray AnimDefs;
};

struct tEntityVikingData {
	union {
		tEntity::tData sEntityData;
		struct {
			tSteer *pSteer;
			tBitMap *pFrames[Lmc::enumValue(tEntityVikingFacing::Count)];
			tBitMap *pMasks[Lmc::enumValue(tEntityVikingFacing::Count)];
			const tEntityVikingDefs *pVikingDefs;
			tItemKind pInventory[VIKING_INVENTORY_SIZE];
			tVikingState eState;
			tMoveState eMoveState;
			tEntityVikingFacing eFacing;
			tEntityVikingAnimationKind eCurrentAnimation;
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

template<>
struct tEntityLookup<tEntityVikingData> { static constexpr auto getKind() {return tEntityKind::GroupViking;} };

#endif // INC_GAME_ENTITY_ENTITY_VIKING_HPP
