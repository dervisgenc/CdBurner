// DiscRecorder.cpp : implementation file
//

#include "pch.h"
#include "CdBurnApp.h"
#include "DiscRecorder.h"


// DiscRecorder
DiscRecorder::DiscRecorder() : m_recorder(NULL), m_volumePathNames(NULL)
{
}
DiscRecorder::~DiscRecorder()
{

	if (m_recorder != NULL)
		m_recorder->Release();
}

bool DiscRecorder::initialize(const CString& recorderUniqueId) {
	m_recorderUniqueId = recorderUniqueId;

	if (m_recorder == NULL)
	{
		m_result = CoCreateInstance(__uuidof(MsftDiscRecorder2), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IDiscRecorder2), (void**)&m_recorder);
		if (SUCCEEDED(m_result))
		{
			m_result = m_recorder->InitializeDiscRecorder(m_recorderUniqueId.AllocSysString());
			if (SUCCEEDED(m_result)) {
				m_result = m_recorder->get_VolumePathNames(&m_volumePathNames);
				return true;
			}
		}

		m_errorMessage.Format(_T("Couldn't initialize the IDiscRecorder2 - Error:0x%08x"), m_result);        //create a error message that show hresult code of error in hex format
		return false;

	}
	return false;

}

bool DiscRecorder::AcquireExclusiveAccess(bool force, const CString& clientName) {
	if (m_recorder != NULL) {
		force = force ? VARIANT_TRUE : VARIANT_FALSE;
		m_result = m_recorder->AcquireExclusiveAccess(force, clientName.AllocSysString());
		if (SUCCEEDED(m_result))
			return true;;
	}
	m_errorMessage.Format(_T("AcquireExclusiveAccess	 error IDiscRecorder2 - Error:0x%08x"), m_result);
	return false;
}

bool DiscRecorder::ReleaseExclusiveAccess() {
	if (m_recorder != NULL) {
		m_result = m_recorder->ReleaseExclusiveAccess();
		if (SUCCEEDED(m_result)) {
			return true;
		}
	}
	m_errorMessage.Format(_T("ReleaseExclusiveAccess	 error IDiscRecorder2 - Error:0x%08x"), m_result);
	return false;
}

CString DiscRecorder::ExclusiveAccessOwner() {
	if (m_recorder != NULL) {
		BSTR owner;
		m_result = m_recorder->get_ExclusiveAccessOwner(&owner);
		if (SUCCEEDED(m_result)) {
			return owner;
		}
		return _T("");
	}


}

bool DiscRecorder::EjectMedia() {
	if (m_recorder != NULL) {
		m_result = m_recorder->EjectMedia();
		if (SUCCEEDED(m_result)) {
			return true;
		}
	}
	m_errorMessage.Format(_T("EjectMedia error IDiscRecorder2 - Error:0x%08x"), m_result);
	return false;
}

bool DiscRecorder::CloseTray() {
	if (m_recorder != NULL)
	{
		m_result = m_recorder->CloseTray();
		if (SUCCEEDED(m_result))
		{
			return true;
		}
	}
	m_errorMessage.Format(_T("CloseTray error IDiscRecorder2 - Error:0x%08x"), m_result);
	return false;
}

bool DiscRecorder::EnableMcn() {
	if (m_recorder != NULL)
	{
		m_result = m_recorder->EnableMcn();
		if (SUCCEEDED(m_result))
		{
			return true;
		}
	}
	m_errorMessage.Format(_T("EnableMcn error IDiscRecorder2 - Error:0x%08x"), m_result);
	return false;

}
bool DiscRecorder::DisableMcn() {
	if (m_recorder != NULL) {
		m_result = m_recorder->DisableMcn();
		if (SUCCEEDED(m_result))
		{
			return true;
		}
		m_errorMessage.Format(_T("DisableMcn error IDiscRecorder2 - Error:0x%08x"), m_result);
	}
}

LONG DiscRecorder::GetLegacyNumber() {

	if (m_recorder != NULL) {
		LONG legacyNumber = 0;
		m_result = m_recorder->get_LegacyDeviceNumber(&legacyNumber);

		return legacyNumber;

	}
}

CString DiscRecorder::GetProductID() {
	BSTR productId = NULL;
	if (m_recorder != NULL)
	{
		m_recorder->get_ProductId(&productId);
	}
	return productId;
}
CString DiscRecorder::GetProductRevision() {
	BSTR productRevision = NULL;
	if (m_recorder != NULL)
	{
		m_recorder->get_ProductId(&productRevision);
	}
	return productRevision;
}
CString DiscRecorder::GetVolumeName() {
	BSTR volumeName = NULL;
	if (m_recorder != NULL)
	{
		m_recorder->get_ProductId(&volumeName);
	}
	return volumeName;
}
CString DiscRecorder::GetVendorID() {
	BSTR vendorId = NULL;
	if (m_recorder != NULL)
	{
		m_recorder->get_ProductId(&vendorId);
	}
	return vendorId;
}



ULONG DiscRecorder::GetTotalVolumePaths() {
	if (m_volumePathNames != NULL)
	{
		return m_volumePathNames->rgsabound[0].cElements; //which holds the count of elements in the array
	}
	return 0;
}

CString DiscRecorder::GetVolumePath(ULONG index) {
	if (index < m_volumePathNames->rgsabound[0].cElements) {
		return ((VARIANT*)(m_volumePathNames->pvData))[index].bstrVal; //List of drive letters and NTFS mount points for the device. Each element of the list is a VARIANT of type VT_BSTR. The bstrVal member of the variant contains the path.
	}
	return _T("");
}