/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _ENTITY_ERIK_H_
#define _ENTITY_ERIK_H_

#include "entity.h"

typedef struct tEntityErik {
	tEntity sBase;
	UBYTE ubJoy;
	tUwCoordYX sPos;
} tEntityErik;

tEntityErik *entityErikCreate(void);

void entityErikProcess(tEntityErik *pEntity);

void entityErikDestroy(tEntityErik *pEntity);

#endif // _ENTITY_ERIK_H_
