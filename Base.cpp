// Base.cpp : implementation file
//

#include "pch.h"
#include "CdBurnApp.h"
#include "Base.h"


// Base


Base::~Base()
{
}

// Base member functions

CString Base::GetName() {
	int index = m_path.ReverseFind('\\');
	return m_path.Mid(index + 1);
}