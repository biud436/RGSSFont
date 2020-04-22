// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

//#define USE_UTF16

extern "C" {
	typedef int(*RGSSEvalProto)(const char*);
	typedef int(*RGSSSetStringUTF16Proto)(const char*, const wchar_t*);
	typedef const wchar_t*(*RGSSGetStringUTF16Proto)(const char*);
	typedef int(*RGSSSetStringUTF8Proto)(const char*, const char*);
	typedef const char*(*RGSSGetStringUTF8Proto)(const char*);
	typedef int(*RGSSGetIntProto)(const char*);
	typedef BOOL(*RGSSGetBoolProto)(const char*);

	//typedef union {
	//	unsigned int pixel;
	//	struct {
	//		unsigned char blue;
	//		unsigned char green;
	//		unsigned char red;
	//		unsigned char alpha;
	//	};
	//} RGSSRGBA;

	//typedef struct {
	//	DWORD unk1;
	//	DWORD unk2;
	//	BITMAPINFOHEADER *infoheader;
	//	RGSSRGBA *firstRow;
	//	RGSSRGBA *lastRow;
	//} RGSSBMINFO;

	//typedef struct {
	//	DWORD unk1;
	//	DWORD unk2;
	//	RGSSBMINFO *bminfo;
	//} BITMAPSTRUCT;

	//typedef struct {
	//	DWORD flags;
	//	DWORD klass;
	//	void(*dmark) (void*);
	//	void(*dfree) (void*);
	//	BITMAPSTRUCT *bm;
	//} RGSSBITMAP;

	//typedef struct _RECT {
	//	VALUE x; // 20
	//	VALUE y; // 24
	//	VALUE width; // 28
	//	VALUE height; // 32
	//} RGSSRect;

	//// DrawText for RPG Maker XP
	//typedef struct {
	//	VALUE object; // [0x04] 4 Byte (object ? 1)
	//	VALUE str; // [0x08] 4 Byte (문자열 + 0x0C에 문자열 데이터 있음
	//	void(*dmark) (void*); // 4 byte = char pad1[4]
	//	void(*dfree) (void*); // 4 byte = char pad2[4]
	//	RGSSRect rect; // 4 byte (16 byte)

	//} RGSSDrawTextInfo;

	//// 0x08 : 인자 갯수
	//// 0x0C 
	//// 0x10
	//typedef int(*RGSS1DrawTextProto)(__int64 argc, struct RGSSDrawTextInfo* info, VALUE unknown_flag);

}

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.