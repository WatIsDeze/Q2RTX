/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef GAME_H
#define GAME_H

#include "shared/list.h"
#include "sharedgame/pmove.h"
#include "sharedgame/protocol.h"

//
// game.h -- game dll information visible to server
//

#define SVGAME_API_VERSION_MAJOR VERSION_MAJOR
#define SVGAME_API_VERSION_MINOR VERSION_MINOR
#define SVGAME_API_VERSION_POINT VERSION_POINT

// edict->svflags

#define SVF_NOCLIENT            0x00000001  // don't send entity to clients, even if it has effects
#define SVF_DEADMONSTER         0x00000002  // treat as CONTENTS_DEADMONSTER for collision
#define SVF_MONSTER             0x00000004  // treat as CONTENTS_MONSTER for collision

// edict->solid values

typedef enum {
    SOLID_NOT,          // no interaction with other objects
    SOLID_TRIGGER,      // only touch when inside, after moving
    SOLID_BBOX,         // touch on edge
    SOLID_BSP           // bsp clip, touch on edge
} solid_t;

// extended features

#define GMF_CLIENTNUM               0x00000001
#define GMF_PROPERINUSE             0x00000002
#define GMF_MVDSPEC                 0x00000004
#define GMF_WANT_ALL_DISCONNECTS    0x00000008

#define GMF_ENHANCED_SAVEGAMES      0x00000400
#define GMF_VARIABLE_FPS            0x00000800
#define GMF_EXTRA_USERINFO          0x00001000
#define GMF_IPV6_ADDRESS_AWARE      0x00002000

//===============================================================

#define MAX_ENT_CLUSTERS    16


typedef struct edict_s edict_t;
typedef struct gclient_s gclient_t;


#ifndef GAME_INCLUDE

struct gclient_s {
    player_state_t  ps;     // communicated by server to clients
    int             ping;

    // the game dll can add anything it wants after
    // this point in the structure
    int             clientNum;
};


struct edict_s {
    entity_state_t  s;
    struct gclient_s    *client;
    qboolean    inuse;
    int         linkcount;

    // FIXME: move these fields to a server private sv_entity_t
    list_t      area;               // linked to a division node or leaf

    int         num_clusters;       // if -1, use headnode instead
    int         clusternums[MAX_ENT_CLUSTERS];
    int         headnode;           // unused if num_clusters != -1
    int         areanum, areanum2;

    //================================

    int         svflags;            // SVF_NOCLIENT, SVF_DEADMONSTER, SVF_MONSTER, etc
    vec3_t      mins, maxs;
    vec3_t      absmin, absmax, size;
    solid_t     solid;
    int         clipmask;
    edict_t     *owner;

    // the game dll can add anything it wants after
    // this point in the structure
};

#endif      // GAME_INCLUDE

//===============================================================

//
// functions provided by the main engine
//
typedef struct {
    //---------------------------------------------------------------------
    // API Version.
    // 
    // The version numbers will always be equal to those that were set in 
    // CMake at the time of building the engine/game(dll/so) binaries.
    // 
    // In an ideal world, we comply to proper version releasing rules.
    // For Nail & Crescent, the general following rules apply:
    // --------------------------------------------------------------------
    // MAJOR: Ground breaking new features, you can expect anything to be 
    // incompatible at that.
    // 
    // MINOR : Everytime we have added a new feature, or if the API between
    // the Client / Server and belonging game counter-parts has actually 
    // changed.
    // 
    // POINT : Whenever changes have been made, and the above condition 
    // is not met.
    //---------------------------------------------------------------------
    struct {
        int32_t major;
        int32_t minor;
        int32_t point;
    } apiversion;

    // special messages
    void (* q_printf(2, 3) bprintf)(int printlevel, const char *fmt, ...);
    void (* q_printf(1, 2) dprintf)(const char *fmt, ...);
    void (* q_printf(3, 4) cprintf)(edict_t *ent, int printlevel, const char *fmt, ...);
    void (* q_printf(2, 3) centerprintf)(edict_t *ent, const char *fmt, ...);
    void (*sound)(edict_t *ent, int channel, int soundindex, float volume, float attenuation, float timeofs);
    void (*positioned_sound)(vec3_t origin, edict_t *ent, int channel, int soundinedex, float volume, float attenuation, float timeofs);

    // config strings hold all the index strings, the lightstyles,
    // and misc data like the sky definition and cdtrack.
    // All of the current configstrings are sent to clients when
    // they connect, and changes are sent to all connected clients.
    void (*configstring)(int num, const char *string);

    void (* q_noreturn q_printf(1, 2) error)(const char *fmt, ...);

    // the *index functions create configstrings and some internal server state
    int (*modelindex)(const char *name);
    int (*soundindex)(const char *name);
    int (*imageindex)(const char *name);

    void (*setmodel)(edict_t *ent, const char *name);

    // collision detection
    trace_t (* q_gameabi Trace)(const vec3_t &start, const vec3_t &mins, const vec3_t &maxs, const vec3_t &end, edict_t *passent, int contentmask);
    int (*PointContents)(const vec3_t &point);
    qboolean (*InPVS)(const vec3_t &p1, const vec3_t &p2);
    qboolean (*InPHS)(const vec3_t &p1, const vec3_t &p2);
    void (*SetAreaPortalState)(int portalnum, qboolean open);
    qboolean (*AreasConnected)(int area1, int area2);

    // an entity will never be sent to a client or used for collision
    // if it is not passed to linkentity.  If the size, position, or
    // solidity changes, it must be relinked.
    void (*linkentity)(edict_t *ent);
    void (*unlinkentity)(edict_t *ent);     // call before removing an interactive edict
    int (*BoxEdicts)(const vec3_t &mins, const vec3_t &maxs, edict_t **list, int maxcount, int areatype);
    
    //
    // N&C: We've moved the PMove functionality into sharedgame/pmove.c
    // by doing so, we now call into the server for finding the proper
    // pmoveparams_t to work in.
    //
    // Doing it this reversed method, allows for games to customize the pmove
    // code.
    //
    //void (*PMove)(pm_move_t *pmove);          // player movement code common with client prediction
    pmoveParams_t* (*GetPMoveParams) ();    // Used to call the shared PMove code with.

    // network messaging
    void (*Multicast)(const vec3_t *origin, multicast_t to);
    void (*Unicast)(edict_t *ent, qboolean reliable);
    void (*WriteChar)(int c);
    void (*WriteByte)(int c);
    void (*WriteShort)(int c);
    void (*WriteLong)(int c);
    void (*WriteFloat)(float f);
    void (*WriteString)(const char *s);
    void (*WritePosition)(const vec3_t &pos);    // some fractional bits
    void (*WriteDirection)(const vec3_t &pos);         // single byte encoded, very coarse
    void (*WriteAngle)(float f);

    // managed memory allocation
    void *(*TagMalloc)(size_t size, unsigned tag);
    void (*TagFree)(void *block);
    void (*FreeTags)(unsigned tag);

    // console variable interaction
    cvar_t *(*cvar)(const char *var_name, const char *value, int flags);
    cvar_t *(*cvar_set)(const char *var_name, const char *value);
    cvar_t *(*cvar_forceset)(const char *var_name, const char *value);

    // ClientCommand and ServerCommand parameter access
    int (*argc)(void);
    const char *(*argv)(int n);     // C++20: char*
    const char *(*args)(void);      // concatenation of all argv >= 1 // C++20: char*

    // N&C: Stuff Cmd.
    void (*stuffcmd) (edict_t* pent, const char* pszCommand); // C++20: STRING: Added const to char*
    
    // add commands to the server console as if they were typed in
    // for map changing, etc
    void (*AddCommandString)(const char *text);

    void (*DebugGraph)(float value, int color);
} svgame_import_t;

