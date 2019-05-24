#include "LuaHelper.hpp"
#include <fstream>
#include <vector>
//global LUA ptr
lua_State* L;
extern HANDLE ScriptThread;
extern DWORD NPCUniqueID;

void init_lua()
{
	L = luaL_newstate();
	luaL_openlibs(L);

	lua_register(L, "MessageBox", LUA_MessageBox);
	lua_register(L, "Sleep", LUA_Sleep);
	lua_register(L, "SendPacket", LUA_SendPacket);
	lua_register(L, "EnterDungeon", LUA_EnterDungeon);
	lua_register(L, "Teleport", LUA_Teleport);
	lua_register(L, "UseSkill", LUA_UseSkill);
}

void execute_lua(char* stream)
{
	init_lua();

	std::ofstream tempFile;
	tempFile.open("C:\\tempScript.lua");
	tempFile << stream;
	tempFile.close();

	if (stream == NULL || strcmp(stream, "Enter script here...") == 0 || strcmp(stream, "") == 0)
	{
		MessageBoxA(0, "No script entered or found.", "TOS Scripter", 0);
		return;
	}	
	else
		luaL_dofile(L, "C:\\tempScript.lua"); //execute LUA script
}

void end_lua()
{
	lua_close(L);
	TerminateThread(ScriptThread, 0);
}

//LUA Custom Functions
static int LUA_MessageBox(LUA_PTR)
{
	const char* str = lua_tostring(L, 1); // get function argument
	MessageBoxA(0, str, "TOS Scripter", 0);
	return 0;
}

int LUA_Sleep(LUA_PTR)
{
	int n = lua_gettop(L);
	int delay = lua_tonumber(L, 1);
	Sleep(delay);
	return 0;
}


static int LUA_SendPacket(LUA_PTR)
{
	const char* str = lua_tostring(L, 1);
	BYTE* hexbytes = convertStringToPacketFormat((char*)str);
	DWORD length = getPacketLength((char*)str);
	SendPacket(hexbytes, length);
	return 0;
}

static int LUA_EnterDungeon(LUA_PTR)
{
	BYTE packet[] = {0x5E, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	memcpy(&packet[10], &NPCUniqueID, sizeof(DWORD));
	SendPacket(&packet[0], 15);
	return 0;
}

static int LUA_Teleport(LUA_PTR)
{
	int X = lua_tonumber(L, 1);
	int Y = lua_tonumber(L, 2);
	int Z = lua_tonumber(L, 3);

	byte Packet[] = "\x54\x0e\x54\x0E\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	memcpy(&Packet[14], &X, sizeof(char) * 4);
	memcpy(&Packet[18], &Y, sizeof(char) * 4);
	memcpy(&Packet[22], &Z, sizeof(char) * 4);

	SendPacket(Packet, 26);
	
	return 0;
}

static int LUA_UseSkill(LUA_PTR)
{
	short Skill = lua_tointeger(L, 1);
	
	PacketWriter* p = new PacketWriter();
	p->Write<SHORT>(SkillOpcode); //opcode
	p->Write<UINT64>(0);
	p->Write<BYTE>(0);
	p->Write<USHORT>(Skill);
	p->Write<UINT64>(0);
	p->Write<SHORT>(0);
	SendPacket((LPBYTE)p->GetBuffer(), p->GetSize());
	delete p;

	return 0;
}