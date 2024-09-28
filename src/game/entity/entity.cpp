/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <entity/entity.hpp>
#include <ace/managers/log.h>
#include <lmc/enum_value.hpp>
#include <entity/entity_viking.hpp>
#include <entity/entity_info_box.hpp>

using namespace Lmc;

#define ENTITY_INSTANCE_MAX 10

static const tEntityDef s_pEntityDefs[] = {
	[enumValue(tEntityKind::Invalid)] = {
		.eKind = tEntityKind::Invalid,
		.cbCreate = nullptr, .cbProcess = nullptr, .cbDestroy = nullptr,
		.cbCollided = nullptr
	},
	[enumValue(tEntityKind::Erik)] = {
		.eKind = tEntityKind::Erik,
		.cbCreate = entityVikingCreate,
		.cbProcess = entityVikingProcess,
		.cbDestroy = entityVikingDestroy,
		.cbCollided = nullptr
	},
	[enumValue(tEntityKind::Olaf)] = {
		.eKind = tEntityKind::Olaf,
		.cbCreate = entityVikingCreate,
		.cbProcess = entityVikingProcess,
		.cbDestroy = entityVikingDestroy,
		.cbCollided = nullptr
	},
	[enumValue(tEntityKind::Baelog)] = {
		.eKind = tEntityKind::Baelog,
		.cbCreate = entityVikingCreate,
		.cbProcess = entityVikingProcess,
		.cbDestroy = entityVikingDestroy,
		.cbCollided = nullptr
	},
	[enumValue(tEntityKind::Fang)] = {
		.eKind = tEntityKind::Fang,
		.cbCreate = nullptr,
		.cbProcess = nullptr,
		.cbDestroy = nullptr,
		.cbCollided = nullptr
	},
	[enumValue(tEntityKind::Scorch)] = {
		.eKind = tEntityKind::Scorch,
		.cbCreate = nullptr,
		.cbProcess = nullptr,
		.cbDestroy = nullptr,
		.cbCollided = nullptr
	},
	[enumValue(tEntityKind::Platform)] = {
		.eKind = tEntityKind::Platform,
		.cbCreate = nullptr,
		.cbProcess = nullptr,
		.cbDestroy = nullptr,
		.cbCollided = nullptr
	},
	[enumValue(tEntityKind::Block)] = {
		.eKind = tEntityKind::Block,
		.cbCreate = nullptr,
		.cbProcess = nullptr,
		.cbDestroy = nullptr,
		.cbCollided = nullptr
	},
	[enumValue(tEntityKind::InfoBox)] = {
		.eKind = tEntityKind::InfoBox,
		.cbCreate = entityInfoBoxCreate,
		.cbProcess = entityInfoBoxProcess,
		.cbDestroy = entityInfoBoxDestroy,
		.cbCollided = entityInfoBoxCollided
	},
};

static tEntity s_pEntities[ENTITY_INSTANCE_MAX] = {{}};

static void entityDestroy(tEntity &Entity) {
	if(!Entity.isValid() || !Entity.pDef->cbCreate) {
		logWrite("ERR: entity cbDestroy is zero\n");
	}
	Entity.pDef->cbDestroy(Entity);
}

void entityManagerReset(void) {
	for(UBYTE i = ENTITY_INSTANCE_MAX; i--;) {
		if(s_pEntities[i].isValid()) {
			entityDestroy(s_pEntities[i]);
		}
		s_pEntities[i].pDef = nullptr;
	}
}

void entityManagerProcess(void) {
	for(UBYTE i = ENTITY_INSTANCE_MAX; i--;) {
		auto &Entity = s_pEntities[i];
		if(!Entity.isValid()) {
			continue;
		}
		// TODO: check if on screen
		Entity.pDef->cbProcess(Entity);
	}
}

tEntity *entityManagerSpawnEntity(
	tEntityKind eKind, UWORD uwX, UWORD uwY, UWORD uwCenterX, UWORD uwCenterY,
	UWORD uwParam1, UWORD uwParam2
)
{
	for(UBYTE i = 0; i < ENTITY_INSTANCE_MAX; ++i) {
		if(!s_pEntities[i].isValid()) {
			s_pEntities[i].pDef = &s_pEntityDefs[enumValue(eKind)];
			// TODO: pass x/y/cx/cy to entity instance, or to the cbCreate() call
			if(!s_pEntities[i].pDef->cbCreate) {
				logWrite("ERR: entity cbCreate is zero\n");
			}
			s_pEntities[i].pDef->cbCreate(s_pEntities[i], uwX, uwY, uwParam1, uwParam2);
			return &s_pEntities[i];
		}
	}
	logWrite("ERR: Can't add entity %d - no more space!", enumValue(eKind));
	return 0;
}

bool entityCheckForCollisionsWith(tEntity &Entity)
{
	bool isCollided = true;
	for(UBYTE i = ENTITY_INSTANCE_MAX; i--;) {
		auto &Other = s_pEntities[i];
		if(&Other == &Entity || !Other.isValid()) {
			continue;
		}

		if (
			Entity.sBob.sPos.uwX < Other.sBob.sPos.uwX + Other.sBob.uwWidth &&
			Entity.sBob.sPos.uwX + Entity.sBob.uwWidth > Other.sBob.sPos.uwX &&
			Entity.sBob.sPos.uwY < Other.sBob.sPos.uwY + Other.sBob.uwHeight &&
			Entity.sBob.sPos.uwY + Entity.sBob.uwHeight > Other.sBob.sPos.uwY
		) {
			if(Other.pDef->cbCollided != nullptr) {
				isCollided |= Other.pDef->cbCollided(Other, Entity);
			}
		}
	}
	return isCollided;
}
