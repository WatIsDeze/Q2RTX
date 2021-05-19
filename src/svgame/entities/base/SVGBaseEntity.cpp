/*
// LICENSE HERE.

//
// SVGBaseEntity.cpp
//
//
*/
#include "../../g_local.h"     // SVGame.
#include "../../effects.h"     // Effects.
#include "../../utils.h"       // Util funcs.
#include "SVGBaseEntity.h"

// Constructor/Deconstructor.
SVGBaseEntity::SVGBaseEntity(Entity* svEntity) : serverEntity(svEntity) {
	thinkFunction = nullptr;
	useFunction = nullptr;
	touchFunction = nullptr;
	blockedFunction = nullptr;
	takeDamageFunction = nullptr;
	dieFunction = nullptr;
}
SVGBaseEntity::~SVGBaseEntity() {

}

// Interface functions. 
//
//===============
// SVGBaseEntity::PreCache
//
// This function is used to load all entity data with.
//===============
//
void SVGBaseEntity::PreCache() {
	gi.DPrintf("SVGBaseEntity::PreCache();");
}

//
//===============
// SVGBaseEntity::Spawn
//
// This function can be overrided, to allow for entity spawning.
// Setup the basic entity properties here.
//===============
//
void SVGBaseEntity::Spawn() {
	gi.DPrintf("SVGBaseEntity::Spawn();");
}

//
//===============
// SVGBaseEntity::PostSpawn
//
// This function can be overrided, to allow for entity post spawning.
// An example of that could be finding targetnames for certain target
// trigger settings, etc.
//===============
//
void SVGBaseEntity::PostSpawn() {
	gi.DPrintf("SVGBaseEntity::PostSpawn();");
}

//
//===============
// SVGBaseEntity::Think
//
// This function can be overrided, to allow for custom entity thinking.
// By default it only executes the 'Think' callback in case we have any set.
//===============
//
void SVGBaseEntity::Think() {
	// Safety check.
	if (thinkFunction == nullptr)
		return;

	// Execute 'Think' callback function.
	(this->*thinkFunction)();
}

//
//===============
// SVGBaseEntity::Die
//
// Execute the 'Die' callback in case we ran into any.
//===============
//
void SVGBaseEntity::Die(SVGBaseEntity* inflictor, SVGBaseEntity* attacker, int damage, const vec3_t& point) {
	// Safety check.
	if (dieFunction == nullptr)
		return;

	// Execute 'Die' callback function.
	(this->*dieFunction)(inflictor, attacker, damage, point);
}

//
//===============
// SVGBaseEntity::Touch
//
// Execute the 'Touch' callback in case we ran into any.
//===============
//
void SVGBaseEntity::Touch(SVGBaseEntity* self, SVGBaseEntity* other, cplane_t* plane, csurface_t* surf) {
	// Safety check.
	if (touchFunction == nullptr)
		return;

	// Execute 'Touch' callback function.
	(this->*touchFunction)(self, other, plane, surf);
}

//
//===============
// SVGBaseEntity::LinkEntity
//
// Link entity to world for collision testing using gi.LinkEntity.
//===============
//
void SVGBaseEntity::LinkEntity() {
	gi.LinkEntity(serverEntity);
}