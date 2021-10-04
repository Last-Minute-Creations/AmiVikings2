/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "entity.h"
#include <ace/managers/log.h>
#include "entity_erik.h"

#define ENTITIES_MAX 10

tEntity *s_pEntities[ENTITIES_MAX] = {0};

static void entityDestroy(tEntity *pEntity) {
	switch(pEntity->eType) {
		case ENTITY_TYPE_ERIK:
			entityErikDestroy((tEntityErik*)pEntity);
			break;
		default:
			break;
	}
}

void entityManagerReset(void) {
	for(UBYTE i = ENTITIES_MAX; i--;) {
		if(s_pEntities[i]) {
			entityDestroy(s_pEntities[i]);
		}
		s_pEntities[i] = 0;
	}
}

void entityManagerProcess(void) {
	for(UBYTE i = ENTITIES_MAX; i--;) {
		tEntity *pEntity = s_pEntities[i];
		if(!pEntity) {
			continue;
		}
		// TODO: check if on screen
		switch(pEntity->eType) {
			case ENTITY_TYPE_ERIK:
				entityErikProcess((tEntityErik*)pEntity);
				break;
			default:
				break;
		}
	}
}

void entityAdd(tEntity *pEntity) {
	for(UBYTE i = 0; i < ENTITIES_MAX; ++i) {
		if(s_pEntities[i] == 0) {
			s_pEntities[i] = pEntity;
			return;
		}
	}
	logWrite("ERR: Can't add entity %p - no more space!", pEntity);
}
