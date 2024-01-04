#include "stdafx.h"

#include "main.h"
#include <thread>
MODULEINFO g_MainModuleInfo = { 0 };

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {

		if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &g_MainModuleInfo, sizeof(g_MainModuleInfo))) {
		}
		scriptRegister(hModule, RunUnreliable);

		//NoIntro();
		//SpawnScriptHook();
		//BypassOnlineModelRequestBlock();
	}

	return TRUE;
}

