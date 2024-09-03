/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <entity/entity.h>
#include <entity/entity_viking.h>
#include <ace/managers/log.h>

#define ENTITY_INSTANCE_MAX 10

static const tEntityDef s_pEntityDefs[] = {
	[ENTITY_KIND_INVALID] = {.eKind = ENTITY_KIND_INVALID, .cbReset = 0, .cbProcess = 0, .cbDestroy = 0},
	[ENTITY_KIND_ERIK] = {.eKind = ENTITY_KIND_ERIK, .cbReset = entityVikingCreate, .cbProcess = entityVikingProcess, .cbDestroy = entityVikingDestroy},
	[ENTITY_KIND_OLAF] = {.eKind = ENTITY_KIND_OLAF, .cbReset = entityVikingCreate, .cbProcess = entityVikingProcess, .cbDestroy = entityVikingDestroy},
	[ENTITY_KIND_BAELOG] = {.eKind = ENTITY_KIND_BAELOG, .cbReset = entityVikingCreate, .cbProcess = entityVikingProcess, .cbDestroy = entityVikingDestroy},
	[ENTITY_KIND_FANG] = {.eKind = ENTITY_KIND_FANG, .cbReset = 0, .cbProcess = 0, .cbDestroy = 0},
	[ENTITY_KIND_SCORCH] = {.eKind = ENTITY_KIND_SCORCH, .cbReset = 0, .cbProcess = 0, .cbDestroy = 0},
	[ENTITY_KIND_PLATFORM] = {.eKind = ENTITY_KIND_PLATFORM, .cbReset = 0, .cbProcess = 0, .cbDestroy = 0},
	[ENTITY_KIND_BLOCK] = {.eKind = ENTITY_KIND_BLOCK, .cbReset = 0, .cbProcess = 0, .cbDestroy = 0},
};

static tEntity s_pEntities[ENTITY_INSTANCE_MAX] = {0};

static void entityDestroy(tEntity *pEntity) {
	if(!pEntity->pDef->cbReset) {
		logWrite("ERR: entity cbDestroy is zero\n");
	}
	pEntity->pDef->cbDestroy(pEntity);
}

void entityManagerReset(void) {
	for(UBYTE i = ENTITY_INSTANCE_MAX; i--;) {
		if(s_pEntities[i].pDef) {
			entityDestroy(&s_pEntities[i]);
		}
		s_pEntities[i].pDef = 0;
	}
}

void entityManagerProcess(void) {
	for(UBYTE i = ENTITY_INSTANCE_MAX; i--;) {
		tEntity *pEntity = &s_pEntities[i];
		if(!pEntity->pDef) {
			continue;
		}
		// TODO: check if on screen
		pEntity->pDef->cbProcess(pEntity);
	}
}

tEntity *entityManagerSpawnEntity(
	tEntityKind eKind, UWORD uwX, UWORD uwY, UWORD uwCenterX, UWORD uwCenterY
) {
	for(UBYTE i = 5; i < ENTITY_INSTANCE_MAX; ++i) {
		if(s_pEntities[i].pDef == 0) {
			s_pEntities[i].pDef = &s_pEntityDefs[eKind];
			// TODO: pass x/y/cx/cy to entity instance, or to the cbReset() call
			if(!s_pEntities[i].pDef->cbReset) {
				logWrite("ERR: entity cbReset is zero\n");
			}
			s_pEntities[i].pDef->cbReset(&s_pEntities[i], uwX, uwY);
			return &s_pEntities[i];
		}
	}
	logWrite("ERR: Can't add entity %d - no more space!", eKind);
	return 0;
}
