#include <windows.h>
#include "../Common/common.h"
#include "../Common/Console.h"
#include "../Common/detours/detours.h"
#include "../Common/GetCommonDirectory.h"
#include "../Common/shared_io.h"
#include "../Common/ConfigFile.h"
#include "../Common/Silkroad.h"
#include <iphlpapi.h>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include "Analyzer.h"

//-----------------------------------------------------------------------------

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")

//-----------------------------------------------------------------------------

struct ExeSectionInfo
{
	DWORD codeStart;
	LPBYTE codePtr;
	DWORD codeSize;

	DWORD dataStart;
	LPBYTE dataPtr;
	DWORD dataSize;
};

HINSTANCE globalInstance = NULL;
HANDLE globalThread = NULL;
ExeSectionInfo globalExeSectionInfo = {0};

BYTE peHeader[4096];

DWORD & codeStart = globalExeSectionInfo.codeStart;
DWORD & codeSize = globalExeSectionInfo.codeSize;
LPBYTE & codePtr = globalExeSectionInfo.codePtr;

DWORD & dataStart = globalExeSectionInfo.dataStart;
DWORD & dataSize = globalExeSectionInfo.dataSize;
LPBYTE & dataPtr = globalExeSectionInfo.dataPtr;

//-----------------------------------------------------------------------------

void User_Initialize();
void User_Deinitialize();

DWORD WINAPI InjectionThread(LPVOID lpParam);

//-----------------------------------------------------------------------------

HMODULE WINAPI Detoured()
{
	return globalInstance;
}

//-----------------------------------------------------------------------------

namespace nsFramework
{
	LPVOID framework_gCleanupAddr = NULL;
	DWORD framework_entryPoint = 0;
	BYTE framework_resBytes[6] = {0};
	DWORD framework_ret_InitCleanup = 0;

	void Framework_Initialize()
	{
		User_Initialize();
	}

	void Framework_Deinitialize()
	{
		User_Deinitialize();
	}

	VOID framework_InitCleanup()
	{
		VirtualFreeEx(GetCurrentProcess(), framework_gCleanupAddr, 0, MEM_RELEASE);
		framework_gCleanupAddr = 0;
		WriteBytes(framework_entryPoint, framework_resBytes, 6);
		framework_ret_InitCleanup -= 5;
		Framework_Initialize();
	}

	__declspec(naked) VOID framework_codecave_initcleanup(VOID)
	{
		__asm pop framework_ret_InitCleanup
		__asm pushad
		framework_InitCleanup();
		__asm popad
		__asm push framework_ret_InitCleanup
		__asm ret
	}

	extern "C" __declspec(dllexport) VOID __cdecl Initialize(LPDWORD allocAddr, DWORD restoreAddr, LPBYTE origBytes)
	{
		globalThread = OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId());
		framework_gCleanupAddr = allocAddr;
		framework_entryPoint = restoreAddr;
		WriteBytes(framework_entryPoint, origBytes, 6);
		memcpy(framework_resBytes, origBytes, 6);
		CreateCodeCave(framework_entryPoint, 6, framework_codecave_initcleanup);
	}
}

//-----------------------------------------------------------------------------

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
{
	if(ulReason == DLL_PROCESS_ATTACH)
	{
		globalInstance = hModule;
	}
	else if(ulReason == DLL_PROCESS_DETACH)
	{
		nsFramework::Framework_Deinitialize();
	}
	return TRUE;
}

//-----------------------------------------------------------------------------

namespace nsDetours
{
	LONG inject = 1;

	extern "C" BOOL (WINAPI * Real_QueryPerformanceCounter)(LARGE_INTEGER *lpPerformanceCount) = QueryPerformanceCounter;
	BOOL WINAPI User_QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount)
	{
		BOOL result = Real_QueryPerformanceCounter(lpPerformanceCount);
		if(InterlockedCompareExchange(&inject, 0, 1) == 1)
		{
			InjectionThread(0);
		}
		return result;
	}

	extern "C" HANDLE (WINAPI * Real_CreateMutexA)(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName) = CreateMutexA;
	HANDLE WINAPI User_CreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName)
	{
		if(lpName && strcmp(lpName, "Silkroad Client") == 0)
		{
			char newName[128] = {0};
			_snprintf_s(newName, sizeof(newName), sizeof(newName) / sizeof(newName[0]) - 1, "Client_%d", 0xFFFFFFFF & __rdtsc());
			return Real_CreateMutexA(lpMutexAttributes, bInitialOwner, newName);
		}
		return Real_CreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
	}

	extern "C" int (WINAPI * Real_bind)(SOCKET s, const struct sockaddr *name, int namelen) = bind;
	int WINAPI User_bind(SOCKET s, const struct sockaddr *name, int namelen)
	{
		if(name && namelen == 16)
		{
			sockaddr_in * inaddr = (sockaddr_in *)name;
			if(inaddr->sin_port == ntohs(15779))
			{
				return 0;
			}
		}
		return Real_bind(s, name, namelen);
	}

	extern "C" DWORD (WINAPI * Real_GetAdaptersInfo)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen) = GetAdaptersInfo;
	DWORD WINAPI User_GetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen)
	{
		DWORD dwResult = Real_GetAdaptersInfo(pAdapterInfo, pOutBufLen);
		if(dwResult == ERROR_SUCCESS)
		{
			PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
			srand(__rdtsc() & 0xFFFFFFFF);
			while(pAdapter)
			{
				for(UINT i = 1; i < pAdapter->AddressLength; i++)
				{
					printf("%.2X -> ", pAdapter->Address[i]);
					pAdapter->Address[i] = rand() % 256;
					printf("%.2X ", pAdapter->Address[i]);
				}
				printf("\n");
				pAdapter = pAdapter->Next;
			}
		}
		return dwResult;
	}

	// How many routes we have to work with
	size_t routeListCount = 16;

	// Custom detour routing structure
#pragma pack(push, 1)
	struct TDetourRoute
	{
		BYTE srcA;
		BYTE srcB;
		BYTE srcC;
		BYTE srcD;
		WORD srcPort;

		BYTE dstA;
		BYTE dstB;
		BYTE dstC;
		BYTE dstD;
		WORD dstPort;
	};
#pragma pack(pop)

	// Pointer to the route list
	TDetourRoute routeArray[16] = {0};
	TDetourRoute * routeList = routeArray;

	extern "C" int (WINAPI * Real_connect)(SOCKET, const struct sockaddr*, int) = connect;
	int WINAPI Detour_connect(SOCKET s, const struct sockaddr* name, int namelen)
	{
		// Store the real port
		WORD port = ntohs((*(WORD*)name->sa_data));

		// Breakup the IP into the parts
		BYTE a = name->sa_data[2];
		BYTE b = name->sa_data[3];
		BYTE c = name->sa_data[4];
		BYTE d = name->sa_data[5];

		struct sockaddr myname = {0};
		memcpy(&myname, name, sizeof(sockaddr));
		// Loop through the vector of routed addresses
		for(size_t x = 0; x < routeListCount; ++x)
		{
			const TDetourRoute & route = routeList[x];
			// If the port matches or the port doesn't matter
			if(route.srcPort == port || route.srcPort == -1)
			{
				// If the addresses match
				if(route.srcA != 255 && route.srcA != a)
					continue;
				if(route.srcB != 255 && route.srcB != b)
					continue;
				if(route.srcC != 255 && route.srcC != c)
					continue;
				if(route.srcD != 255 && route.srcD != d)
					continue;

				// Use the new address instead!
				myname.sa_data[2] = (char)route.dstA;
				myname.sa_data[3] = (char)route.dstB;
				myname.sa_data[4] = (char)route.dstC;
				myname.sa_data[5] = (char)route.dstD;

				// If the dst port is -1, use the original port
				(*(WORD*)myname.sa_data) = htons(route.dstPort == -1 ? port : route.dstPort);

				// Detoured connect in effect
				return Real_connect(s, &myname, namelen);
			}
		}
		// Regular connect
		return Real_connect(s, name, namelen);
	}
}

