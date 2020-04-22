#include "RGSSFont.h"

#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>

#include <sstream>
#include <string>

#include <process.h>
#include <TlHelp32.h>
#include <iostream>
#include <Psapi.h>

#include <locale>
#include <codecvt>

VOID CALLBACK APCProc(ULONG_PTR);

#pragma region MBCS->WBCS, WBCS->MBCS
const wchar_t* AllocWideChar(const char* law)
{
	int length = MultiByteToWideChar(CP_UTF8, 0, law, -1, NULL, 0);

	if (length == 0)
	{
		return L"";
	}

	// NULL 문자를 포함하여 메모리를 초기화 않으면 오류가 난다.
	LPWSTR lpszWideChar = new WCHAR[length + 1];

	if (lpszWideChar == NULL)
	{
		return L"";
	}

	memset(lpszWideChar, 0, sizeof(WCHAR) * (length + 1));
	int ret = MultiByteToWideChar(CP_UTF8, 0, law, -1, lpszWideChar, length);

	if (ret == 0)
	{
		return L"";
	}

	return lpszWideChar;
}

std::string ToUTF8(const std::wstring& law)
{
	int length = WideCharToMultiByte(CP_UTF8, 0, law.c_str(), -1, NULL, 0, NULL, NULL);

	if (length == 0)
	{
		return "";
	}

	std::string utf8(length + 1, 0);

	if (&utf8[0] == NULL)
	{
		return "";
	}

	int ret = WideCharToMultiByte(CP_UTF8, 0, law.c_str(), -1, &utf8[0], length + 1, NULL, NULL);

	if (ret == 0)
	{
		return "";
	}

	return utf8;
}
std::wstring ToStlUTF8(const char* input)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring output = converter.from_bytes(input);

	return output;
}
#pragma endregion

namespace RGSS 
{
	LPCSTR lpAppName = _T("Game");
	LPWSTR lpwAppName = L"Game";
	LPWSTR lpwFontName = L"Font";
	LPWSTR lpwDefaultFontName = L"나눔고딕";
	LPCSTR lpLibrary = _T("Library");
	LPCSTR lpFontSize = _T("FontSize");
	LPCSTR lpDefaultFontSize = _T("16");
	LPCSTR lpIniName = _T("\\Game.ini");
	LPCSTR lpLibraryPath = _T("System/RGSS301.dll");
}

RGSSFunctions* _modules = NULL;

HMODULE _hRGSSSystemDLL = NULL;

HWND _hWnd = NULL;

TCHAR _szRGSSSystemFilePath[MAX_PATH];
TCHAR _szIniDir[MAX_PATH];
WCHAR _szFontName[MAX_PATH];
TCHAR _szFontSize[MAX_PATH];
WCHAR _szGameTitle[MAX_PATH];

bool _bReady = false;
bool _bInitModules = false;
bool _bDirtyFont = false;
DWORD _dwCurrentProcessId = 0;
DWORD _dwMainThreadId = 0;

void RGSSFont_Create(DWORD dwCurrentProcessId)
{
	_modules = new RGSSFunctions();

	_dwCurrentProcessId = dwCurrentProcessId;

	AllocConsole();
	freopen("CONOUT$", "wt", stdout);
}

void RGSSFont_Dispose()
{
	printf_s("Released RGSSFont!\n");
	delete _modules;
}

void RGSSFont_PrePare()
{
	_bReady = true;
	_hWnd = FindWindow("RGSS Player", NULL);
	printf_s("Prepare()\n");
}

void RGSSFont_ReadIni()
{
	
	GetCurrentDirectory(MAX_PATH, _szIniDir);
	_tcsncat_s(_szIniDir, MAX_PATH, RGSS::lpIniName, MAX_PATH);

	GetPrivateProfileString(RGSS::lpAppName, RGSS::lpLibrary, RGSS::lpLibraryPath, _szRGSSSystemFilePath, MAX_PATH, _szIniDir);

	GetPrivateProfileStringW(RGSS::lpwAppName, RGSS::lpwFontName, RGSS::lpwDefaultFontName, _szFontName, MAX_PATH, AllocWideChar(_szIniDir));

	GetPrivateProfileStringW(RGSS::lpwAppName, L"Title", L"UnTitled", _szGameTitle, MAX_PATH, AllocWideChar(_szIniDir));
	
	GetPrivateProfileString(RGSS::lpAppName, RGSS::lpFontSize, RGSS::lpDefaultFontSize, _szFontSize, MAX_PATH, _szIniDir);

}

BOOL RGSSFont_InitWithModules()
{
	_hRGSSSystemDLL = GetModuleHandle(_szRGSSSystemFilePath);

	if (_hRGSSSystemDLL == NULL) {
		return false;
	}

#define GETPROC(MODULE_NAME) \
	_modules->_p##MODULE_NAME = (MODULE_NAME##Proto)GetProcAddress(_hRGSSSystemDLL, #MODULE_NAME); \
	if(_modules->_p##MODULE_NAME == NULL) { \
		return false; \
	}

	GETPROC(RGSSEval);
	GETPROC(RGSSGetInt);
	GETPROC(RGSSGetBool);
#ifdef USE_UTF16
	GETPROC(RGSSSetStringUTF16);
	GETPROC(RGSSGetStringUTF16);
#else
	GETPROC(RGSSSetStringUTF8);
	GETPROC(RGSSGetStringUTF8);
#endif

#undef GETPROC

	MODULEINFO moduleInfo;
	GetModuleInformation(GetCurrentProcess(), _hRGSSSystemDLL, &moduleInfo, sizeof(moduleInfo));
	
	// 가상 공간의 시작 주소
	char* begin = static_cast<char*>(moduleInfo.lpBaseOfDll);
	//RGSS1DrawTextProto pRGSS1DrawText = (RGSS1DrawTextProto)((PBYTE)begin + 0x0092A0);

	return true;
}

