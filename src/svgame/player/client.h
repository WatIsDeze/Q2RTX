// LICENSE HERE.

//
// svgame/player/client.h
//
// N&C SVGame: Client Header
// 
//
#ifndef __SVGAME_PLAYER_CLIENT_H__
#define __SVGAME_PLAYER_CLIENT_H__

qboolean SVG_ClientConnect(Entity* ent, char* userinfo);
void SVG_ClientDisconnect(Entity* ent);

void SVG_PutClientInServer(Entity* ent);
void SVG_InitClientPersistant(GameClient* client);
void SVG_InitClientResp(GameClient* client);
void SVG_HUD_BeginIntermission(Entity* targ);

// Respawns the actual client.
void SVG_RespawnClient(Entity* ent);

// Tosses the client weapon.
void SVG_TossClientWeapon(Entity* self);

// Updates the client obituary.
void SVG_ClientUpdateObituary(Entity* self, Entity* inflictor, Entity* attacker);
void SVG_TossClientWeapon(Entity* self);

void SVG_ClientBegin(Entity* ent);
void SVG_ClientCommand(Entity* ent);
void SVG_ClientUserinfoChanged(Entity* ent, char* userinfo);

void SVG_ClientBeginServerFrame(Entity* ent);
void SVG_ClientThink(Entity* ent, ClientUserCommand* cmd);

#endif // __SVGAME_PLAYER_CLIENT_H__