//-----------------------------------------------------------------------------

// New and improved English patch!
namespace nsEnglishPatch
{
	// languageFlag 
	// 0 - Korean
	// 1 - Chinese
	// 2 - Taiwan
	// 3 - Japan
	// 4 - English
	// 5 - Vietnam	
	// 7 - Turkey
	// 8 - Thailand
	// 9 - Russia
	// 10 - Spain
	// 11 - Arabic

	DWORD languageFlag = 0;

	DWORD codecave_RussiaPatchesYou_ReturnAddress;
	__declspec(naked) void codecave_RussiaPatchesYou()
	{
		__asm pop codecave_RussiaPatchesYou_ReturnAddress
		__asm mov languageFlag, eax // I default text to Korean style (diff font), but set to normal sro
		__asm mov eax, 0x08 // English is 8th tab index
		__asm MOV DWORD PTR DS:[ECX + 0x7E4],EAX
		__asm push codecave_RussiaPatchesYou_ReturnAddress
		__asm ret
	}

	// Function we use to obtain the global settings object
	FARPROC GetGlobalSettingsFunc = 0;
	DWORD customLangAddr;
	DWORD codecave_EnglishPatch_ReturnAddress = 0;
	__declspec(naked) void codecave_EnglishPatch()
	{
		__asm pop codecave_EnglishPatch_ReturnAddress
		__asm pushad
		__asm pushfd
		__asm mov ecx, languageFlag
		__asm call GetGlobalSettingsFunc 
		__asm MOV DWORD PTR DS:[EAX + 0x160], ecx
		__asm popfd
		__asm popad
		__asm mov eax, customLangAddr
		__asm CMP DWORD PTR DS:[eax], 0 // original code, needs updating each client patch
		__asm push codecave_EnglishPatch_ReturnAddress
		__asm ret
	}

	bool PatchLanguage(const char * language, LPBYTE patch_ptr, DWORD patch_size)
	{
		std::vector<LONGLONG> results;
		DWORD physicalStringAddress = 0;
		DWORD logicalStringAddress = 0;

		results = FindSignature((LPBYTE)language, 0, strlen(language) + 1, dataPtr, dataSize);
		if(results.size() != 1)
		{
			printf("[%s][%i] %i results were returned. Only %i were expected. Please use an updated signature for (%s).\n", __FUNCTION__, __LINE__, results.size(), 1, language);
			return false;
		}
		physicalStringAddress = (DWORD)(results[0] + dataStart);
		printf("physicalStringAddress: 0x%X [%s]\n", physicalStringAddress, language);

		BYTE languageStringLoadSig[] = { 0xBE, 0x00, 0x00, 0x00, 0x00 };

		memcpy(languageStringLoadSig + 1, &physicalStringAddress, 4);
		results = FindSignature(languageStringLoadSig, 0, sizeof(languageStringLoadSig), codePtr, codeSize);
		if(results.size() != 1)
		{
			printf("[%s][%i] %i results were returned. Only %i were expected. Please use an updated signature for (%s).\n", __FUNCTION__, __LINE__, results.size(), 1, language);
			return false;
		}
		logicalStringAddress = (DWORD)(results[0] + codeStart);

		logicalStringAddress += 0xC;
		if(strcmp(language, "Korean") != 0)
		{
			logicalStringAddress += 2;
		}
		printf("logicalStringAddress: 0x%X [%s]\n", logicalStringAddress, language);

		WriteBytes(logicalStringAddress, patch_ptr, patch_size);

		return true;
	}

	void Install()
	{
		do
		{
			std::vector<LONGLONG> results;

			BYTE patch1[] = {0xEB};
			PatchLanguage("Korean", patch1, 1);
			PatchLanguage("Chinese", patch1, 1);
			PatchLanguage("Taiwan", patch1, 1);
			PatchLanguage("Japan", patch1, 1);
			PatchLanguage("Vietnam", patch1, 1);
			PatchLanguage("Thailand", patch1, 1);
			PatchLanguage("Russia", patch1, 1);

			BYTE patch2[] = {0x90, 0x90};
			PatchLanguage("English", patch2, 2);

			DWORD physicalCharSelectStringAddress = 0;
			DWORD logicalCharSelectStringAddress = 0;

			DWORD physicalLauncherStringAddress = 0;
			DWORD logicalLauncherStringAddress = 0;

			BYTE charSelectStringSig[] = { 0x43, 0x68, 0x61, 0x72, 0x53, 0x65, 0x6C, 0x65, 0x63, 0x74, 0x00 };

			results = FindSignature(charSelectStringSig, 0, sizeof(charSelectStringSig), dataPtr, dataSize);
			if(results.size() != 1)
			{
				printf("[%s][%i] %i results were returned. Only %i were expected. Please use an updated signature.\n", __FUNCTION__, __LINE__, results.size(), 1);
				break;
			}
			physicalCharSelectStringAddress = (DWORD)(results[0] + dataStart);
			printf("physicalCharSelectStringAddress: 0x%X\n", physicalCharSelectStringAddress);

			BYTE charSelectStringLoadSig[] = { 0x68, 0x00, 0x00, 0x00, 0x00 };
			memcpy(charSelectStringLoadSig + 1, &physicalCharSelectStringAddress, 4);
			results = FindSignature(charSelectStringLoadSig, 0, sizeof(charSelectStringLoadSig), codePtr, codeSize);
			if(results.size() != 2)
			{
				printf("[%s][%i] %i results were returned. Only %i were expected. Please use an updated signature.\n", __FUNCTION__, __LINE__, results.size(), 2);
				break;
			}
			logicalCharSelectStringAddress = (DWORD)(results[1] + codeStart);
			printf("logicalCharSelectStringAddress: 0x%X\n", logicalCharSelectStringAddress);

			DWORD callOffset = *(LPDWORD)(codePtr + results[1] + 0x15 + 1);
			DWORD callAddr = logicalCharSelectStringAddress + 0x15 + callOffset + 5;

			printf("callOffset: 0x%X\n", callOffset);
			printf("callAddr: 0x%X\n", callAddr);
			printf("\n");

			GetGlobalSettingsFunc = (FARPROC)(callAddr);

			BYTE launcherStringSig[] = { 0x50, 0x6C, 0x65, 0x61, 0x73, 0x65, 0x20, 0x45, 0x78, 0x65, 0x63, 0x75, 0x74, 0x65, 0x20, 0x74, 0x68, 0x65, 0x20, 0x22, 0x53, 0x69, 0x6C, 0x6B, 0x72, 0x6F, 0x61, 0x64, 0x2E, 0x65, 0x78, 0x65, 0x2E, 0x22, 0x00 };

			results = FindSignature(launcherStringSig, 0, sizeof(launcherStringSig), dataPtr, dataSize);
			if(results.size() != 1)
			{
				printf("[%s][%i] %i results were returned. Only %i were expected. Please use an updated signature.\n", __FUNCTION__, __LINE__, results.size(), 1);
				break;
			}
			physicalLauncherStringAddress = (DWORD)(results[0] + dataStart);

			printf("physicalLauncherStringAddress: 0x%X\n", physicalLauncherStringAddress);

			BYTE launcherStringLoadSig[] = { 0x68, 0x00, 0x00, 0x00, 0x00 };
			memcpy(launcherStringLoadSig + 1, &physicalLauncherStringAddress, 4);
			results = FindSignature(launcherStringLoadSig, 0, sizeof(launcherStringLoadSig), codePtr, codeSize);
			if(results.size() != 1)
			{
				printf("[%s][%i] %i results were returned. Only %i were expected. Please use an updated signature.\n", __FUNCTION__, __LINE__, results.size(), 1);
				break;
			}
			logicalLauncherStringAddress = (DWORD)(results[0] + codeStart);

			printf("logicalLauncherStringAddress: 0x%X\n", logicalLauncherStringAddress);

			BYTE offs = 0;

			if(*(LPWORD)(codePtr + results[0] + 0x6C) == 0x3D83)
			{
				offs = 0x6C;
			}
			else if(*(LPWORD)(codePtr + results[0] + 0x68) == 0x3D83)
			{
				offs = 0x68;
			}
			else
			{
				printf("[%s][%i] Could not determine where to place the English Patch. It will be skipped.\n", __FUNCTION__, __LINE__);
				break;
			}

			DWORD codecaveAddr = logicalLauncherStringAddress + offs;
			nsEnglishPatch::customLangAddr = *(LPDWORD)(codePtr + results[0] + offs + 2);

			printf("codecaveAddr: 0x%X\n", codecaveAddr);
			printf("customMultiAddr: 0x%X\n", nsEnglishPatch::customLangAddr);
			printf("\n");

			CreateCodeCave(codecaveAddr, 7, nsEnglishPatch::codecave_EnglishPatch);

			// First check to see if this client has the data string 'LanguageTabIndex'
			BYTE tabIndexString[] = { 0x4C, 0x61, 0x6E, 0x67, 0x75, 0x61, 0x67, 0x65, 0x54, 0x61, 0x62, 0x49, 0x6E, 0x64, 0x65, 0x78, 0x00 };
			results = FindSignature(tabIndexString, 0, sizeof(tabIndexString), dataPtr, dataSize);
			if(results.size() > 0) 
			{
				printf("[%s][%i] A client with LanguageTabIndex was detected.\n", __FUNCTION__, __LINE__);
				// RSRO hot fix -- thanks bootdisk
				// http://www.stealthex.org/board/showthread.php?p=72929#post72929
				BYTE patch3[] = { 0x83, 0xC4, 0x04, 0x89, 0x81, 0xE4, 0x07, 0x00, 0x00 };
				results = FindSignature(patch3, 0, sizeof(patch3), codePtr, codeSize);
				if(results.size() == 1)
				{
					CreateCodeCave(codeStart + results[0] + 3, 6, nsEnglishPatch::codecave_RussiaPatchesYou);
					printf("[%s][%i] Found the location to modify the LanguageTabIndex!\n", __FUNCTION__, __LINE__);
				}
				else
				{
					printf("[%s][%i] Could not find the location to modify the LanguageTabIndex.\n", __FUNCTION__, __LINE__);
				}
			}
			else
			{
				printf("[%s][%i] A client without LanguageTabIndex was detected.\n", __FUNCTION__, __LINE__);
			}
		} while(false);
	}
}

