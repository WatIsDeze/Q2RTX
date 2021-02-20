/*
===========================================================================
Copyright (C) 1997-2001 Id Software, Inc.
Copyright (C) 2000-2002 Mr. Hyde and Mad Dog

This file is part of Lazarus Quake 2 Mod source code.

Lazarus Quake 2 Mod source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Lazarus Quake 2 Mod source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lazarus Quake 2 Mod source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/*
==============================================================================

PMover - AI using Player Movement.

==============================================================================
*/

#include "g_local.h"
#include "m_pmover.h"

// AI Player Move settings are stored here.
pmoveParams_t aipmp;

//
// NON IMPORTANT FUNCTION.
//
void PMover_Pain(edict_t* self, edict_t* other, float kick, int damage)
{

}

//
// NON IMPORTANT FUNCTION.
//
void PMover_Die(edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, vec3_t point)
{
	self->s.effects = 0;
	self->monsterinfo.power_armor_type = POWER_ARMOR_NONE;

	self->s.skinnum |= 1;

	if (self->deadflag == DEAD_DEAD)
		return;

	// regular death
	self->deadflag = DEAD_DEAD;
	self->takedamage = DAMAGE_YES;
}

//
// This is a straight copy from PM_Trace.
// Eventually it needs to be moved into its own file obviously.
//
edict_t* aipm_passent;
trace_t	AIPM_trace(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end)
{
	if (aipm_passent->health > 0)
		return gi.trace(start, mins, maxs, end, aipm_passent, MASK_PLAYERSOLID);
	else
		return gi.trace(start, mins, maxs, end, aipm_passent, MASK_DEADSOLID);
}

//
// Does the thinking for the pmover...
//
void PMover_Think(edict_t* self) {
	usercmd_t aicmd;

	// Setup a user input command for this AI frame.
	aicmd.forwardmove = 80;

	// Execute the player movement using the given "AI Player Input"
	aipm_passent = self;		// Store self in pm_passent
	self->aicmd = aicmd;		// Copy over ai movement cmd.
	Pmove(&self->aipm, &aipmp);	// Execute!
	//ge->Pmove
	
	// Copy over origin to the entity.
	VectorCopy(self->aipm.s.origin, self->move_origin);
}

/*QUAKED monster_pmover (1 .5 0) (-16 -16 -24) (16 16 32) 
*/
void SP_monster_pmover(edict_t* self)
{
	// It is free to stick around in Multiplayer.
	//if (deathmatch->value)
	//{
	//	G_FreeEdict(self);
	//	return;
	//}
	// Set movetype, solid, model, and bounds.
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex("models/monsters/infantry/tris.md2");
	VectorSet(self->mins, -16, -16, -24);
	VectorSet(self->maxs, 16, 16, 32);

	// Setup edict entity functions.
	self->pain = PMover_Pain;
	self->die = PMover_Die;

	// Set health and "flies".
	self->health = 100;
	if (!self->monsterinfo.flies)
		self->monsterinfo.flies = 0.10;

	// Setup class description.
	self->common_name = "Pmover";
	self->class_id = ENTITY_MONSTER_PMOVER;

	// Setup the think.
	self->nextthink = level.time + 0.1;
	self->think = PMover_Think;

	//-------------------------------------------------------------------------
	// Setup actual player movement.
	//-------------------------------------------------------------------------
	// Initialize the AI Player Move settings.
	// Cheap trick using a static to preven it from happening again.
	static qboolean aipmp_is_initialized = false;
	if (aipmp_is_initialized == false) {
		PmoveInit(&aipmp);
		aipmp_is_initialized = true;
	}

	// Setup the pmove trace and point contents function pointers.
	self->aipm.trace = AIPM_trace;	// adds default parms
	self->aipm.pointcontents = gi.pointcontents;

	// Copy over the entities origin into the player move for its spawn point.
	VectorCopy(self->move_origin, self->aips.pmove.origin);

	// Setup the AI Player State.
	self->aips.pmove.pm_flags &= ~PMF_NO_PREDICTION;	// We don't want it to use prediction, there is no client.
	self->aips.pmove.gravity = sv_gravity->value;		// Default gravity.
	self->aips.pmove.pm_type = PM_NORMAL;				// Defualt Player Movement.

	// Setup regular entity data.
	self->movetype = MOVETYPE_WALK;		// Regular walk movetype.
	self->svflags &= ~SVF_NOCLIENT;		// Let the server know that this is not a client either.
	self->clipmask = MASK_PLAYERSOLID;	// We want clipping to behave as if it is a player.
	self->solid = SOLID_BBOX;			// Solid bbox ofc.

	// Link entity to world.
	gi.linkentity(self);
}