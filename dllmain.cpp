// dllmain.cpp: DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"
#include <stdio.h>
#include <process.h>
#include "RGSSFont.h"

unsigned int WINAPI Prepare(LPVOID lParam)
{
	RGSSFont_Create();
	RGSSFont_PrePare();
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	HANDLE hThread = NULL;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hThread = (HANDLE)_beginthreadex(NULL, 0, Prepare, (LPVOID)0, 0, NULL);
		CloseHandle(hThread);
		break;
	case DLL_THREAD_ATTACH:
		RGSSFont_Initialize(GetCurrentThreadId());
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		RGSSFont_Dispose();
		break;
	}
	return TRUE;
}

