#pragma once


// DiscMaster

class DiscMaster 
{
	IDiscMaster2* m_discMaster; ///DiscMaster object 
	HRESULT m_result;       //A kind of windows api to hold result
	CString m_errorMessage; //Class from MFC to hold error messages

public:
    DiscMaster(/* args */) : m_discMaster(nullptr), m_result(S_OK) {}
    ~DiscMaster() { if (m_discMaster != NULL) m_discMaster->Release(); }
    HRESULT getHResult() { return this->m_result; }    //inline get function
    CString getError() { return this->m_errorMessage; }//inline getter function

    bool initialize();  //to initialize DiscMaster interface
    long getNumberOfDevices(); //to get number of devices
    bool enumerateDiscMasters(); //to enumerate devices

};


