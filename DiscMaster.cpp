// DiscMaster.cpp : implementation file
//

#include "pch.h"
#include "CdBurnApp.h"
#include "DiscMaster.h"

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
CString DiscMaster::getId(long index)
{

    BSTR	uniqueID = NULL;
    m_result = m_discMaster->get_Item(index, &uniqueID);
    if (FAILED(m_result))
    {
        m_errorMessage.Format(_T("IDiscMaster2->get_Item(%d) failed! - Error:0x%08x"),
            index, m_result);
        return _T("");
    }

    return uniqueID;
}


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
                //_tprintf(TEXT("\nUnique identifier of the disc device associated with index %d is: %s\n"), iCount, bstrDeviceName);
            }
        }

        
        iCounter++;
    } while (iCounter < 1);
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



// DiscMaster message handlers