//-----------------------------------------------------------------------------

namespace nsKoreanCaptcha
{
	FARPROC AppendStringFunc = 0;

	char * pImageCode;
	char newImageCode[7];
	char * pNewImageCode;

	char b1[3] = {0};
	char b2[3] = {0};
	char b3[3] = {0};
	char b4[3] = {0};
	char b5[3] = {0};
	char b6[3] = {0};

	static DWORD HexStringToInteger(const char * str)
	{
		DWORD val = 0;
		std::stringstream ss;
		ss << std::hex << str;
		ss >> val;
		return val;
	}

	void EnglishCaptcha()
	{
		b1[0] = pImageCode[0];
		b1[1] = pImageCode[1];
		b1[2] = 0;
		newImageCode[0] = (char)HexStringToInteger(b1);

		b2[0] = pImageCode[2];
		b2[1] = pImageCode[3];
		b2[2] = 0;
		newImageCode[1] = (char)HexStringToInteger(b2);

		b3[0] = pImageCode[4];
		b3[1] = pImageCode[5];
		b3[2] = 0;
		newImageCode[2] = (char)HexStringToInteger(b3);

		b4[0] = pImageCode[6];
		b4[1] = pImageCode[7];
		b4[2] = 0;
		newImageCode[3] = (char)HexStringToInteger(b4);

		b5[0] = pImageCode[8];
		b5[1] = pImageCode[9];
		b5[2] = 0;
		newImageCode[4] = (char)HexStringToInteger(b5);

		b6[0] = pImageCode[10];
		b6[1] = pImageCode[11];
		b6[2] = 0;
		newImageCode[5] = (char)HexStringToInteger(b6);

		newImageCode[6] = 0;

		printf("Image Code: %.2X%.2X%.2X%.2X%.2X%.2X\n", (BYTE)newImageCode[0], (BYTE)newImageCode[1], (BYTE)newImageCode[2], (BYTE)newImageCode[3], (BYTE)newImageCode[4], (BYTE)newImageCode[5]);

		pNewImageCode = newImageCode;
	}

	DWORD codecave_Captcha_ReturnAddress = 0;
	__declspec(naked) void codecave_EnglishCaptcha()
	{
		__asm pop codecave_Captcha_ReturnAddress
		__asm mov pImageCode, eax
		__asm pushad
		__asm pushfd
		EnglishCaptcha();
		__asm popfd
		__asm popad
		__asm pop eax
		__asm mov eax, pNewImageCode
		__asm push eax
		__asm call AppendStringFunc // Original code
		__asm push codecave_Captcha_ReturnAddress
		__asm ret
	}

	void Install()
	{
		do
		{
			std::vector<LONGLONG> results;

			BYTE imgCodeSig[] =
			{
				0x66, 0xC7, 0x00, 0x23, 0x63
			};
			results = FindSignature(imgCodeSig, 0, sizeof(imgCodeSig), codePtr, codeSize);
			if(results.size() != 1)
			{
				printf("[%s][%i] %i results were returned. Only %i were expected. Please use an updated signature.\n", __FUNCTION__, __LINE__, results.size(), 1);
				break;
			}

			DWORD ImgCodeCaveAddr = (DWORD)(results[0] + codeStart + 0x1A);
			printf("ImgCodeCaveAddr: 0x%X\n", ImgCodeCaveAddr);

			DWORD callOffset = *(LPDWORD)(codePtr + results[0] + 0x1A + 1);
			DWORD callAddr = ImgCodeCaveAddr + callOffset + 5;
			printf("callOffset: 0x%X\n", callOffset);
			printf("callAddr: 0x%X\n", callAddr);
			printf("\n");

			AppendStringFunc = (FARPROC)callAddr;

			CreateCodeCave(ImgCodeCaveAddr, 5, codecave_EnglishCaptcha);

		} while (false);
	}
}

//-----------------------------------------------------------------------------

