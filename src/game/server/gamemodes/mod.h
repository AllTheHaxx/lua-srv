/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMEMODES_MOD_H
#define GAME_SERVER_GAMEMODES_MOD_H
#include <game/server/gamecontroller.h>


class CGameControllerLuaMod : public IGameController
{
protected:
	class lua_State *L();

public:
	CGameControllerLuaMod(class CGameContext *pGameServer);

	virtual void Tick();
};
#endif