DWORD RGSSGetMainThreadID(DWORD processID)
{
	THREADENTRY32 entry;
	ZeroMemory(&entry, sizeof(THREADENTRY32));
	entry.dwSize = sizeof(THREADENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (Thread32First(snapshot, &entry))
	{
		while (Thread32Next(snapshot, &entry))
		{
			if (entry.th32OwnerProcessID == processID)
			{
				CloseHandle(snapshot);
				return entry.th32ThreadID;
			}
		}
	}

	CloseHandle(snapshot);

	return NULL;
}

int RGSSFont_Initialize(DWORD threadId)
{

	if (!_bInitModules) 
	{
		RGSSFont_ReadIni();

		Sleep(200);

		if (!RGSSFont_InitWithModules()) {
			printf_s("initWithModules failed\n");
			return -1;
		}

		_bInitModules = true;
	}

	if (!_bDirtyFont) {

#ifdef USE_UTF16
		_modules->_pRGSSSetStringUTF16("$font_name", _szFontName);
#else
		_modules->_pRGSSSetStringUTF8(ToUTF8(L"$font_name").c_str(), ToUTF8(_szFontName).c_str());
#endif

		std::string sRubyVersion = _modules->_pRGSSGetStringUTF8("RUBY_VERSION");

		int	nValidId = 0;

		if (sRubyVersion == "1.9.2") {
			nValidId = _modules->_pRGSSGetInt("Game_BattlerBase::FEATURE_ELEMENT_RATE");
			if (_modules->_pRGSSGetBool("defined?(Font) == 'constant'") == TRUE)
			{
				_modules->_pRGSSEval("Font.default_name = [$font_name]");
				_modules->_pRGSSEval("Font.default_size = $font_size.to_i");

				printf_s("Font has been changed!\n");
			}
		}
		else {
			nValidId = _modules->_pRGSSGetInt("Game_Player::CENTER_X");
		}

		// Game_BattlerBase::FEATURE_ELEMENT_RATE가 0이면 아직 스크립트 로드가 덜된 것으로 추측한다.
		if (nValidId == 0)
		{
			printf_s("Scripts is not loaded yet...\n");
			return -1;
		}

		// 폰트 크기를 설정한다.
		std::ostringstream oss;
		oss << "Font.default_size = " << _szFontSize;
		std::string sFontSize = oss.str();

		printf_s("Font.default_size has been changed!\n");

		// 폰트 명을 설정한다.
		_modules->_pRGSSEval("Font.default_name = [$font_name]");
		_modules->_pRGSSEval(sFontSize.c_str());
		_modules->_pRGSSEval("$font_name = nil");

		printf_s("Font.default_name has been changed!\n");

		DWORD newBase;
		__asm {
			MOV EAX, DWORD PTR FS:[0x30]
			MOV EAX, DWORD PTR DS:[EAX+0x08]
			MOV newBase, EAX
		}

		printf_s("Target Address : 0x%x\n", newBase);

		if (sRubyVersion == "1.8.1") {
			_modules->_pRGSSEval("c = Proc.new { cw = $scene.instance_variable_get(\"@command_window\"); cw.contents.font.name = Font.default_name; cw.refresh; cw = nil }; c.call if $scene && $scene.is_a?(Scene_Title)");
		}

		// Set the font when using YEA-MessageSystem
		RGSSFont_UpdateConfig();

		_bDirtyFont = true;
	}

	return 0;
}

void RGSSFont_UpdateConfig()
{
	std::ostringstream oss;
	std::string sRubyVersion = _modules->_pRGSSGetStringUTF8("RUBY_VERSION");
	BOOL isValid = FALSE;
	std::string sValid = "";

	if (sRubyVersion == "1.9.2") {
		
		oss << "$imported[" << "\"YEA-MessageSystem\"" << "]";
		sValid = oss.str();

		isValid = _modules->_pRGSSGetBool(sValid.c_str());

		if (isValid == TRUE)
		{
			_modules->_pRGSSEval("YEA::MESSAGE.const_set(:MESSAGE_WINDOW_FONT_NAME, Font.default_name)");
			_modules->_pRGSSEval("YEA::MESSAGE.const_set(:MESSAGE_WINDOW_FONT_SIZE, Font.default_size)");
			printf_s("YEA-MessageSystem found!\n");
		}

		oss.str("");
		oss << "$imported[" << "\"RS_HangulMessageSystem\"" << "]";
		sValid = oss.str();

		isValid = _modules->_pRGSSGetBool(sValid.c_str());
		if (isValid == TRUE)
		{
#ifdef USE_UTF16
			_modules->_pRGSSSetStringUTF16("$temp_font_name", L"RS::LIST[\"폰트명\"] = Font.default_name");
			_modules->_pRGSSSetStringUTF16("$temp_font_size", L"RS::LIST[\"폰트크기\"] = Font.default_size");
#else
			_modules->_pRGSSSetStringUTF8(ToUTF8(L"$temp_font_name").c_str(), ToUTF8(L"RS::LIST[\"폰트명\"] = Font.default_name").c_str());
			_modules->_pRGSSSetStringUTF8(ToUTF8(L"$temp_font_size").c_str(), ToUTF8(L"RS::LIST[\"폰트크기\"] = Font.default_size").c_str());
#endif
			_modules->_pRGSSEval("$temp_font_name");
			_modules->_pRGSSEval("$temp_font_size");
			_modules->_pRGSSEval("$temp_font_name = nil");
			_modules->_pRGSSEval("$temp_font_size = nil");

			printf_s("RS_HangulMessageSystem found!\n");
		}
	}
}