namespace nsHackShield
{
	void Install()
	{
		std::vector<LONGLONG> results;

		BYTE winmain_sig[] = { 0x6A, 0x00, 0x68, 0x00, 0x00, 0x40, 0x00 };
		results = FindSignature(winmain_sig, 0, sizeof(winmain_sig), codePtr, codeSize);
		if(results.size() != 1)
		{
			MessageBoxA(0, "Could not find the HackShield signature. The application will now exit for your safety.", "Fatal Error", MB_ICONERROR);
			ExitProcess(0);
		}
		DWORD addr = (DWORD)(results[0] + codeStart + 7);

		DWORD callOffset = *(LPDWORD)(codePtr + results[0] + 7 + 1);
		DWORD callAddr = addr + callOffset + 5;
		printf("callOffset: %X\n", callOffset);
		printf("callAddr: %X\n", callAddr);

		BYTE second_sg[] = {0x53, 0x8B, 0x1D};
		std::vector<LONGLONG> results2 = FindSignature(second_sg, 0, sizeof(second_sg), codePtr + ((callAddr + 0x10) - codeStart), codeSize - ((callAddr + 0x10) - codeStart));
		if(results2.empty())
		{
			MessageBoxA(0, "Could not find the HackShield signature. The application will now exit for your safety.", "Fatal Error", MB_ICONERROR);
			ExitProcess(0);
		}
		printf("Going to write %i NOPs.\n", results2[0]);

		BYTE nops[1024];
		memset(nops, 0x90, 1024);
		WriteBytes(callAddr + 0x10, nops, (DWORD)results2[0]);
	}
}

//-----------------------------------------------------------------------------

namespace nsHookInput
{
	std::vector<LONGLONG> results;

	wchar_t * message;
	DWORD hookInputCustomAddr = 0;

	void OnConsoleInput(std::string input)
	{
		if(input == "/min")
		{
			ShowWindow(GetActiveWindow(), SW_MINIMIZE);
		}
		else if(input == "/exit")
		{
			ExitProcess(0);
		}
	}

	void HookInput()
	{
		char mbMessage[4096] = {0};
		wcstombs(mbMessage, message, 4095);
		OnConsoleInput(mbMessage);
	}

	DWORD codecave_HookInput_ReturnAddress = 0;
	__declspec(naked) void codecave_HookInput()
	{
		__asm pop codecave_HookInput_ReturnAddress
		__asm mov message, EDI
		__asm pushad
		__asm pushfd
		__asm call HookInput
		__asm popfd
		__asm popad
		__asm push edi
		__asm mov edi, hookInputCustomAddr
		__asm cmp [edi], ebp
		__asm pop edi
		__asm push codecave_HookInput_ReturnAddress
		__asm ret
	}

	void Install()
	{
		do
		{
			BYTE chattingStringSig[] =
			{
				0x55, 0x00, 0x49, 0x00, 0x49, 0x00, 0x54, 0x00,
				0x5F, 0x00, 0x4D, 0x00, 0x53, 0x00, 0x47, 0x00, 
				0x5F, 0x00, 0x43, 0x00, 0x41, 0x00, 0x4E, 0x00, 
				0x54, 0x00, 0x5F, 0x00, 0x43, 0x00, 0x48, 0x00,
				0x41, 0x00, 0x54, 0x00, 0x54, 0x00, 0x49, 0x00, 
				0x4E, 0x00, 0x47, 0x00, 0x00, 0x00
			};
			results = FindSignature(chattingStringSig, 0, sizeof(chattingStringSig), dataPtr, dataSize);
			if(results.size() != 1)
			{
				printf("[%s] %i results were returned. Only %i were expected. Please use an updated signature.\n", "chattingStringSig", results.size(), 1);
				break;
			}

			DWORD chattingStringPhysicalAddress = (DWORD)(results[0] + dataStart);
			printf("chattingStringPhysicalAddress: 0x%X\n", chattingStringPhysicalAddress);

			BYTE chattingSig[] =
			{
				0x68, 0x00, 0x00, 0x00, 0x00
			};
			memcpy(chattingSig + 1, &chattingStringPhysicalAddress, 4);

			results = FindSignature(chattingSig, 0, sizeof(chattingSig), codePtr, codeSize);
			if(results.size() != 1)
			{
				printf("[%s] %i results were returned. Only %i were expected. Please use an updated signature.\n", "chattingSig", results.size(), 1);
				break;
			}

			DWORD chattingLogicalAddress = (DWORD)(results[0] + codeStart);
			printf("chattingLogicalAddress: 0x%X\n", chattingLogicalAddress);
			DWORD newSearchAddrOffset = chattingLogicalAddress + 0x200 - codeStart;

			BYTE patchSig[] =
			{
				0x89, 0x6C, 0x24, 0x00,
				0x89, 0x6C, 0x24, 0x00,
				0x89, 0x6C, 0x24, 0x00,
				0x89, 0x6C, 0x24, 0x00
			};
			BYTE patchSigWildcard[] =
			{
				0x00, 0x00, 0x00, 0x01,
				0x00, 0x00, 0x00, 0x01,
				0x00, 0x00, 0x00, 0x01,
				0x00, 0x00, 0x00, 0x01
			};

			results = FindSignature(patchSig, patchSigWildcard, sizeof(patchSig), codePtr + newSearchAddrOffset, 0x200);
			if(results.size() != 1)
			{
				printf("[%s] %i results were returned. Only %i were expected. Please use an updated signature.\n", "patchSig", results.size(), 1);
				break;
			}

			DWORD customAddr = *(LPDWORD)(codePtr + newSearchAddrOffset + results[0] + sizeof(patchSig) + 2);
			printf("customAddr: 0x%X\n", customAddr);
			nsHookInput::hookInputCustomAddr = customAddr;

			DWORD patchLogicalAddress = (DWORD)(newSearchAddrOffset + results[0] + codeStart + sizeof(patchSig));
			printf("patchLogicalAddress: 0x%X\n", patchLogicalAddress);
			printf("\n");
			CreateCodeCave(patchLogicalAddress, 6, nsHookInput::codecave_HookInput);
		} while (false);
	}
}

//-----------------------------------------------------------------------------

namespace nsPacket
{
#pragma pack(push, 4)
	struct Buffer
	{
		// Linked list of buffers for spanned packets
		Buffer * next;

		// Packets are built on the stack and copied to the networking
		// system eventually.
		unsigned char write_buffer[4096];

		// Points to write_buffer on writes, 
		// another (volatile, temporary) buffer on reads
		unsigned char * buffer_ptr;
	};

	struct MsgStreamBuffer
	{
		// Points to a table of functions for the class, in
		// this case, only one function to free the buffers
		// from the internal memory system. Not going to
		// implement that, so this field goes unused.
		void * _vftbl;

		// Current read index
		DWORD current_read_index;

		// The max index that can be read or the current write index
		DWORD max_index;

		// Can't figure this one out yet, seems to always be 0.
		BYTE unknown;

		// The current buffer to read from or write to. When this buffer has been
		// fully processed, it will be marked for freeing and contents will be lost.
		Buffer * current_buffer;

		// The first buffer stored, used internally for memory management I'd think.
		// You cannot loop from original_buffer -> current_buffer after reading has
		// started since buffers are discarded!
		Buffer * original_buffer;

		// The opcode for this message
		WORD opcode;
	};
#pragma pack(pop)

	struct ReadBytesArgs
	{
		DWORD retAddr;
		unsigned char * dst;
		int sze;
	};

	struct WriteBytesArgs
	{
		DWORD retAddr;
		unsigned char * src;
		int sze;
	};

	struct ParsingStruct
	{
		std::list<int> fields;
		std::vector<unsigned char> data;
	};

	bool installed = false;
	DWORD sendPacketRetAddr = 0;
	MsgStreamBuffer * pMsgStreamBufferRead;
	MsgStreamBuffer * pMsgStreamBufferWrite;
	MsgStreamBuffer * pMsgStreamBufferFormat;
	MsgStreamBuffer * pMsgStreamBufferCreate;
	ReadBytesArgs * pReadBytesArgs;
	WriteBytesArgs * pWriteBytesArgs;
	std::map<MsgStreamBuffer *,  ParsingStruct> m_parsing;

