#ifndef INC_GAME_ENTITY_INFO_BOX_HPP
#define INC_GAME_ENTITY_INFO_BOX_HPP

#include <entity/entity.hpp>
#include <lmc/array.hpp>

// TODO: generalize to tEntityNpc?

struct tEntityInfoBoxData {
	union {
		tEntity::tData sEntityData;
		struct {
			UWORD uwMessageIndex;
			bool isTriggeringOnCollision;
			bool isTriggered;
		};
	};
};

void entityInfoBoxCreate(
	tEntity &Entity, UWORD uwPosX, UWORD uwPosY, UWORD uwCenterX, UWORD uwCenterY,
	UWORD uwParam1, UWORD uwMessageIndex
);

void entityInfoBoxProcess(tEntity &Entity);

void entityInfoBoxDestroy(tEntity &Entity);

bool entityInfoBoxCollided(tEntity &Entity, tEntity &Collider);

template<>
struct tEntityLookup<tEntityInfoBoxData> { static constexpr auto getKind() {return tEntityKind::InfoBox;} };

#endif // INC_GAME_ENTITY_INFO_BOX_HPP
