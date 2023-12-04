#include "DiscFormat2.h"
DiscFormat2::DiscFormat2() : m_discFormatData(NULL), m_result(0), m_supportedMediaTypes(NULL)
{
}

DiscFormat2::~DiscFormat2()
{
	if (m_discFormatData != NULL)
	{
		m_discFormatData->Release();
	}
}

int DiscFormat2::GetSupportedMediaType(ULONG index) {
	if (m_discFormatData != NULL)
	{
		VARIANT varItem;
		VariantInit(&varItem);
		if (index < GetTotalNumberSupportedMediaTypes())
		{
			m_result = SafeArrayGetElement(m_supportedMediaTypes, (LONG*)&index, &varItem);
			if (FAILED(m_result))
			{
				m_errorMessage.Format(_T("SafeArrayGetElement Error DiscFormat2 - Error:0x%08x"), m_result); return -1;
			}
			return varItem.intVal;
		}
	}
	return -1;
}

bool DiscFormat2::initialize(DiscRecorder* pDiscRecorder, const CString& clientName) {
	if (pDiscRecorder == NULL)
	{
		m_errorMessage.Format( _T("Error - CDiscFormatData::Initialize - pDiscRecorder is NULL"));
		return false;
	}

	
	if (m_discFormatData == NULL)
	{
		m_result = CoCreateInstance(__uuidof(MsftDiscFormat2Data), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IDiscFormat2Data), (void**)&m_discFormatData);
		if (FAILED(m_result))
		{
			m_errorMessage.Format(_T("Failed to create instance of IDiscFormat2Data: %08X"), m_result);
			return false;
		}
		VARIANT_BOOL isSupported = VARIANT_FALSE;
		m_result = m_discFormatData->IsRecorderSupported(pDiscRecorder->GetRecorder(), &isSupported);
		if (isSupported == VARIANT_FALSE)
		{
			m_errorMessage.Format(_T("Recorder not supported")); return false;
		}
		m_result = m_discFormatData->put_Recorder(pDiscRecorder->GetRecorder());
		if (FAILED(m_result))
		{
			m_errorMessage.Format(_T("put_Recorder Error DiscFormat2 - Error:0x%08x"), m_result); return false;
		}
		m_result =	m_discFormatData->put_ClientName(clientName.AllocSysString());
		if (FAILED(m_result))
		{
			m_errorMessage.Format(_T("put_ClientName Error DiscFormat2 - Error:0x%08x"), m_result); return false;
		}
		m_result = m_discFormatData->get_SupportedMediaTypes(&m_supportedMediaTypes);
		if (FAILED(m_result))
		{
			m_errorMessage.Format(_T("get_SupportedMediaTypes Error DiscFormat2 - Error:0x%08x"), m_result); return false;
		}
		return true;
	}
	return false;
}

ULONG DiscFormat2::GetTotalNumberSupportedMediaTypes() {
	if (m_discFormatData != NULL)
	{
		return m_supportedMediaTypes->rgsabound[0].cElements;
	}
	return 0;
}

bool DiscFormat2::Burn(IStream *data) {//add encryption and notifcation part to track the progress

	if (m_discFormatData==NULL || data == nullptr)
	{
		return false;
	}

	//Is VARIANT_TRUE if the next write session ends by marking the disc as closed to subsequent write sessions.
	m_discFormatData->put_ForceMediaToBeClosed(m_closeMedia ? VARIANT_TRUE : VARIANT_FALSE);//Our case it should be true

	m_result = m_discFormatData->Write(data);
	if (FAILED(m_result))
	{
		m_errorMessage.Format(_T("IDiscFormat2Data->Write Failed! Error:0x%08x"), m_result);

		return false;

	}
	return true;

}