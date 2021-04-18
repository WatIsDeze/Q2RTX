// LICENSE HERE.

//
// svgame/entities/func_wall.c
//
//
// func_wall entity implementation.
//

#include "../../g_local.h"      // Include SVGame funcs.
#include "../../utils.h"        // Include Util funcs.

//=====================================================
/*QUAKED func_wall (0 .5 .8) ? TRIGGER_SPAWN TOGGLE START_ON ANIMATED ANIMATED_FAST
This is just a solid wall if not inhibited

TRIGGER_SPAWN   the wall will not be present until triggered
                it will then blink in to existance; it will
                kill anything that was in it's way

TOGGLE          only valid for TRIGGER_SPAWN walls
                this allows the wall to be turned on and off

START_ON        only valid for TRIGGER_SPAWN walls
                the wall will initially be present
*/

void func_wall_use(entity_t* self, entity_t* other, entity_t* activator)
{
    if (self->solid == SOLID_NOT) {
        self->solid = SOLID_BSP;
        self->svFlags &= ~SVF_NOCLIENT;
        KillBox(self);
    }
    else {
        self->solid = SOLID_NOT;
        self->svFlags |= SVF_NOCLIENT;
    }
    gi.LinkEntity(self);

    if (!(self->spawnFlags & 2))
        self->Use = NULL;
}

void SP_func_wall(entity_t* self)
{
    self->moveType = MOVETYPE_PUSH;
    gi.SetModel(self, self->model);

    if (self->spawnFlags & 8)
        self->s.effects |= EntityEffectType::EET_AnimCycleAll2hz;
    if (self->spawnFlags & 16)
        self->s.effects |= EntityEffectType::EET_AnimCycleAll30hz;

    // just a wall
    if ((self->spawnFlags & 7) == 0) {
        self->solid = SOLID_BSP;
        gi.LinkEntity(self);
        return;
    }

    // it must be TRIGGER_SPAWN
    if (!(self->spawnFlags & 1)) {
        //      gi.DPrintf("func_wall missing TRIGGER_SPAWN\n");
        self->spawnFlags |= 1;
    }

    // yell if the spawnFlags are odd
    if (self->spawnFlags & 4) {
        if (!(self->spawnFlags & 2)) {
            gi.DPrintf("func_wall START_ON without TOGGLE\n");
            self->spawnFlags |= 2;
        }
    }

    self->Use = func_wall_use;
    if (self->spawnFlags & 4) {
        self->solid = SOLID_BSP;
    }
    else {
        self->solid = SOLID_NOT;
        self->svFlags |= SVF_NOCLIENT;
    }
    gi.LinkEntity(self);
}