	void Process_ReadBytes()
	{
		// Skip the ReadBytes called after the send packet logic
		if(pReadBytesArgs->retAddr == sendPacketRetAddr)
		{
			return;
		}

		ParsingStruct & parsingStruct = m_parsing[pMsgStreamBufferRead];

		if(pMsgStreamBufferRead->current_read_index == 0)
		{
			parsingStruct.data.clear();
			parsingStruct.fields.clear();

			int size = pMsgStreamBufferRead->max_index;
			int index = 0;

			parsingStruct.data.resize(size);

			Buffer * b = pMsgStreamBufferRead->original_buffer;
			while(b)
			{
				int process = size;
				if(process > 4096)
				{
					process = 4096;
				}

				memcpy(&parsingStruct.data[index], b->buffer_ptr, process);

				size -= process;
				index += process;

				b = b->next;
			}
		}

		parsingStruct.fields.push_back(pReadBytesArgs->sze);

		if(pMsgStreamBufferRead->current_read_index + pReadBytesArgs->sze == pMsgStreamBufferRead->max_index)
		{
			std::list<int>::iterator itr0 = parsingStruct.fields.begin();
			std::list<int>::iterator itr1 = parsingStruct.fields.end();

			Analyzer_SetOpcode(pMsgStreamBufferRead->opcode, true);
			//printf("[S->C][%.4X][%i bytes]\n", pMsgStreamBufferRead->opcode, parsingStruct.data.size());

			int index = 0;
			while(itr0 != itr1)
			{
				int len = (*itr0);
				if(index >= (int)parsingStruct.data.size())
				{
					break;
				}
				if(len <= 0)
				{
					printf("\n");
				}
				else
				{
					if(index + len > (int)parsingStruct.data.size())
					{
						break;
					}
					Analyzer_StreamData(&parsingStruct.data[index], len);
					/*for(int x = 0; x < len; ++x)
					{
						printf("%.2X ", parsingStruct.data[index + x]);
					}
					printf("\n");*/
				}
				index += len;
				++itr0;
			}

			//printf("\n");
			Analyzer_EndPacket();

			parsingStruct.data.clear();
			parsingStruct.fields.clear();
		}
	}

	void Process_CreatePacket()
	{
		ParsingStruct & parsingStruct = m_parsing[pMsgStreamBufferCreate];
		parsingStruct.data.clear();
		parsingStruct.fields.clear();
	}

	void Process_WriteBytes()
	{
		ParsingStruct & parsingStruct = m_parsing[pMsgStreamBufferWrite];
		parsingStruct.fields.push_back(pWriteBytesArgs->sze);
		std::copy(pWriteBytesArgs->src, pWriteBytesArgs->src + pWriteBytesArgs->sze, std::back_inserter(parsingStruct.data));
	}

	void Process_FormatPacket()
	{
		ParsingStruct & parsingStruct = m_parsing[pMsgStreamBufferFormat];

		std::list<int>::iterator itr0 = parsingStruct.fields.begin();
		std::list<int>::iterator itr1 = parsingStruct.fields.end();

		Analyzer_SetOpcode(pMsgStreamBufferFormat->opcode, false);
		//printf("[C->S][%.4X][%i bytes]\n", pMsgStreamBufferFormat->opcode, parsingStruct.data.size());

		int index = 0;
		while(itr0 != itr1)
		{
			int len = (*itr0);
			if(index >= (int)parsingStruct.data.size())
			{
				break;
			}
			if(len <= 0)
			{
				printf("\n");
			}
			else
			{
				if(index + len > (int)parsingStruct.data.size())
				{
					break;
				}
				Analyzer_StreamData(&parsingStruct.data[index], len);
				/*for(int x = 0; x < len; ++x)
				{
					printf("%.2X ", parsingStruct.data[index + x]);
				}
				printf("\n");*/
			}
			index += len;
			++itr0;
		}
		
		//printf("\n");
		Analyzer_EndPacket();

		parsingStruct.data.clear();
		parsingStruct.fields.clear();
	}

	DWORD codecave_ReadBytes_ReturnAddress;
	__declspec(naked) void codecave_ReadBytes()
	{
		__asm pop codecave_ReadBytes_ReturnAddress
		__asm mov pReadBytesArgs, esp
		__asm mov pMsgStreamBufferRead, ecx

		// Custom logic
		__asm pushad
		__asm pushfd
		Process_ReadBytes();
		__asm popfd
		__asm popad

		// Original code
		__asm push ebx
		__asm MOV EBX,DWORD PTR SS:[ESP + 0x0C]

		__asm push codecave_ReadBytes_ReturnAddress
		__asm ret
	}

	DWORD codecave_WriteBytes_ReturnAddress;
	__declspec(naked) void codecave_WriteBytes()
	{
		__asm pop codecave_WriteBytes_ReturnAddress
		__asm mov pWriteBytesArgs, esp
		__asm mov pMsgStreamBufferWrite, ecx

		// Custom logic
		__asm pushad
		__asm pushfd
		Process_WriteBytes();
		__asm popfd
		__asm popad

		// Original code
		__asm MOV EAX,DWORD PTR SS:[ESP + 0x04]
		__asm PUSH EBX

		__asm push codecave_WriteBytes_ReturnAddress
		__asm ret
	}

	DWORD codecave_CreatePacket_ReturnAddress;
	__declspec(naked) void codecave_CreatePacket()
	{
		__asm pop codecave_CreatePacket_ReturnAddress
		__asm mov pMsgStreamBufferCreate, esi

		// Custom logic
		__asm pushad
		__asm pushfd
		Process_CreatePacket();
		__asm popfd
		__asm popad

		// Original code
		__asm MOV DWORD PTR DS:[ESI+ 0x04], 0x00

		__asm push codecave_CreatePacket_ReturnAddress
		__asm ret
	}

	DWORD codecave_FormatPacket_ReturnAddress;
	__declspec(naked) void codecave_FormatPacket()
	{
		__asm pop codecave_FormatPacket_ReturnAddress

		// Original code
		__asm MOV ESI,DWORD PTR SS:[ESP + 0x1028]

		__asm mov pMsgStreamBufferFormat, esi

		// Custom logic
		__asm pushad
		__asm pushfd
		Process_FormatPacket();
		__asm popfd
		__asm popad

		__asm push codecave_FormatPacket_ReturnAddress
		__asm ret
	}

