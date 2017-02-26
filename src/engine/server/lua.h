#ifndef ENGINE_SERVER_LUA_H
#define ENGINE_SERVER_LUA_H

#include <exception>
#include <vector>
#include <string>

#include <engine/lua.h>

#include <lua.hpp>
#include <engine/external/luabridge/LuaBridge.h>

using namespace luabridge;

class IServer;
class CServer;
class CGameContext;


class CLua : public ILua
{
	class IStorage *m_pStorage;
	class IConsole *m_pConsole;

	static CLua *ms_pStaticLua;

public:
//	static class IServer *ms_pServer;
//	static class CServer *ms_pCServer;
//	static class IGameServer *ms_pGameServer;
	static class CGameContext *ms_pCGameServer;

private:
	std::vector<std::string> m_lErrors;

protected:
	class IStorage *Storage() { return m_pStorage; }
	class IConsole *Console() { return m_pConsole; }

public:
	CLua();

	virtual void Init();
	virtual void StartupLua();
	virtual bool LoadGametype(const char *pGameType);
	virtual bool GametypeExists(const char *pGameType);
	virtual const char *GetError(int i);
	virtual int NumErrors() const { return (int)m_lErrors.size(); }

	bool LoadScript(const char *pPath);

	static int ListdirLoadCallback(const char *pName, const char *pFullPath, int is_dir, int dir_type, void *pUser);
	static int ListdirTestCallback(const char *pName, int is_dir, int dir_type, void *pUser);

	static void SetStaticVars(IServer *pServer, CGameContext *pGameServer);

	static int HandleException(std::exception& e);
	static int HandleException(const char *pError);
	static int ErrorFunc(lua_State *L);
	static int Panic(lua_State *L);

private:
//	int LoadFolderHelper(const char *pFolder);

	void RegisterLuaBindings();

	// helper stuff
	struct CListdirLoadHelper
	{
		CLua *m_pSelf;
		unsigned m_NumFiles;

		CListdirLoadHelper() { mem_zero(this, sizeof(CListdirLoadHelper)); }
	};

	struct CListdirTestHelper
	{
		const char *m_pSearch;
		bool m_Found;

		CListdirTestHelper() { mem_zero(this, sizeof(CListdirTestHelper)); }
	};
};

#endif
