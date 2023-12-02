#pragma once
#include "imapi2.h"
#include <atlstr.h>
#include "DiscRecorder.h"
class DiscFormat2
{
private:
	IDiscFormat2Data* m_discFormatData;
	HRESULT m_result;
	CString m_errorMessage;
	SAFEARRAY* m_supportedMediaTypes; //supported media types
	bool	m_closeMedia;
protected:
	IStream *m_dataStream;
public:
	DiscFormat2();
	~DiscFormat2();
	inline HRESULT GetResult() { return m_result; };
	inline CString GetErrorMessage() { return m_errorMessage; };
	inline IDiscFormat2Data* GetInterface() { return m_discFormatData; };

	bool initialize(DiscRecorder* pDiscRecorder, const CString& clientName);
	
	ULONG GetTotalNumberSupportedMediaTypes();
	int GetSupportedMediaType(ULONG index); //In the context of IMAPI2 and disc burning APIs, supported media types are often represented as integers rather than strings for a few reasons	

	inline void SetCloseMedia(bool closeMedia) { m_closeMedia = closeMedia; };

	bool Burn(IStream* data);

};

