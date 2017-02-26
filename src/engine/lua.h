#ifndef ENGINE_LUA_H
#define ENGINE_LUA_H
#include "kernel.h"

#define LUA_GAMETYPE_FOLDER "gametypes"

class ILua : public IInterface
{
	MACRO_INTERFACE("lua", 0)
protected:
	class lua_State *m_pLuaState;

public:
	virtual class lua_State *GetLuaState() { return m_pLuaState; }

	virtual void Init() = 0;
	virtual void StartupLua() = 0;
	virtual bool LoadGametype(const char *pGameType) = 0;
	virtual bool GametypeExists(const char *pGameType) = 0;
	virtual const char *GetError(int i) = 0;
	virtual int NumErrors() const = 0;

};

extern ILua *CreateLua();

#endif
