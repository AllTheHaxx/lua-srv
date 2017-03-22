#include "lua.h"

#include <base/system.h>
#include <base/vmath.h>

#include <engine/server.h>

#include <engine/shared/config.h>
#include <engine/shared/protocol.h>

#include <game/server/player.h>
#include <game/server/gamecontext.h>

#include <game/server/entities/character.h>

#include "luabinding.h"


void CLua::RegisterLuaBindings()
{
	lua_State *L = GetLuaState();

	// kill everything malicious
	static const char s_aBlacklist[][64] = {
			"os.exit",
			"os.execute",
			"os.rename",
			"os.remove",
			"os.setlocal",
			"dofile",
			"require",
			"load",
			"loadfile",
			"loadstring",
	};
	for(unsigned i = 0; i < sizeof(s_aBlacklist)/sizeof(s_aBlacklist[0]); i++)
	{
		char aCmd[128];
		str_format(aCmd, sizeof(aCmd), "%s=nil", s_aBlacklist[i]);
		luaL_dostring(L, aCmd);
		if(g_Config.m_Debug)
			dbg_msg("lua", "disable: '%s'", aCmd);
	}

	// register everything
	lua_register(L, "print", CLuaBinding::LuaPrintOverride);
	lua_register(L, "Listdir", CLuaBinding::LuaListdir); // TODO: get rid of me

	getGlobalNamespace(L)
		// global types
		.beginClass< vector2_base<float> >("vec2")
			.addConstructor <void (*) (float)> ()
			.addConstructor <void (*) (float, float)> ()
			.addFunction("__add", &vector2_base<float>::operator+)
			.addFunction("__sub", &vector2_base<float>::operator-)
			.addFunction("__mul", &vector2_base<float>::operator*)
			.addFunction("__div", &vector2_base<float>::operator/)
			.addFunction("__eq", &vector2_base<float>::operator==)
			.addData("x", &vector2_base<float>::x)
			.addData("y", &vector2_base<float>::y)
		.endClass()
		.beginClass< vector3_base<float> >("vec3")
			.addConstructor <void (*) (float)> ()
			.addConstructor <void (*) (float, float, float)> ()
			.addFunction("__add", &vector3_base<float>::operator+)
			.addFunction("__sub", &vector3_base<float>::operator-)
			.addFunction("__mul", &vector3_base<float>::operator*)
			.addFunction("__div", &vector3_base<float>::operator/)
			.addFunction("__eq", &vector3_base<float>::operator==)
			.addData("x", &vector3_base<float>::x)
			.addData("y", &vector3_base<float>::y)
			.addData("z", &vector3_base<float>::z)
		.endClass()
		.beginClass< vector4_base<float> >("vec4")
			.addConstructor <void (*) (float)> ()
			.addConstructor <void (*) (float, float, float, float)> ()
			.addData("r", &vector4_base<float>::r)
			.addData("g", &vector4_base<float>::g)
			.addData("b", &vector4_base<float>::b)
			.addData("a", &vector4_base<float>::a)
		.endClass()

		.beginClass< vector2_base<int> >("vec2i")
			.addConstructor <void (*) (int, int)> ()
			.addFunction("__add", &vector2_base<int>::operator+)
			.addFunction("__sub", &vector2_base<int>::operator-)
			.addFunction("__mul", &vector2_base<int>::operator*)
			.addFunction("__div", &vector2_base<int>::operator/)
			.addFunction("__eq", &vector2_base<int>::operator==)
			.addData("x", &vector2_base<int>::x)
			.addData("y", &vector2_base<int>::y)
		.endClass()
		.beginClass< vector3_base<int> >("vec3i")
			.addConstructor <void (*) (int, int, int)> ()
			.addFunction("__add", &vector3_base<int>::operator+)
			.addFunction("__sub", &vector3_base<int>::operator-)
			.addFunction("__mul", &vector3_base<int>::operator*)
			.addFunction("__div", &vector3_base<int>::operator/)
			.addFunction("__eq", &vector3_base<int>::operator==)
			.addData("x", &vector3_base<int>::x)
			.addData("y", &vector3_base<int>::y)
			.addData("z", &vector3_base<int>::z)
		.endClass()
		.beginClass< vector4_base<int> >("vec4i")
			.addConstructor <void (*) (int, int, int, int)> ()
			.addData("r", &vector4_base<int>::r)
			.addData("g", &vector4_base<int>::g)
			.addData("b", &vector4_base<int>::b)
			.addData("a", &vector4_base<int>::a)
		.endClass()

		// Game:Players(ID).Character
		.beginClass<CCharacter>("CCharacter")
			// functions
			.addFunction("Die", &CCharacter::Die)
			.addFunction("TakeDamage", &CCharacter::TakeDamage)
			.addFunction("IncreaseHealth", &CCharacter::IncreaseHealth)
			.addFunction("IncreaseArmor", &CCharacter::IncreaseArmor)
			.addFunction("GiveWeapon", &CCharacter::GiveWeapon)
			.addFunction("GiveNinja", &CCharacter::GiveNinja)
			.addFunction("SetEmote", &CCharacter::SetEmote)

			// attributes
			.addProperty("ActiveWeapon", &CCharacter::GetWeapon, &CCharacter::SetWeapon)
			//.addProperty("IsGrounded", CCharacter::IsGrounded) // not sure why this won't work
			.addProperty("IsAlive", &CCharacter::IsAlive)
		.endClass()

		// Game:Players(ID)
		.beginClass<CPlayer>("CPlayer")
			// functions
			.addFunction("Respawn", &CPlayer::Respawn)
			.addFunction("SetTeam", &CPlayer::SetTeam)
			.addFunction("KillCharacter", &CPlayer::KillCharacter)

			// attributes
			.addProperty("Team", &CPlayer::GetTeam)
			.addData("PlayerFlags", &CPlayer::m_PlayerFlags, true)
			.addData("SpectatorID", &CPlayer::m_SpectatorID, true)
			.addData("RespawnTick", &CPlayer::m_RespawnTick, false)
			.addData("DieTick", &CPlayer::m_DieTick, false)
			.addData("Score", &CPlayer::m_Score, true)
			.addData("ScoreStartTick", &CPlayer::m_ScoreStartTick, false)
			.addData("ForceBalanced", &CPlayer::m_ForceBalanced, false)
			.addData("LastActionTick", &CPlayer::m_LastActionTick, false)
			.addData("TeamChangeTick", &CPlayer::m_TeamChangeTick, false)
			// TODO: add the m_TeeInfos struct (probably really easy :D) (yet better add all the structs.)
			.addData("Character", &CPlayer::m_pCharacter) // cleaner as variable instead of a getter function I think
		.endClass()

		// TODO: PUT  STUFF  HERE
		// Game.Server
		.beginClass<IServer>("IServer")
			.addProperty("Tick", &IServer::Tick)
			.addProperty("TickSpeed", &IServer::TickSpeed)

			.addFunction("IsAuthed", &IServer::IsAuthed)
			.addFunction("Kick", &IServer::Kick)

			// CGameContext stuff (makes sense in Game.Server IMO)
			.addFunction("StartVote", &CGameContext::StartVote)
			.addFunction("EndVote", &CGameContext::EndVote)

			.addFunction("CreateDamageInd", &CGameContext::CreateDamageInd)
			.addFunction("CreateExplosion", &CGameContext::CreateExplosion)
			.addFunction("CreateHammerHit", &CGameContext::CreateHammerHit)
			.addFunction("CreatePlayerSpawn", &CGameContext::CreatePlayerSpawn)
			.addFunction("CreateDeath", &CGameContext::CreateDeath)
			.addFunction("CreateSound", &CGameContext::CreateSound)
			.addFunction("CreateSoundGlobal", &CGameContext::CreateSoundGlobal)

			.addFunction("SendChatTarget", &CGameContext::SendChatTarget)
			.addFunction("SendChat", &CGameContext::SendChat)
			.addFunction("SendEmoticon", &CGameContext::SendEmoticon)
			.addFunction("SendWeaponPickup", &CGameContext::SendWeaponPickup)
			.addFunction("SendBroadcast", &CGameContext::SendBroadcast)

			.addFunction("SwapTeams", &CGameContext::SwapTeams)
		.endClass()

		.beginNamespace("Game")
			.addVariable("Server", &CLua::m_pServer, false)
			.addFunction("Players", &CGameContext::GetPlayer)
		.endNamespace()

		// g_Config stuff... EVERYTHING AT ONCE!
		/// Config.<var_name>
#define MACRO_CONFIG_STR(Name,ScriptName,Len,Def,Save,Desc) \
			.addStaticProperty(#Name, &CConfigProperties::GetConfig_##Name, &CConfigProperties::SetConfig_##Name) \
			.addStaticProperty(#ScriptName, &CConfigProperties::GetConfig_##Name, &CConfigProperties::SetConfig_##Name)

#define MACRO_CONFIG_INT(Name,ScriptName,Def,Min,Max,Save,Desc) \
			.addStaticProperty(#Name, &CConfigProperties::GetConfig_##Name, &CConfigProperties::SetConfig_##Name) \
			.addStaticProperty(#ScriptName, &CConfigProperties::GetConfig_##Name, &CConfigProperties::SetConfig_##Name)

		.beginClass<CConfigProperties>("Config")
			#include <engine/shared/config_variables.h>
		.endClass()

#undef MACRO_CONFIG_STR
#undef MACRO_CONFIG_INT

		// OOP ENDS HERE
	;

	//if(g_Config.m_Debug)
		dbg_msg("lua/debug", "registering lua bindings complete (L=%p)", L);

}
