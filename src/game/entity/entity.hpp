/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_GAME_ENTITY_ENTITY_HPP
#define INC_GAME_ENTITY_ENTITY_HPP

#include <ace/managers/bob.h>
#include "steer.hpp"

#define ENTITY_DATA_MAX_SIZE 50

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
};

using tCbEntityReset = void (*)(struct tEntity *pEntity, UWORD uwPosX, UWORD uwPosY);
using tCbEntityProcess = void (*)(struct tEntity *pEntity);
using tCbEntityDestroy = void (*)(struct tEntity *pEntity);

template <typename T>
struct tEntityLookup { };

struct tEntityData {
	UBYTE pData[ENTITY_DATA_MAX_SIZE];
};

typedef struct tEntityDef {
	tEntityKind eKind; // for cast safety checks
	tCbEntityReset cbReset;
	tCbEntityProcess cbProcess;
	tCbEntityDestroy cbDestroy;
} tEntityDef;

typedef struct tEntity {
	const tEntityDef *pDef; // set to 0 if entity is free on s_pEntities
	tBob sBob;
	tEntityData pData;

	template<typename T>
	constexpr T *dataAs() {
// #if defined(GAME_DEBUG)
		if(tEntityLookup<T>::getKind() != pDef->eKind) {
			logWrite("Invalid entity cast, should be %d", (int)eKind);
		}
// #endif

		return reinterpret_cast<T*>(&this->pData);
	}
} tEntity;


void entityManagerReset(void);
void entityManagerProcess(void);
tEntity *entityManagerSpawnEntity(tEntityKind eKind, UWORD uwX, UWORD uwY, UWORD uwCenterX, UWORD uwCenterY);

#endif // INC_GAME_ENTITY_ENTITY_HPP
