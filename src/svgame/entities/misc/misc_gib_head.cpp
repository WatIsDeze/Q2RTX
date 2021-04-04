// LICENSE HERE.

//
// svgame/entities/misc_gib_leg.c
//
//
// misc_gib_leg entity implementation.
//

// Include local game header.
#include "../../g_local.h"

// Declared in misc.c
extern void gib_die(entity_t* self, entity_t* inflictor, entity_t* attacker, int damage, const vec3_t& point);

//=====================================================
/*QUAKED misc_gib_head (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_head(entity_t* ent)
{
    gi.SetModel(ent, "models/objects/gibs/head/tris.md2");
    ent->solid = SOLID_NOT;
    ent->s.effects |= EF_GIB;
    ent->takeDamage = DAMAGE_YES;
    ent->Die = gib_die;
    ent->moveType = MOVETYPE_TOSS;
    ent->svFlags |= SVF_MONSTER;
    ent->deadFlag = DEAD_DEAD;
    ent->avelocity[0] = random() * 200;
    ent->avelocity[1] = random() * 200;
    ent->avelocity[2] = random() * 200;
    ent->Think = G_FreeEntity;
    ent->nextThink = level.time + 30;
    gi.LinkEntity(ent);
}