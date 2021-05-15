// LICENSE HERE.

//
// svgame/weapons/blaster.c
//
//
// Blaster weapon code.
//

// Include local game header.
#include "../g_local.h"

// Include player headers.
#include "../player/animations.h"
#include "../player/weapons.h"

// Include weapon header.
#include "blaster.h"

//
//======================================================================
//
// BLASTER
//
//======================================================================
//

void Blaster_Fire(Entity* ent, const vec3_t &g_offset, int damage, qboolean hyper, int effect)
{
    vec3_t  forward, right;
    vec3_t  start;
    vec3_t  offset;

    if (is_quad)
        damage *= 4;
    AngleVectors(ent->client->aimAngles, &forward, &right, NULL);
    VectorSet(offset, 24, 8, ent->viewHeight - 8);
    VectorAdd(offset, g_offset, offset);
    start = SVG_PlayerProjectSource(ent->client, ent->state.origin, offset, forward, right);

    VectorScale(forward, -2, ent->client->kickOrigin);
    ent->client->kickAngles[0] = -1;

    SVG_FireBlaster(ent, start, forward, damage, 1000, effect, hyper);

    // send muzzle flash
    gi.WriteByte(SVG_CMD_MUZZLEFLASH);
    gi.WriteShort(ent - g_entities);
    gi.WriteByte(MuzzleFlashType::Blaster | is_silenced);
    gi.Multicast(&ent->state.origin, MultiCast::PVS);

    SVG_PlayerNoise(ent, start, PNOISE_WEAPON);
}


void Weapon_Blaster_Fire(Entity* ent)
{
    int     damage;

    if (deathmatch->value)
        damage = 15;
    else
        damage = 10;
    Blaster_Fire(ent, vec3_origin, damage, false, EntityEffectType::Blaster);
    ent->client->playerState.gunFrame++;
}

void Weapon_Blaster(Entity* ent)
{
    static int  pause_frames[] = { 19, 32, 0 };
    static int  fire_frames[] = { 5, 0 };

    Weapon_Generic(ent, 4, 8, 52, 55, pause_frames, fire_frames, Weapon_Blaster_Fire);
}