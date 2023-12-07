#include "DiscMaster.h"
#include "pch.h"

bool DiscMaster::initialize() {

    if (m_discMaster == nullptr)
    {
        this->m_result = CoCreateInstance(__uuidof(MsftDiscMaster2), NULL, CLSCTX_ALL, __uuidof(IDiscMaster2),
            reinterpret_cast<void**>(&m_discMaster));       //Create instance of IDiscMaster2 interface
        if (!SUCCEEDED(this->m_result))
        {
            m_errorMessage.Format(_T("Couldn't initialize the IDiscMaster2 - Error:0x%08x"), m_result);        //create a error message that show hresult code of error in hex format
            return false;
        }

    }
    //check is there any device use the interface
    VARIANT_BOOL isSupported = VARIANT_FALSE;
    m_result = m_discMaster->get_IsSupportedEnvironment(&isSupported);

    if (!SUCCEEDED(m_result))
    {
        m_errorMessage.Format(_T("get_IsSupportedEnvironment error IDiscMaster2 - Error:0x%08x"), m_result);        //create a error message that show hresult code of error in hex format
        return false;
    }

    //check the media is supported or not
    if (isSupported == VARIANT_FALSE)
    {
        m_errorMessage = _T("There were no writable devices detected!");
        return false;
    }
    return true;


}

//bool DiscMaster::enumerateDiscMasters() {
//
//    CComPtr<IEnumVARIANT> pEnum;        //o hold result list
//    m_result = m_discMaster->get__NewEnum(&pEnum);
//    if (FAILED(m_result)) {
//        m_errorMessage.Format(_T("get__NewEnum error IDiscMaster2 - Error:0x%08x"), m_result);
//        return false;
//    }
//
//    VARIANT varItem;
//    VariantInit(&varItem);
//
//    while (pEnum->Next(1, &varItem, NULL) == S_OK)
//    {
//        CComPtr<IDiscRecorder2> pRecorder;
//        m_result = varItem.pdispVal->QueryInterface(IID_PPV_ARGS(&pRecorder));
//        if (SUCCEEDED(m_result))
//        {
//        }
//
//        VariantClear(&varItem);
//    }
//
//
//}

//bool DiscMaster::enumerateDiscMasters   () {
//
//	CComPtr<IEnumVARIANT> pEnum;        //o hold result list
//	m_result = m_discMaster->get__NewEnum(&pEnum);
//	if (FAILED(m_result)) {
//		m_errorMessage.Format(_T("get__NewEnum error IDiscMaster2 - Error:0x%08x"), m_result);
//		return false;
//	}
//
//	VARIANT varItem;
//	VariantInit(&varItem);
//
//	while (pEnum->Next(1, &varItem, NULL) == S_OK)
//	{
//		CComPtr<IDiscRecorder2> pRecorder;
//		m_result = varItem.pdispVal->QueryInterface(IID_PPV_ARGS(&pRecorder));
//		if (SUCCEEDED(m_result))
//		{
//			BSTR bstrUniqueID;
//			m_result = m_discMaster->get_Item(&bstrUniqueID);
//			if (SUCCEEDED(m_result))
//			{
//				CString strUniqueID(bstrUniqueID);
//				m_listDiscMasters.AddTail(strUniqueID);
//				SysFreeString(bstrUniqueID);
//			}
//		}
//
//		VariantClear(&varItem);
//	}
//
//	return true;
//
//}



bool DiscMaster::enumerateDiscMasters() {

    UINT           iCounter = 0;
    LONG           lValue = 0;
    BSTR           bstrDeviceName;
    do {
        // Get the number of drives 
        m_result = m_discMaster->get_Count(&lValue);
        if (SUCCEEDED(m_result)) {
            m_errorMessage.Format(_T("\nTotal number of drives = %d\n"), lValue);
        }

        // Print all the optical drives attached to the system 
        if (SUCCEEDED(m_result)) {
            for (LONG iCount = 0; iCount < lValue; iCount++) {
                m_result = m_discMaster->get_Item(iCount, &bstrDeviceName);
                _tprintf(TEXT("\nUnique identifier of the disc device associated with index %d is: %s\n"), iCount, bstrDeviceName);
            }
        }

        // Prompt the user to unhook or add drives
        if (iCounter < 1) {
            MessageBox(NULL, TEXT("Please un-hook or add drives and hit OK"), TEXT("Manual Action"), MB_OK);
            _tprintf(TEXT("\nGetting the altered configuration ... \n"));
        }
        iCounter++;
    } while (iCounter < 2);
    return true;
}



long DiscMaster::getNumberOfDevices() {

    if (m_discMaster == NULL)
    {
        return 0;
    }
    long total_number = 0;
    m_result = m_discMaster->get_Count(&total_number);
    if (FAILED(m_result))
    {
        m_errorMessage.Format(_T("get_Count error IDiscMaster2 - Error:0x%08x"), m_result);
        return 0;
    }
    return total_number;

}
