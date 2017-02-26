/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <lua.hpp>
#include <engine/external/luabridge/LuaBridge.h>

#include <engine/shared/config.h>
#include <game/generated/protocol.h>
#include <engine/server.h>
#include <engine/lua.h>

#include <game/server/gamecontext.h>
#include "mod.h"


CGameControllerLuaMod::CGameControllerLuaMod(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
	m_pGameType = g_Config.m_SvGametype; // XXX this can be set by the gametype itself dynamically later
	m_GameFlags = GAMEFLAG_TEAMS; // GAMEFLAG_TEAMS makes it a two-team gamemode
}

void CGameControllerLuaMod::Tick()
{
	// this is the main part of the gamemode, this function is run every tick

	//IGameController::Tick(); TODO: register this to lua
	//LUA_FIRE_EVENT("Tick");
}

lua_State *CGameControllerLuaMod::L()
{
	return GameServer()->Lua()->GetLuaState();
}

// TODO add all and stuff
