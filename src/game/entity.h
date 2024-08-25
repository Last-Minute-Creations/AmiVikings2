/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <ace/managers/bob.h>
#include "steer.h"

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

typedef struct tEntity {
	tBob sBob;
	tEntityKind eType;
} tEntity;

void entityManagerReset(void);

void entityManagerProcess(void);

void entityAdd(tEntity *pEntity);

void entitySetSteer(tEntity *pEntity, tSteer *pSteer);

#endif // _ENTITY_H_
