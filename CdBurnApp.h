
// CdBurnApp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CCdBurnApp:
// See CdBurnApp.cpp for the implementation of this class
//

class CCdBurnApp : public CWinApp
{
public:
	CCdBurnApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CCdBurnApp theApp;
