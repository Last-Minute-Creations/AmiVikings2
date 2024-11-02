/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <entity/entity.hpp>
#include <ace/managers/log.h>
#include <lmc/enum_value.hpp>
#include <lmc/assert.hpp>
#include <entity/entity_viking.hpp>
#include <entity/entity_erik.hpp>
#include <entity/entity_olaf.hpp>
#include <entity/entity_baelog.hpp>
#include <entity/entity_info_box.hpp>
#include "entity.hpp"

using namespace Lmc;

#define ENTITY_INSTANCE_MAX 10

static const tEntityDef s_pEntityDefs[] = {
	[enumValue(tEntityKind::Invalid)] = {
		.eKind = tEntityKind::Invalid,
		.cbCreate = nullptr, .cbProcess = nullptr, .cbDestroy = nullptr,
		.cbCollided = nullptr, .cbInteracted = nullptr, .cbItemUsed = nullptr
	},
	[enumValue(tEntityKind::Erik)] = {
		.eKind = tEntityKind::GroupViking | tEntityKind::Erik,
		.cbCreate = entityErikCreate,
		.cbProcess = entityVikingProcess,
		.cbDestroy = nullptr,
		.cbCollided = nullptr,
		.cbInteracted = nullptr,
		.cbItemUsed = nullptr,
	},
	[enumValue(tEntityKind::Baelog)] = {
		.eKind = tEntityKind::GroupViking | tEntityKind::Baelog,
		.cbCreate = entityBaelogCreate,
		.cbProcess = entityVikingProcess,
		.cbDestroy = nullptr,
		.cbCollided = nullptr,
		.cbInteracted = nullptr,
		.cbItemUsed = nullptr,
	},
	[enumValue(tEntityKind::Olaf)] = {
		.eKind = tEntityKind::GroupViking | tEntityKind::Olaf,
		.cbCreate = entityOlafCreate,
		.cbProcess = entityVikingProcess,
		.cbDestroy = nullptr,
		.cbCollided = nullptr,
		.cbInteracted = nullptr,
		.cbItemUsed = nullptr,
	},
	[enumValue(tEntityKind::Fang)] = {
		.eKind = tEntityKind::GroupViking | tEntityKind::Fang,
		.cbCreate = nullptr,
		.cbProcess = nullptr,
		.cbDestroy = nullptr,
		.cbCollided = nullptr,
		.cbInteracted = nullptr,
		.cbItemUsed = nullptr,
	},
	[enumValue(tEntityKind::Scorch)] = {
		.eKind = tEntityKind::GroupViking | tEntityKind::Scorch,
		.cbCreate = nullptr,
		.cbProcess = nullptr,
		.cbDestroy = nullptr,
		.cbCollided = nullptr,
		.cbInteracted = nullptr,
		.cbItemUsed = nullptr,
	},
	[enumValue(tEntityKind::Platform)] = {
		.eKind = tEntityKind::Platform,
		.cbCreate = nullptr,
		.cbProcess = nullptr,
		.cbDestroy = nullptr,
		.cbCollided = nullptr,
		.cbInteracted = nullptr,
		.cbItemUsed = nullptr,
	},
	[enumValue(tEntityKind::Block)] = {
		.eKind = tEntityKind::Block,
		.cbCreate = nullptr,
		.cbProcess = nullptr,
		.cbDestroy = nullptr,
		.cbCollided = nullptr,
		.cbInteracted = nullptr,
		.cbItemUsed = nullptr,
	},
	[enumValue(tEntityKind::InfoBox)] = {
		.eKind = tEntityKind::InfoBox,
		.cbCreate = entityInfoBoxCreate,
		.cbProcess = entityInfoBoxProcess,
		.cbDestroy = nullptr,
		.cbCollided = entityInfoBoxCollided,
		.cbInteracted = entityInfoBoxInteracted,
		.cbItemUsed = nullptr,
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
			s_pEntities[i].pDef->cbCreate(s_pEntities[i], uwX, uwY, uwCenterX, uwCenterY, uwParam1, uwParam2);
			return &s_pEntities[i];
		}
	}
	logWrite("ERR: Can't add entity %d - no more space!", enumValue(eKind));
	return 0;
}

tEntity &entityManagerGetEntityFromIndex(UBYTE ubEntityIndex)
{
	LMC_ASSERT(ubEntityIndex < ENTITY_INSTANCE_MAX);
	return s_pEntities[ubEntityIndex];
}

bool tEntity::checkForCollisions()
{
	bool isCollided = true;
	for(UBYTE i = ENTITY_INSTANCE_MAX; i--;) {
		auto &Other = s_pEntities[i];
		if(&Other == this || !Other.isValid()) {
			continue;
		}

		if (
			this->sBob.sPos.uwX < Other.sBob.sPos.uwX + Other.sBob.uwWidth &&
			this->sBob.sPos.uwX + this->sBob.uwWidth > Other.sBob.sPos.uwX &&
			this->sBob.sPos.uwY < Other.sBob.sPos.uwY + Other.sBob.uwHeight &&
			this->sBob.sPos.uwY + this->sBob.uwHeight > Other.sBob.sPos.uwY
		) {
			if(Other.pDef->cbCollided != nullptr) {
				isCollided |= Other.pDef->cbCollided(Other, *this);
			}
		}
	}
	return isCollided;
}

void tEntity::tryInteract()
{
	for(UBYTE i = ENTITY_INSTANCE_MAX; i--;) {
		auto &Other = s_pEntities[i];
		if(&Other == this || !Other.isValid()) {
			continue;
		}

		if (
			this->sBob.sPos.uwX < Other.sBob.sPos.uwX + Other.sBob.uwWidth &&
			this->sBob.sPos.uwX + this->sBob.uwWidth > Other.sBob.sPos.uwX &&
			this->sBob.sPos.uwY < Other.sBob.sPos.uwY + Other.sBob.uwHeight &&
			this->sBob.sPos.uwY + this->sBob.uwHeight > Other.sBob.sPos.uwY
		) {
			if(Other.pDef->cbInteracted != nullptr) {
				Other.pDef->cbInteracted(Other);
				return;
			}
		}
	}
}
