#pragma once
#include "imapi2.h"
#include <atlstr.h>
class DiscFormat2DataEvents : IDispatch
{
private :
	LPTYPEINFO  m_ptinfo;
	HWND		m_hNotifyWnd; //to send messages to a specific window. This allows your GUI to react and update based on the events received
	DWORD		m_dw;	//to store the cookie returned by the connection point
	LPUNKNOWN	m_pUnkSink;			//to store the IUnknown pointer of the sink object
	LPUNKNOWN	m_pUnkSrc;		//to store the IUnknown pointer of the source object

public:
	DiscFormat2DataEvents();
	~DiscFormat2DataEvents();

	inline void SetHwnd(HWND hWnd) { m_hNotifyWnd = hWnd; }

	DiscFormat2DataEvents* Update();

};