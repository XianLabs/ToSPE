#include "Hooks.hpp"
#include "HWID.hpp"
#include "Security.hpp"

	
extern DWORD dwSendClass;
extern int Main();

DWORD ptrToNothing = 0x01524553;

VOID Startup()
{
	Sleep(500);
	
	// HP-1824836455, D3-1748875454, Chronoless-77161068, Chronoless--14201972, Zhiwei--1431239405, Chakong--1095819322
	//int NumHWIDS = 1;

	//char HWID[19] = { 0x70, 0x5b, 0x52, 0x58, 0x5c, 0x5d, 0x54, 0x1e, 0x1e, 0x02, 0x03, 0x0a, 0x06, 0x0b, 0x02, 0x0a, 0x00, 0x01, 0x01 };

	//for (int i = 0; i < strlen(HWID); i++)
	//{
	//	//memcpy(HWIDS[i], chakong_hwid, 19);
	//	printf("%c", (HWID[i] ^ 0x33));
	//}




	//std::string hwid = GenerateHWID();
	//if (hwid.compare("SF Heavy Equipment-1284220259") != 0) 
	//{
	//	__asm
	//	{
	//		mov eax, ptrToNothing
	//		call dword ptr [eax+10]
	//		_emit 0CCh
	//		_emit 0CCh
	//		_emit 0CCh
	//		ret
	//		mov ecx, 0x01A0B100
	//		mov edx, 0x01A0B2F0
	//		rep ret
	//		sub esp, 0x28
	//		ret
	//	}
	//}
	
	__asm
	{
		pushad
		push eax
		xor eax,eax
		jz execute
		and esp, 0x00FFFFFF
		jmp $+3
	execute:
		pop eax
		popad
	}

	DWORD dwOld; //get rid of gay NtdllPVM hook
	DWORD ntdll_ZwProtectVirtualMemory = (DWORD)GetProcAddress(GetModuleHandle(L"ntdll"), "ZwProtectVirtualMemory");
	VirtualProtect((void*)ntdll_ZwProtectVirtualMemory, 5, PAGE_EXECUTE_WRITECOPY, &dwOld);
	memcpy((void*)ntdll_ZwProtectVirtualMemory, (void*) "\xB8\x50\x00\x00\x00", 5);
	
	dwSendClass = FindPattern((BYTE*)SendClassPattern, "xx???x???xxxxxxxxxxxxxxxxxxx", 6, 0x00400000);
	dwSendClass = *(DWORD*)dwSendClass;
	
	WriteHookBytes(hkNPCList1, HookNPCList1, TRUE); //NPC/Dungeon botting
	WriteHookBytes(hkConnect, HookConnect, TRUE); //Auto-login
	
	SetWindowTextA(FindWindowA(NULL, "Tree Of Savior"), "TOS");
	Main(); //GUI
}


BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason) {
		case DLL_PROCESS_ATTACH: {

		__asm
		{
			push eax
			xor eax,eax
			jz execute
			and esp, 0x00FFFFFF
			jmp $+3
		execute:
			pop eax
		}

			 CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&Startup, 0, 0, 0);
		}break;

		case DLL_PROCESS_DETACH:
			break;
	};
	
	return TRUE;
}