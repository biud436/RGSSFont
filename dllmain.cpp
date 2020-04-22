// dllmain.cpp: DLL ���� ���α׷��� �������� �����մϴ�.
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
		// ������� ���ϱ� ���� ���ο� �����带 ������ �Ѵ�.
		// ���� �����带 ������ �� �������� �ڵ��� �ٷ� �����ؾ� �Ѵ�.
		// ����ȭ�� �ڵ��̸� DllMain���� ���� ������ �����ϸ� �ɰ��� ������ ���� �� �ִٰ� �Ѵ�.
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

