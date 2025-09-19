
// fap50_sdk_app.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cfap50sdkappApp:
// See fap50_sdk_app.cpp for the implementation of this class
//

class C_FAP50_SDK_App : public CWinApp
{
public:
	C_FAP50_SDK_App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern C_FAP50_SDK_App theApp;
