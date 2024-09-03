/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ENTITY_ENTITY_H
#define ENTITY_ENTITY_H

#include <ace/managers/bob.h>
#include "steer.h"

#define ENTITY_DATA_MAX_SIZE 20

struct tEntity;

typedef enum tEntityKind {
	ENTITY_KIND_INVALID = 0,
	ENTITY_KIND_ERIK,
	ENTITY_KIND_OLAF,
	ENTITY_KIND_BAELOG,
	ENTITY_KIND_FANG,
	ENTITY_KIND_SCORCH,
	ENTITY_KIND_PLATFORM,
	ENTITY_KIND_BLOCK,
} tEntityKind;

typedef void (*tCbEntityReset)(struct tEntity *pEntity, UWORD uwPosX, UWORD uwPosY);
typedef void (*tCbEntityProcess)(struct tEntity *pEntity);
typedef void (*tCbEntityDestroy)(struct tEntity *pEntity);

typedef UBYTE tEntityData[ENTITY_DATA_MAX_SIZE];

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
} tEntity;


void entityManagerReset(void);
void entityManagerProcess(void);
tEntity *entityManagerSpawnEntity(tEntityKind eKind, UWORD uwX, UWORD uwY, UWORD uwCenterX, UWORD uwCenterY);

#endif // ENTITY_ENTITY_H
