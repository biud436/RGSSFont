#ifndef __RGSSFONT_H__
#define __RGSSFONT_H__

#include "stdafx.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct RGSSFunctions
{
	RGSSEvalProto _pRGSSEval;
	RGSSSetStringUTF16Proto _pRGSSSetStringUTF16;
	RGSSGetStringUTF16Proto _pRGSSGetStringUTF16;
	RGSSSetStringUTF8Proto _pRGSSSetStringUTF8;
	RGSSGetStringUTF8Proto _pRGSSGetStringUTF8;
	RGSSGetIntProto _pRGSSGetInt;
	RGSSGetBoolProto _pRGSSGetBool;

	RGSSFunctions() :
		_pRGSSEval(NULL),
		_pRGSSSetStringUTF16(NULL),
		_pRGSSGetStringUTF16(NULL),
		_pRGSSSetStringUTF8(NULL),
		_pRGSSGetStringUTF8(NULL),
		_pRGSSGetInt(NULL),
		_pRGSSGetBool(NULL)
	{

	}
};

void RGSSFont_Create();
void RGSSFont_Dispose();
void RGSSFont_PrePare();
int RGSSFont_Initialize(DWORD threadId);
BOOL RGSSFont_InitWithModules();
void RGSSFont_ReadIni();
void RGSSFont_UpdateConfig();

#ifdef __cplusplus
}
#endif

#endif