	void Install()
	{
		std::vector<LONGLONG> results;

		const DWORD & codeStart = globalExeSectionInfo.codeStart;
		const DWORD & codeSize = globalExeSectionInfo.codeSize;
		const LPBYTE & codePtr = globalExeSectionInfo.codePtr;

		const DWORD & dataStart = globalExeSectionInfo.dataStart;
		const DWORD & dataSize = globalExeSectionInfo.dataSize;
		const LPBYTE & dataPtr = globalExeSectionInfo.dataPtr;

		// Part 1 - Find the string in the client
		// UNICODE "(%03d) Message : 0x%04x"
		BYTE sig_sendPacketDbgMsg[] =
		{
			0x28, 0x00, 0x25, 0x00, 0x30, 0x00, 0x33, 0x00, 0x64,
			0x00, 0x29, 0x00, 0x20, 0x00, 0x4D, 0x00, 0x65, 0x00,
			0x73, 0x00, 0x73, 0x00, 0x61, 0x00, 0x67, 0x00, 0x65,
			0x00, 0x20, 0x00, 0x3A, 0x00, 0x20, 0x00, 0x30, 0x00,
			0x78, 0x00, 0x25, 0x00, 0x30, 0x00, 0x34, 0x00, 0x78,
			0x00, 0x00
		};

		results = FindSignature(sig_sendPacketDbgMsg, 0, sizeof(sig_sendPacketDbgMsg), dataPtr, dataSize);
		if(results.size() != 1)
		{
			printf("[%s][%i] %i results were returned. Only 1 was expected. Please use an updated signature.\n", __FUNCTION__, __LINE__, results.size());
			return;
		}

		DWORD physicalAddress1 = (DWORD)(results[0] + dataStart);
		printf("[%s] physicalAddress1: 0x%X\n", __FUNCTION__, physicalAddress1);

		// Part 2 - Perform the signature search based on the client's address
		BYTE sig_pushSendPacketDbgMsg[] =
		{
			0x68, 0x00, 0x00, 0x00, 0x00
		};
		memcpy(sig_pushSendPacketDbgMsg + 1, &physicalAddress1, 4);

		results = FindSignature(sig_pushSendPacketDbgMsg, 0, sizeof(sig_pushSendPacketDbgMsg), codePtr, codeSize);
		if(results.size() != 1)
		{
			printf("[%s][%i] %i results were returned. Only 1 was expected. Please use an updated signature.\n", __FUNCTION__, __LINE__, results.size());
			return;
		}

		DWORD logicalAddress1 = (DWORD)(results[0] + codeStart);
		printf("[%s] logicalAddress1: 0x%X\n", __FUNCTION__, logicalAddress1);

		// The latest versions of SRO all have the same offsets
		sendPacketRetAddr = logicalAddress1 + 0x57;
		printf("sendPacketRetAddr: %X\n", sendPacketRetAddr);

		DWORD dwReadBytesCallAddr = logicalAddress1 + 0x52 + 5 + *(LPDWORD)(codePtr + results[0] + 1 + 0x52);
		printf("dwReadBytesCallAddr: %X\n", dwReadBytesCallAddr);

		BYTE sig_CreatePacketFunc[] =
		{
			0x33, 0xC0, 0x89, 0x46, 0x10, 0x89, 0x46, 0x14, 
			0xC7, 0x46, 0x08, 0x00, 0x00, 0x00, 0x00, 0xC7, 
			0x46, 0x04, 0x00, 0x00, 0x00, 0x00, 0x8B, 0xC6, 
			0x5E, 0xC2, 0x04, 0x00, 0xCC
		};

		results = FindSignature(sig_CreatePacketFunc, 0, sizeof(sig_CreatePacketFunc), codePtr, codeSize);
		if(results.size() != 1)
		{
			printf("[%s][%i] %i results were returned. Only %i were expected. Please use an updated signature.\n", __FUNCTION__, __LINE__, results.size(), 1);
			return;
		}
		DWORD addrCreatePacketAddr = (DWORD)(results[0] + codeStart + 0x0F);
		printf("addrCreatePacketAddr: 0x%X\n", addrCreatePacketAddr);

		BYTE sig_WriteBytesFunc[] =
		{
			0x56, 0x57, 0x8D, 0x9B, 0x00, 0x00, 0x00, 0x00, 
			0x8B, 0x73, 0x08, 0x81, 0xE6, 0xFF, 0x0F, 0x00, 
			0x80, 0x8B, 0xFD
		};

		results = FindSignature(sig_WriteBytesFunc, 0, sizeof(sig_WriteBytesFunc), codePtr, codeSize);
		if(results.size() != 1)
		{
			printf("[%s][%i] %i results were returned. Only %i were expected. Please use an updated signature.\n", __FUNCTION__, __LINE__, results.size(), 1);
			return;
		}
		DWORD addrWriteBytesAddr = (DWORD)(results[0] + codeStart - 0x18);
		printf("addrWriteBytesAddr: 0x%X\n", addrWriteBytesAddr);

		BYTE sig_FormatPacketFunc[] =
		{
			0x8B, 0xB4, 0x24, 0x28, 0x10, 0x00, 0x00, 0x0F, 
			0xB7, 0x46, 0x18
		};

		results = FindSignature(sig_FormatPacketFunc, 0, sizeof(sig_FormatPacketFunc), codePtr, codeSize);
		if(results.size() != 1)
		{
			printf("[%s][%i] %i results were returned. Only %i were expected. Please use an updated signature.\n", __FUNCTION__, __LINE__, results.size(), 1);
			return;
		}
		DWORD addrFormatPacketAddr = (DWORD)(results[0] + codeStart);
		printf("addrFormatPacketAddr: 0x%X\n", addrFormatPacketAddr);

		CreateCodeCave(dwReadBytesCallAddr, 5, codecave_ReadBytes);

		CreateCodeCave(addrCreatePacketAddr, 7, codecave_CreatePacket);
		CreateCodeCave(addrWriteBytesAddr, 5, codecave_WriteBytes);
		CreateCodeCave(addrFormatPacketAddr, 7, codecave_FormatPacket);

		installed = true;
	}

	void Uninstall()
	{
		if(!installed) return;
	}
}


//-----------------------------------------------------------------------------

namespace nsPatch
{
	std::string English_Patch = "no";
	std::string Multiclient = "no";
	std::string Debug_Console = "no";
	std::string Swear_Filter = "no";
	std::string Nude_Patch = "no";
	std::string Zoom_Hack = "no";
	std::string Korean_Captcha = "no";
	std::string No_Hackshield = "no";
	std::string Redirect_Gateway = "no";
	std::string Redirect_Agent = "no";
	std::string Gateway_Ip = "127.0.0.1";
	std::string Gateway_Port = "19000";
	std::string Agent_Ip = "127.0.0.1";
	std::string Agent_Port = "19001";
	std::string Hook_Input = "no";
	std::string Patch_Seed = "no";
	std::string Auto_Parse = "no";

	SilkroadData silkroadData;

