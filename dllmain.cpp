// dllmain.cpp: DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"
#include <stdio.h>
#include <process.h>
#include "RGSSFont.h"

unsigned int WINAPI Prepare(LPVOID lParam)
{
	DWORD dwCurrentProcessId = GetCurrentProcessId();

	RGSSFont_Create(dwCurrentProcessId);
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
		// 데드락을 피하기 위해 새로운 쓰레드를 만들어야 한다.
		// 또한 쓰레드를 생성한 후 쓰레드의 핸들을 바로 종료해야 한다.
		// 정형화된 코드이며 DllMain에서 많은 연산을 수행하면 심각한 문제가 생길 수 있다고 한다.
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

