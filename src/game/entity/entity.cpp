/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <entity/entity.hpp>
#include <ace/managers/log.h>
#include <lmc/enum_value.hpp>
#include <entity/entity_viking.hpp>

using namespace Lmc;

#define ENTITY_INSTANCE_MAX 10

static const tEntityDef s_pEntityDefs[] = {
	[enumValue(tEntityKind::Invalid)] = {.eKind = tEntityKind::Invalid, .cbReset = nullptr, .cbProcess = nullptr, .cbDestroy = nullptr},
	[enumValue(tEntityKind::Erik)] = {.eKind = tEntityKind::Erik, .cbReset = entityVikingCreate, .cbProcess = entityVikingProcess, .cbDestroy = entityVikingDestroy},
	[enumValue(tEntityKind::Olaf)] = {.eKind = tEntityKind::Olaf, .cbReset = entityVikingCreate, .cbProcess = entityVikingProcess, .cbDestroy = entityVikingDestroy},
	[enumValue(tEntityKind::Baelog)] = {.eKind = tEntityKind::Baelog, .cbReset = entityVikingCreate, .cbProcess = entityVikingProcess, .cbDestroy = entityVikingDestroy},
	[enumValue(tEntityKind::Fang)] = {.eKind = tEntityKind::Fang, .cbReset = nullptr, .cbProcess = nullptr, .cbDestroy = nullptr},
	[enumValue(tEntityKind::Scorch)] = {.eKind = tEntityKind::Scorch, .cbReset = nullptr, .cbProcess = nullptr, .cbDestroy = nullptr},
	[enumValue(tEntityKind::Platform)] = {.eKind = tEntityKind::Platform, .cbReset = nullptr, .cbProcess = nullptr, .cbDestroy = nullptr},
	[enumValue(tEntityKind::Block)] = {.eKind = tEntityKind::Block, .cbReset = nullptr, .cbProcess = nullptr, .cbDestroy = nullptr},
};

static tEntity s_pEntities[ENTITY_INSTANCE_MAX] = {{}};

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
		s_pEntities[i].pDef = nullptr;
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
			s_pEntities[i].pDef = &s_pEntityDefs[enumValue(eKind)];
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
