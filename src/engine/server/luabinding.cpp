#include <lua.hpp>

#include <engine/server.h>
#include <engine/console.h>
#include <engine/storage.h>
#include <game/server/gamecontext.h>
#include <game/client/gameclient.h>

#include "luabinding.h"
#include "lua.h"


int CLuaBinding::LuaListdirCallback(const char *name, int is_dir, int dir_type, void *user)
{
	LuaListdirCallbackParams *params = (LuaListdirCallbackParams*)user;
	lua_State *L = params->L;

	lua_getglobal(L, params->aCallbackFunc);
	lua_pushstring(L, name);
	lua_pushboolean(L, is_dir);
	lua_pcall(L, 2, 1, 0);
	int ret = 0;
	if(lua_isnumber(L, -1))
		ret = round_to_int((float)lua_tonumber(L, -1));
	lua_pop(L, 1); // pop return
	return ret;
}

// global namespace
int CLuaBinding::LuaListdir(lua_State *L)
{
	int nargs = lua_gettop(L);
	if(nargs != 2)
		return luaL_error(L, "Listdir expects 2 arguments");

	argcheck(lua_isstring(L, 1), 1, "string"); // path
	argcheck(lua_isstring(L, 2), 2, "function name (as a string)"); // function callback

	lua_getglobal(L, lua_tostring(L, 2)); // check if the given callback function actually exists
	argcheck(lua_isfunction(L, -1), 2, "function name (as a string)");
	lua_pop(L, 1); // pop temporary lua function

	const char *pDir = lua_tostring(L, 1); // arg1
	LuaListdirCallbackParams params(L, lua_tostring(L, 2)); // arg2
	lua_pop(L, 1); // pop arg2
	lua_Number ret = (lua_Number)fs_listdir(pDir, LuaListdirCallback, IStorage::TYPE_ALL, &params);
	lua_pop(L, 1); // pop arg1
	lua_pushnumber(L, ret);
	return 1;
}

int CLuaBinding::LuaStrIsNetAddr(lua_State *L)
{
	int nargs = lua_gettop(L);
	if(nargs != 1)
		return luaL_error(L, "StrIsNetAddr expects 1 argument");

	argcheck(lua_isstring(L, 1), 1, "string");

	NETADDR temp;
	int ret = net_addr_from_str(&temp, lua_tostring(L, 1)); // arg1
	lua_pop(L, 1), // pop arg1

	lua_pushboolean(L, ret == 0);
	return 1;
}


int CLuaBinding::LuaPrintOverride(lua_State *L)
{
	int nargs = lua_gettop(L);
	if(nargs < 1)
		return luaL_error(L, "print expects 1 argument or more");

	// construct the message from all arguments
	char aMsg[512] = {0};
	for(int i = 1; i <= nargs; i++)
	{
		argcheck(lua_isstring(L, i) || lua_isnumber(L, i), i, "string or number");
		str_append(aMsg, lua_tostring(L, i), sizeof(aMsg));
		str_append(aMsg, "    ", sizeof(aMsg));
	}
	aMsg[str_length(aMsg)-1] = '\0'; // remove the last tab character

	// pop all to clean up the stack
    lua_pop(L, nargs);

	CGameContext *pGameServer = CLua::ms_pCGameServer;
	pGameServer->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "LUA|GAMETYPE", aMsg);
	//dbg_msg("LUA|GAMETYPE", "%s", aMsg);

	return 0;
}
