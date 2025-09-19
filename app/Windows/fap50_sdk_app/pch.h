// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef __ANDROID__
#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

//////////////////////////////////////////////////////////////
#include <gdiplus.h>
//#include <GdiplusEnums.h>
#pragma comment (lib, "gdiplus.lib")
using namespace Gdiplus;

class CGDIPlusUse
{
public:
	ULONG_PTR m_gdiplusToken;
	CGDIPlusUse()
	{
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	}
	~CGDIPlusUse()
	{
		GdiplusShutdown(m_gdiplusToken);
		//TRACE(L"Gdiplus Shutdown\r");
	}

};

#define NOP() 	do{}while(0);

#endif //PCH_H
#endif //__ANDROID__