	void Setup()
	{
		char cd[MAX_PATH + 1] = {0};
		GetModuleFileName(0, cd, MAX_PATH);
		std::string scd = cd;
		scd = scd.substr(0, scd.find_last_of("\\/"));
		if(LoadPath(scd.c_str(), silkroadData) == false)
		{
			MessageBoxA(0, "Could not load the Silkroad data for this version.", "Fatal Error", MB_ICONERROR);
			ExitProcess(0);
		}

		ConfigFile cf;

		std::string path = GetCommonDirectory("edxSilkroadLoader5");
		path += "edxSilkroadLoader5.ini";
		if(cf.Open(path) == false)
		{
			MessageBoxA(0, "Could not load the edxSilkroadLoader5.ini configuration file.", "Fatal Error", MB_ICONERROR);
			ExitProcess(0);
		}

		English_Patch = cf.Read("Patches", "English_Patch");
		Multiclient = cf.Read("Patches", "Multiclient");
		Debug_Console = cf.Read("Patches", "Debug_Console");
		Swear_Filter = cf.Read("Patches", "Swear_Filter");
		Nude_Patch = cf.Read("Patches", "Nude_Patch");
		Zoom_Hack = cf.Read("Patches", "Zoom_Hack");
		Korean_Captcha = cf.Read("Patches", "Korean_Captcha");
		No_Hackshield = cf.Read("Patches", "No_Hackshield");
		Redirect_Gateway = cf.Read("Patches", "Redirect_Gateway");
		Redirect_Agent = cf.Read("Patches", "Redirect_Agent");
		Gateway_Ip = cf.Read("Patches", "Gateway_Ip");
		Gateway_Port = cf.Read("Patches", "Gateway_Port");
		Agent_Ip = cf.Read("Patches", "Agent_Ip");
		Agent_Port = cf.Read("Patches", "Agent_Port");
		Hook_Input = cf.Read("Patches", "Hook_Input");
		Patch_Seed = cf.Read("Patches", "Patch_Seed");
		Auto_Parse = cf.Read("Patches", "Auto_Parse");

		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID&)nsDetours::Real_QueryPerformanceCounter, nsDetours::User_QueryPerformanceCounter);

		DetourTransactionCommit();
	}

	// Tokenizes a string into a vector
	static std::vector<std::string> TokenizeString(const std::string& str, const std::string& delim)
	{
		// http://www.gamedev.net/community/forums/topic.asp?topic_id=381544#TokenizeString
		using namespace std;
		vector<string> tokens;
		size_t p0 = 0, p1 = string::npos;
		while(p0 != string::npos)
		{
			p1 = str.find_first_of(delim, p0);
			if(p1 != p0)
			{
				string token = str.substr(p0, p1 - p0);
				tokens.push_back(token);
			}
			p0 = str.find_first_not_of(delim, p1);
		}
		return tokens;
	}

	void InstallAll()
	{
		printf("English_Patch: %s\n", English_Patch.c_str());
		printf("Multiclient: %s\n", Multiclient.c_str());
		printf("Debug_Console: %s\n", Debug_Console.c_str());
		printf("Swear_Filter: %s\n", Swear_Filter.c_str());
		printf("Nude_Patch: %s\n", Nude_Patch.c_str());
		printf("Zoom_Hack: %s\n", Zoom_Hack.c_str());
		printf("Korean_Captcha: %s\n", Korean_Captcha.c_str());
		printf("No_Hackshield: %s\n", No_Hackshield.c_str());
		printf("Redirect_Gateway: %s\n", Redirect_Gateway.c_str());
		printf("Redirect_Agent: %s\n", Redirect_Agent.c_str());
		printf("Gateway_Ip: %s\n", Gateway_Ip.c_str());
		printf("Gateway_Port: %s\n", Gateway_Port.c_str());
		printf("Agent_Ip: %s\n", Agent_Ip.c_str());
		printf("Agent_Port: %s\n", Agent_Port.c_str());
		printf("Hook_Input: %s\n", Hook_Input.c_str());
		printf("Patch_Seed: %s\n", Patch_Seed.c_str());
		printf("Auto_Parse: %s\n", Auto_Parse.c_str());

		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		if(Multiclient == "yes")
		{
			DetourAttach(&(PVOID&)nsDetours::Real_CreateMutexA, nsDetours::User_CreateMutexA);
			DetourAttach(&(PVOID&)nsDetours::Real_bind, nsDetours::User_bind);
			DetourAttach(&(PVOID&)nsDetours::Real_GetAdaptersInfo, nsDetours::User_GetAdaptersInfo);
		}

		if(Redirect_Gateway == "yes" || Redirect_Agent == "yes")
		{
			DetourAttach(&(PVOID&)nsDetours::Real_connect, nsDetours::Detour_connect);
		}

		DetourTransactionCommit();

		if(Auto_Parse == "yes")
		{
			Analyzer_Setup();
			nsPacket::Install();
		}

		if(No_Hackshield == "yes")
		{
			nsHackShield::Install();
		}

		if(Korean_Captcha == "yes")
		{
			nsKoreanCaptcha::Install();
		}

		if(English_Patch == "yes")
		{
			nsEnglishPatch::Install();
		}

		if(Patch_Seed == "yes")
		{
			std::vector<LONGLONG> results;
			do
			{
				BYTE securitySeedSig[] =
				{
					0x8B, 0x4C, 0x24, 0x04, 0x81, 0xE1, 0xFF, 0xFF, 
					0xFF, 0x7F
				};
				results = FindSignature(securitySeedSig, 0, sizeof(securitySeedSig), codePtr, codeSize);
				if(results.size() != 1)
				{
					printf("%i results were returned. Only %i were expected. Please use an updated signature.\n", results.size(), 1);
					break;
				}

				DWORD secSeedAddr = (DWORD)(results[0] + codeStart);
				printf("secSeedAddr: 0x%X\n", secSeedAddr);
				printf("\n");

				BYTE patch1[] = {0xB9, 0x33, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90};
				WriteBytes(secSeedAddr, patch1, sizeof(patch1));
			} while(false);
		}

		if(Swear_Filter == "yes")
		{
			std::vector<LONGLONG> results;
			do
			{
				// Part 1 - Find the string in the client.
				// UNICODE "UIIT_MSG_CHATWND_MESSAGE_FILTER"

				BYTE abuseFilterStringSig[] =
				{
					0x55, 0x00, 0x49, 0x00, 0x49, 0x00, 0x54, 0x00,
					0x5F, 0x00, 0x4D, 0x00, 0x53, 0x00, 0x47, 0x00,
					0x5F, 0x00, 0x43, 0x00, 0x48, 0x00, 0x41, 0x00,
					0x54, 0x00, 0x57, 0x00, 0x4E, 0x00, 0x44, 0x00,
					0x5F, 0x00, 0x4D, 0x00, 0x45, 0x00, 0x53, 0x00,
					0x53, 0x00, 0x41, 0x00, 0x47, 0x00, 0x45, 0x00,
					0x5F, 0x00, 0x46, 0x00, 0x49, 0x00, 0x4C, 0x00,
					0x54, 0x00, 0x45, 0x00, 0x52, 0x00, 0x00, 0x00
				};
				results = FindSignature(abuseFilterStringSig, 0, sizeof(abuseFilterStringSig), dataPtr, dataSize);
				if(results.size() != 1)
				{
					printf("%i results were returned. Only %i were expected. Please use an updated signature.\n", results.size(), 1);
					break;
				}

				DWORD logicalAddress1 = (DWORD)(results[0] + dataStart);
				printf("logicalAddress1: 0x%X\n", logicalAddress1);
				// Part 2 - Perform the signature search based on the client's address

				BYTE abuseFilterSig[] =
				{
					0x68, 0x00, 0x00, 0x00, 0x00
				};
				memcpy(abuseFilterSig + 1, &logicalAddress1, 4);

				results = FindSignature(abuseFilterSig, 0, sizeof(abuseFilterSig), codePtr, codeSize);
				if(results.size() <= 3)
				{
					printf("%i results were returned. 4 or more were expected. Please use an updated signature.\n", results.size());
					break;
				}

				for(size_t x = 0; x < 3; ++x)
				{
					DWORD logicalAddress2 = (DWORD)(results[x] + codeStart);
					DWORD patchAddress = logicalAddress2 - 2;
					printf("patchAddress: 0x%X\n", patchAddress);

					BYTE patch1[] = {0xEB};
					WriteBytes(patchAddress, patch1, sizeof(patch1));
				}

				printf("\n");
			} while(false);
		}
		
		if(Nude_Patch == "yes")
		{
			std::vector<LONGLONG> results;
			do
			{
				BYTE nudePatchSig[] =
				{
					0x8B, 0x84, 0xEE, 0x1C, 0x01, 0x00, 0x00, 0x3B, 
					0x44, 0x24, 0x14
				};
				results = FindSignature(nudePatchSig, 0, sizeof(nudePatchSig), codePtr, codeSize);
				if(results.size() != 1)
				{
					printf("%i results were returned. Only %i were expected. Please use an updated signature.\n", results.size(), 1);
					break;
				}

				DWORD nudePatchAddr = (DWORD)(results[0] + sizeof(nudePatchSig) + codeStart);
				printf("nudePatchAddr: 0x%X\n", nudePatchAddr);

				BYTE patch1[] = {0x90, 0x90};
				WriteBytes(nudePatchAddr, patch1, sizeof(patch1));
				printf("\n");
			} while(false);
		}
		
		if(Zoom_Hack == "yes")
		{
			std::vector<LONGLONG> results;
			do
			{
				BYTE zoomHackSig[] =
				{
					0xDF, 0xE0, 0xF6, 0xC4, 0x41, 0x7A, 0x08, 0xD9,
					0x9E
				};
				results = FindSignature(zoomHackSig, 0, sizeof(zoomHackSig), codePtr, codeSize);
				if(results.size() != 2)
				{
					printf("%i results were returned. Only %i were expected. Please use an updated signature.\n", results.size(), 2);
					break;
				}

				DWORD zoomHackAddr = (DWORD)(results[1] + sizeof(zoomHackSig) - 4 + codeStart);
				printf("zoomHackAddr: 0x%X\n", zoomHackAddr);

				BYTE patch1[] = {0xEB};
				WriteBytes(zoomHackAddr, patch1, sizeof(patch1));
				printf("\n");
			} while(false);
		}

		if(Redirect_Gateway == "yes")
		{
			using namespace nsDetours;

			printf("Redirecting To: %s:%s\n", Gateway_Ip.c_str(), Gateway_Port.c_str());

			std::vector<std::string> tokens = TokenizeString(Gateway_Ip, "\r\n\t .");

			for(size_t x = 0; x < routeListCount; ++x)
			{
				routeArray[x].dstA = atoi(tokens[0].c_str());
				routeArray[x].dstB = atoi(tokens[1].c_str());
				routeArray[x].dstC = atoi(tokens[2].c_str());
				routeArray[x].dstD = atoi(tokens[3].c_str());
				routeArray[x].dstPort = atoi(Gateway_Port.c_str());
			}

			WSADATA wsaData = {0};
			WSAStartup(MAKEWORD(2, 2), &wsaData);
			routeListCount = 0;
			for(size_t x = 0; x < silkroadData.divInfo.divisions.size(); ++x)
			{
				for(size_t y = 0; y < silkroadData.divInfo.divisions[x].addresses.size(); ++y)
				{
					std::string nme = silkroadData.divInfo.divisions[x].addresses[y];
					printf("nme: %s\n", nme.c_str());
					struct hostent * remoteHost = gethostbyname(nme.c_str());
					if(remoteHost)
					{
						struct in_addr addr;
						addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];
						std::string hostip = inet_ntoa(addr);
						printf("Original IP: %s:%i\n", hostip.c_str(), 15779);
						tokens = TokenizeString(hostip, ".");
						routeArray[routeListCount].srcA = atoi(tokens[0].c_str());
						routeArray[routeListCount].srcB = atoi(tokens[1].c_str());
						routeArray[routeListCount].srcC = atoi(tokens[2].c_str());
						routeArray[routeListCount].srcD = atoi(tokens[3].c_str());
						routeArray[routeListCount].srcPort = silkroadData.gatePort;
						routeListCount++;
						if(routeListCount > 16)
						{
							MessageBoxA(0, "There are too many host addresses to track. The application will now exit.", "Fatal Error", MB_ICONERROR);
							ExitProcess(0);
						}
					}
					else
					{
						printf("Lookup Failed for: %s\n", nme.c_str());
					}
				}
			}
			WSACleanup();
		}

		if(Redirect_Agent == "yes")
		{
			using namespace nsDetours;

			printf("Redirecting To: %s:%s\n", Agent_Ip.c_str(), Agent_Port.c_str());

			std::vector<std::string> tokens = TokenizeString(Agent_Ip, "\r\n\t .");

			routeArray[routeListCount].dstA = atoi(tokens[0].c_str());
			routeArray[routeListCount].dstB = atoi(tokens[1].c_str());
			routeArray[routeListCount].dstC = atoi(tokens[2].c_str());
			routeArray[routeListCount].dstD = atoi(tokens[3].c_str());
			routeArray[routeListCount].dstPort = atoi(Agent_Port.c_str());

			routeArray[routeListCount].srcA = 255;
			routeArray[routeListCount].srcB = 255;
			routeArray[routeListCount].srcC = 255;
			routeArray[routeListCount].srcD = 255;
			routeArray[routeListCount].srcPort = 15779; // Just hardcode for now since we'd have to do a bit more work to get the real agent port
			routeListCount++;
		}

		if(Hook_Input == "yes")
		{
			nsHookInput::Install();
		}
	}

	void UninstallAll()
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)nsDetours::Real_QueryPerformanceCounter, nsDetours::User_QueryPerformanceCounter);

		if(Multiclient == "yes")
		{
			DetourDetach(&(PVOID&)nsDetours::Real_CreateMutexA, nsDetours::User_CreateMutexA);
			DetourDetach(&(PVOID&)nsDetours::Real_bind, nsDetours::User_bind);
			DetourDetach(&(PVOID&)nsDetours::Real_GetAdaptersInfo, nsDetours::User_GetAdaptersInfo);
		}

		DetourTransactionCommit();

		if(Debug_Console == "yes")
		{
			RemoveConsole();
		}

		if(Auto_Parse == "yes")
		{
			Analyzer_Cleanup();
		}
	}
}

