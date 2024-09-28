/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_GAME_ENTITY_ENTITY_HPP
#define INC_GAME_ENTITY_ENTITY_HPP

#include <ace/managers/bob.h>
#include <lmc/array.hpp>
#include "steer.hpp"

struct tEntity;

enum class tEntityKind: UBYTE {
	Invalid = 0,
	Erik,
	Olaf,
	Baelog,
	Fang,
	Scorch,
	Platform,
	Block,
	InfoBox,
};

struct tEntityDef {
	using tCbCreate = void (*)(
		tEntity &Entity, UWORD uwPosX, UWORD uwPosY,
		UWORD uwCenterX, UWORD uwCenterY, UWORD uwParam1, UWORD uwParam2
	);
	using tCbProcess = void (*)(tEntity &Entity);
	using tCbDestroy = void (*)(tEntity &Entity);
	using tCbCollided = bool(*)(tEntity &Entity, tEntity &Collided);

	tEntityKind eKind; // for cast safety checks
	tCbCreate cbCreate;
	tCbProcess cbProcess;
	tCbDestroy cbDestroy;
	tCbCollided cbCollided;
};

template <typename T>
struct tEntityLookup { };

struct tEntity {
	using tData = Lmc::tArray<UBYTE, 50>;

	const tEntityDef *pDef; // set to 0 if entity is free on s_pEntities
	tBob sBob;
	tData Data;

	constexpr bool isValid() const { return pDef != nullptr; }

	template<typename T>
	constexpr T &dataAs() {
#if defined(GAME_DEBUG)
		if(tEntityLookup<T>::getKind() != pDef->eKind) {
			logWrite("Invalid entity cast, should be %d", Lmc::enumValue(pDef->eKind));
		}
#endif

		return *reinterpret_cast<T*>(this->Data.Data);
	}
};

void entityManagerReset(void);
void entityManagerProcess(void);
tEntity *entityManagerSpawnEntity(
	tEntityKind eKind, UWORD uwX, UWORD uwY, UWORD uwCenterX, UWORD uwCenterY,
	UWORD uwParam1, UWORD uwParam2
);
bool entityCheckForCollisionsWith(tEntity &Entity);

#endif // INC_GAME_ENTITY_ENTITY_HPP
