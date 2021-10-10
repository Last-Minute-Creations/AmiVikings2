/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "bob_new.h"
#include "player.h"

typedef enum tEntityType {
	ENTITY_TYPE_INVALID = 0,
	ENTITY_TYPE_ERIK,
	ENTITY_TYPE_OLAF,
	ENTITY_TYPE_BAELOG,
	ENTITY_TYPE_FANG,
	ENTITY_TYPE_SCORCH,
	ENTITY_TYPE_PLATFORM,
	ENTITY_TYPE_BLOCK,
} tEntityType;

typedef struct tEntity {
	tBobNew sBob;
	tEntityType eType;
} tEntity;

void entityManagerReset(void);

void entityManagerProcess(void);

void entityAdd(tEntity *pEntity);

void entitySetSteer(tEntity *pEntity, tSteerRequest eSteer);

#endif // _ENTITY_H_
