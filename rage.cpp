#include "stdafx.h"

DWORD64 dwRegistrationTablePtr = 0;
DWORD64 dwThreadCollectionPtr = 0;

//NativeRegistration** GetRegistrationTable()
//{
//	if (!dwRegistrationTablePtr) {
//		dwRegistrationTablePtr = Pattern::Scan(g_MainModuleInfo, "76 61 49 8B 7A 40 48 8D 0D");
//
//		if (!dwRegistrationTablePtr) {
//		}
//
//		dwRegistrationTablePtr += 6;
//
//		DWORD64 dwAddressOfRegistrationTable = dwRegistrationTablePtr + *(DWORD*)(dwRegistrationTablePtr + 3) + 7;
//
//		if (!dwAddressOfRegistrationTable ||
//			dwAddressOfRegistrationTable < (DWORD64)g_MainModuleInfo.lpBaseOfDll ||
//			dwAddressOfRegistrationTable >(DWORD64) g_MainModuleInfo.lpBaseOfDll + g_MainModuleInfo.SizeOfImage) {
//		}
//
//		dwRegistrationTablePtr = dwAddressOfRegistrationTable;
//	}
//
//	return (NativeRegistration**)dwRegistrationTablePtr;
//}
//
//NativeHandler GetNativeHandler(UINT64 hash)
//{
//	NativeRegistration** registrationTable = GetRegistrationTable();
//
//	if (!registrationTable)
//		return nullptr;
//
//	NativeRegistration* table = registrationTable[hash & 0xFF];
//
//	for (; table; table = table->nextRegistration)
//	{
//		for (UINT32 i = 0; i < table->numEntries; i++)
//		{
//			if (hash == table->hashes[i])
//			{
//				return table->handlers[i];
//			}
//		}
//	}
//
//	return nullptr;
//}
//
//void NativeInvoke::Invoke(NativeContext *cxt, UINT64 hash)
//{
//	auto fn = GetNativeHandler(hash);
//
//	if (fn != 0)
//	{
//		fn(cxt);
//	}
//}
//
//rage::pgPtrCollection<GtaThread>* GetGtaThreadCollection(BlipList*& pBlipList)
//{
//	if (!dwThreadCollectionPtr) {
//		dwThreadCollectionPtr = Pattern::Scan(g_MainModuleInfo, "48 8B 05 ? ? ? ? 8B CA 4C 8B 0C C8 45 39 51 08");
//
//		if (!dwThreadCollectionPtr) {
//		}
//
//		DWORD64 dwAddressOfThreadCollection = dwThreadCollectionPtr + *(DWORD*)(dwThreadCollectionPtr + 3) + 7;
//
//		if (!dwAddressOfThreadCollection ||
//			dwAddressOfThreadCollection < (DWORD64)g_MainModuleInfo.lpBaseOfDll ||
//			dwAddressOfThreadCollection >(DWORD64) g_MainModuleInfo.lpBaseOfDll + g_MainModuleInfo.SizeOfImage) {
//		}
//
//		dwThreadCollectionPtr = dwAddressOfThreadCollection;
//
//		DWORD64 blipCollectionSignature = Pattern::Scan(g_MainModuleInfo, "4C 8D 05 ? ? ? ? 0F B7 C1");
//
//		pBlipList = (BlipList*)(blipCollectionSignature + *(DWORD*)(blipCollectionSignature + 3) + 7);
//	}
//
//	return (rage::pgPtrCollection<GtaThread>*) dwThreadCollectionPtr;
//}
//
//rage::scrThread* GetActiveThread()
//{
//	char* moduleTls = *(char**)__readgsqword(88);
//
//	return *reinterpret_cast<rage::scrThread**>(moduleTls + 2096);
//}
//
//void SetActiveThread(rage::scrThread* thread)
//{
//	char* moduleTls = *(char**)__readgsqword(88);
//
//	*reinterpret_cast<rage::scrThread**>(moduleTls + 2096) = thread;
//}
//
//void BypassOnlineModelRequestBlock()
//{
//	DWORD64 dwGetModelTableFunctionAddress = Pattern::Scan(g_MainModuleInfo, "48 8B C8 FF 52 30 84 C0 74 05 48");
//	if (dwGetModelTableFunctionAddress != NULL)
//		*(unsigned short*)(dwGetModelTableFunctionAddress + 0x8) = 0x9090;
//	else
//	{
//		#ifndef __DEBUG
//		#else
//		#endif
//	}
//}
//
//void NoIntro()
//{
//	DWORD64 dwSplashScreen = Pattern::Scan(g_MainModuleInfo, "72 1F E8 ? ? ? ? 8B 0D");
//	if (dwSplashScreen == NULL)
//	{
//		while (dwSplashScreen == NULL)
//		{
//			Sleep(10);
//			dwSplashScreen = Pattern::Scan(g_MainModuleInfo, "72 1F E8 ? ? ? ? 8B 0D");
//		}
//
//		if (dwSplashScreen != NULL)
//			*(unsigned short*)(dwSplashScreen) = 0x9090;
//
//		DWORD64 dwRockStarLogo = Pattern::Scan(g_MainModuleInfo, "70 6C 61 74 66 6F 72 6D 3A");
//		int iCounter = 0;
//		while (dwRockStarLogo == NULL)
//		{
//			Sleep(10);
//			dwRockStarLogo = Pattern::Scan(g_MainModuleInfo, "70 6C 61 74 66 6F 72 6D 3A");
//		}
//
//		if (dwRockStarLogo != NULL)
//			*(unsigned char*)(dwRockStarLogo) = 0x71;
//
//		Sleep(15000);
//		*(unsigned char*)(dwRockStarLogo) = 0x70;
//		*(unsigned short*)(dwSplashScreen) = 0x1F72;
//	}
//}