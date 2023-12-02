#pragma once
#include <imapi2.h>
#include <atlstr.h>
class DiscRecorder
{
private:
	IDiscRecorder2* m_recorder;
	HRESULT m_result;
	CString m_recorderUniqueId;
	SAFEARRAY* m_volumePathNames;
	CString m_errorMessage;
public:
	DiscRecorder();
	~DiscRecorder();
	inline HRESULT GetResult() { return m_result; };
	inline CString GetUniqueId() { return m_recorderUniqueId; };
	inline SAFEARRAY* GetRecorderList() { return m_volumePathNames; };
	inline IDiscRecorder2* GetRecorder() { return m_recorder; };
	inline CString GetErrorMessage() { return m_errorMessage; };
	bool initialize(const CString& recorderUniqueId);
	bool AcquireExclusiveAccess(bool force, const CString& clientName);
	bool ReleaseExclusiveAccess(); // to release exclusive access

	CString ExclusiveAccessOwner(); // to get exclusive access owner
	
	bool EjectMedia(); // to eject media
	bool CloseTray(); // to close tray

	bool EnableMcn(); // to enable media change notification
	bool DisableMcn(); // to disable media change notification

	LONG GetLegacyNumber(); // to get legacy number


	CString GetProductID(); // to get product id
	CString GetVendorID(); // to get vendor id
	CString GetVolumeName(); // to get volume name
	CString GetProductRevision(); // to get volume path

	ULONG GetTotalVolumePaths(){}
	CString GetVolumePath(ULONG index);





};	 