//-----------------------------------------------------------------------------

void User_Initialize()
{
	nsPatch::Setup();
}

//-----------------------------------------------------------------------------

void User_Deinitialize()
{
	nsPatch::UninstallAll();

	free(codePtr);
	free(dataPtr);
}

//-----------------------------------------------------------------------------

DWORD WINAPI InjectionThread(LPVOID lpParam)
{
	HINSTANCE exeInstance = GetModuleHandle(0);

	ReadBytes(PtrToUlong(exeInstance), peHeader, 4096);

	if(nsPatch::Debug_Console == "yes")
	{
		AddConsole("edxSilkroadLoader5");
	}

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)peHeader;
#define MakePtr(cast, ptr, addValue) (cast)((DWORD)(ptr)+(DWORD)(addValue))
	PIMAGE_NT_HEADERS pNTHeader = MakePtr(PIMAGE_NT_HEADERS, dosHeader, dosHeader->e_lfanew);
#undef MakePtr

	IMAGE_SECTION_HEADER * pish = (IMAGE_SECTION_HEADER *)(((LPBYTE)&pNTHeader->OptionalHeader) + pNTHeader->FileHeader.SizeOfOptionalHeader);
	for(int x = 0; x < pNTHeader->FileHeader.NumberOfSections; ++x)
	{
		IMAGE_SECTION_HEADER & header = pish[x];

		if(header.VirtualAddress == pNTHeader->OptionalHeader.BaseOfData)
		{
			dataStart = pNTHeader->OptionalHeader.ImageBase + header.VirtualAddress;
			dataSize = header.Misc.VirtualSize;
			printf("-- Data --\n");
			printf("\tdataStart: %X\n", dataStart);
			printf("\tdataSize: %X\n\n", dataSize);
		}
		else if(header.VirtualAddress == pNTHeader->OptionalHeader.BaseOfCode)
		{
			codeStart = pNTHeader->OptionalHeader.ImageBase + header.VirtualAddress;
			codeSize = header.Misc.VirtualSize;

			printf("-- Code --\n");
			printf("\tcodeStart: %X\n", codeStart);
			printf("\tcodeSize: %X\n\n", codeSize);
		}
		else
		{
			continue;
		}
	}

	if(codeSize == 0)
	{
		MessageBoxA(0, "codeSize == 0", "Fatal Error", MB_ICONERROR);
		ExitProcess(0);
	}

	if(dataSize == 0)
	{
		MessageBoxA(0, "dataSize == 0", "Fatal Error", MB_ICONERROR);
		ExitProcess(0);
	}

	codePtr = (LPBYTE)malloc(codeSize);
	ReadBytes(codeStart, codePtr, codeSize);

	dataPtr = (LPBYTE)malloc(dataSize);
	ReadBytes(dataStart, dataPtr, dataSize);

	nsPatch::InstallAll();

	CreateMutexA(0, 0, "Silkroad Online Launcher");
	CreateMutexA(0, 0, "Ready");

	return 0;
}

//-----------------------------------------------------------------------------