//
// functions exported by the game subsystem
//

typedef struct {
    struct edict_s  *edicts;
    int         edict_size;
    int         num_edicts;     // current number, <= max_edicts
    int         max_edicts;
} edict_pool_t;

typedef struct {
    //---------------------------------------------------------------------
    // API Version.
    // 
    // The version numbers will always be equal to those that were set in 
    // CMake at the time of building the engine/game(dll/so) binaries.
    // 
    // In an ideal world, we comply to proper version releasing rules.
    // For Nail & Crescent, the general following rules apply:
    // --------------------------------------------------------------------
    // MAJOR: Ground breaking new features, you can expect anything to be 
    // incompatible at that.
    // 
    // MINOR : Everytime we have added a new feature, or if the API between
    // the Client / Server and belonging game counter-parts has actually 
    // changed.
    // 
    // POINT : Whenever changes have been made, and the above condition 
    // is not met.
    //---------------------------------------------------------------------
    struct {
        int32_t major;
        int32_t minor;
        int32_t point;
    } apiversion;

    // the init function will only be called when a game starts,
    // not each time a level is loaded.  Persistant data for clients
    // and the server can be allocated in init
    void (*Init)(void);
    void (*Shutdown)(void);

    // each new level entered will cause a call to SpawnEntities
    void (*SpawnEntities)(const char *mapname, const char *entstring, const char *spawnpoint);

    // Read/Write Game is for storing persistant cross level information
    // about the world state and the clients.
    // WriteGame is called every time a level is exited.
    // ReadGame is called on a loadgame.
    void (*WriteGame)(const char *filename, qboolean autosave);
    void (*ReadGame)(const char *filename);

    // ReadLevel is called after the default map information has been
    // loaded with SpawnEntities
    void (*WriteLevel)(const char *filename);
    void (*ReadLevel)(const char *filename);

    qboolean (*ClientConnect)(edict_t *ent, char *userinfo);
    void (*ClientBegin)(edict_t *ent);
    void (*ClientUserinfoChanged)(edict_t *ent, char *userinfo);
    void (*ClientDisconnect)(edict_t *ent);
    void (*ClientCommand)(edict_t *ent);
    void (*ClientThink)(edict_t *ent, usercmd_t *cmd);

    void (*RunFrame)(void);

    //
    // N&C: Our custom PMove requires that the server calls into the SVGame
    // module for initializing pmove parameters.
    //
    void (*PMoveInit) (pmoveParams_t* pmp);
    void (*PMoveEnableQW) (pmoveParams_t* pmp);

    // ServerCommand will be called when an "sv <command>" command is issued on the
    // server console.
    // The game can issue gi.argc() / gi.argv() commands to get the rest
    // of the parameters
    void (*ServerCommand)(void);

    //
    // global variables shared between game and server
    //

    // The edict array is allocated in the game dll so it
    // can vary in size from one game to another.
    //
    // The size will be fixed when ge->Init() is called
//    edict_pool_t pool;

    struct edict_s  *edicts;
    int         edict_size;
    int         num_edicts;     // current number, <= max_edicts
    int         max_edicts;
} svgame_export_t;

#endif // GAME_H