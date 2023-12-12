
// CdBurnAppDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "CdBurnApp.h"
#include "CdBurnAppDlg.h"
#include "afxdialogex.h"
#include "DiscMaster.h"
#include "DiscRecorder.h"
#include "DiscFormat2Data.h"
#include "File.h"
#include "Directory.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define CD_MEDIA		0
#define DVD_MEDIA		1
#define DL_DVD_MEDIA	2

#define CLIENT_NAME		_T("Cd Burner")

#define WM_BURN_STATUS_MESSAGE	WM_APP+300
#define WM_BURN_FINISHED		WM_APP+301
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCdBurnAppDlg dialog



CCdBurnAppDlg::CCdBurnAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CDBURNAPP_DIALOG, pParent), m_closeMedia(TRUE)
	, m_cancelBurn(false)
	, m_selectedMediaType(-1)
	, m_isBurning(false)
	, m_ejectWhenFinished(TRUE)
{
	EnableActiveAccessibility();
	m_volumeLabel = CTime::GetCurrentTime().Format(_T("%Y_%m_%d"));

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCdBurnAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, Device_List, m_deviceComboBox);

	DDX_Control(pDX, IDC_LIST, m_fileListbox);
	DDX_Control(pDX, IDC_CAPACITY, m_capacityProgress);
	DDX_Control(pDX, IDC_MEDIA_TYPE_COMBO, m_mediaTypeCombo);
	DDX_Control(pDX, IDC_SUPPORTED_MEDIA_TYPES, m_supportedMediaTypes);
	DDX_Text(pDX, IDC_VOLUME_NAME, m_volumeLabel);
}

BEGIN_MESSAGE_MAP(CCdBurnAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON() 
	ON_CBN_SELCHANGE(Device_List, &CCdBurnAppDlg::OnCbnSelchangeList)
	ON_LBN_SELCHANGE(IDC_LIST, &CCdBurnAppDlg::OnLbnSelchangeList)
	ON_BN_CLICKED(IDC_ADD, &CCdBurnAppDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_FOLDER, &CCdBurnAppDlg::OnBnClickedFolder)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BURN_BUTTON, &CCdBurnAppDlg::OnBnClickedBurnButton)
	ON_CBN_SELCHANGE(IDC_MEDIA_TYPE_COMBO, &CCdBurnAppDlg::OnCbnSelchangeMediaTypeCombo)
	ON_BN_CLICKED(IDC_REMOVE, &CCdBurnAppDlg::OnBnClickedRemove)
END_MESSAGE_MAP()


// CCdBurnAppDlg message handlers

void CCdBurnAppDlg::SetCancelBurning(bool bCancel)
{
	CSingleLock singleLock(&m_critSection);
	m_cancelBurn = bCancel;
}

bool CCdBurnAppDlg::GetCancelBurning()
{
	CSingleLock singleLock(&m_critSection);
	return m_cancelBurn;
}

UINT CCdBurnAppDlg::BurnThread(LPVOID pParam)
{
	CCdBurnAppDlg* pThis = (CCdBurnAppDlg*)pParam;

	int selectedIndex = pThis->m_deviceComboBox.GetCurSel();
	if (selectedIndex == -1)
	{
		pThis->SendMessage(WM_BURN_FINISHED, 0, (LPARAM)_T("Error: No Device Selected"));
		return 0;
	}
	DiscRecorder* pOrginalDiscRecorder = (DiscRecorder*)pThis->m_deviceComboBox.GetItemDataPtr(selectedIndex);
	if (pOrginalDiscRecorder == NULL)
	{
		pThis->SendMessage(WM_BURN_FINISHED, 0, (LPARAM)_T("Error: No Device Selected"));
		return 0;
	}
	if (pThis->GetCancelBurning()) {
		pThis->SendMessage(WM_BURN_FINISHED, 0, (LPARAM)_T("Cancalled"));
		return 0;

	}
	pThis->SendMessage(WM_BURN_STATUS_MESSAGE,0,(LPARAM)_T("Initializing..."));
	DiscRecorder discRecorder;
	CString errorMessage;
	if (!discRecorder.initialize(pOrginalDiscRecorder->GetUniqueId())) {
		errorMessage.Format(_T("Failed to initialize recorder - Unique ID:%s"),
			(LPCTSTR)pOrginalDiscRecorder->GetUniqueId());
		pThis->SendMessage(WM_BURN_FINISHED, discRecorder.GetResult(),
			(LPARAM)(LPCTSTR)errorMessage);
	}
	else
	{
		DiscFormat2Data discFormat;
		if(!discRecorder.AcquireExclusiveAccess(true, CLIENT_NAME)){
			errorMessage.Format(_T("Failed: %s is exclusive owner"),
				(LPCTSTR)discRecorder.ExclusiveAccessOwner());
			pThis->SendMessage(WM_BURN_FINISHED, discRecorder.GetResult(),
				(LPARAM)(LPCTSTR)errorMessage);
		}
		else {
			// Get the media type
			IMAPI_MEDIA_PHYSICAL_TYPE mediatype = IMAPI_MEDIA_TYPE_UNKNOWN;
			discFormat.GetInterface()->get_CurrentPhysicalMediaType(&mediatype);

			//create file sysstem //add encryption
			IStream* dataStream = NULL;
			if (!CreateMediaFileSystem(pThis,mediatype,&dataStream))
			{
				return false;
			}
			discFormat.SetCloseMedia(pThis->m_closeMedia);

			discFormat.Burn(dataStream);

			dataStream->Release();

			// eject if you want
			// discRecorder.EjectMedia();
		}
		discRecorder.ReleaseExclusiveAccess();
		pThis->SendMessage(WM_BURN_FINISHED, discFormat.GetResult(),
			(LPARAM)(LPCTSTR)discFormat.GetErrorMessage());
	}
	return 0;
}

bool CCdBurnAppDlg::CreateMediaFileSystem(CCdBurnAppDlg* pThis, IMAPI_MEDIA_PHYSICAL_TYPE mediaType,IStream** ppDataStream)
{
	IFileSystemImage* image = NULL;
	IFileSystemImageResult* imageResult = NULL;
	IFsiDirectoryItem* rootItem = NULL;
	CString					message;
	bool					returnVal = false;

	HRESULT hr = CoCreateInstance(CLSID_MsftFileSystemImage,
		NULL, CLSCTX_ALL, __uuidof(IFileSystemImage), (void**)&image);
	if (FAILED(hr) || (image == NULL))
	{
		pThis->SendMessage(WM_BURN_FINISHED, hr,
			(LPARAM)_T("Failed to create IFileSystemImage Interface"));
		return false;
	}

	pThis->SendMessage(WM_BURN_STATUS_MESSAGE, 0, (LPARAM)_T("Creating File System..."));

	image->put_FileSystemsToCreate((FsiFileSystems)(FsiFileSystemJoliet | FsiFileSystemISO9660));
	image->put_VolumeName(pThis->m_volumeLabel.AllocSysString());
	image->ChooseImageDefaultsForMediaType(mediaType);

	//
	// Get the image root
	//
	hr = image->get_Root(&rootItem);
	if (SUCCEEDED(hr))
	{
		//
		// Add Files and Directories to File System Image
		//
		int itemCount = pThis->m_fileListbox.GetCount();
		for (int itemIndex = 0; itemIndex < itemCount; itemIndex++)
		{
			Base* pObject = (Base*)pThis->m_fileListbox.GetItemDataPtr(itemIndex);
			ASSERT(pObject != NULL);
			if (pObject == NULL)
				continue;

			CString fileName = pObject->GetName();
			message.Format(_T("Adding \"%s\" to file system..."), (LPCTSTR)fileName);
			pThis->SendMessage(WM_BURN_STATUS_MESSAGE, 0, (LPARAM)(LPCTSTR)message);

			if (pObject->IsKindOf(RUNTIME_CLASS(File)))
			{
				File* pFileObject = (File*)pObject;
				IStream* fileStream = pFileObject->createStream();
				if (fileStream != NULL)
				{
					hr = rootItem->AddFile(pFileObject->GetName().AllocSysString(), fileStream);
					if (FAILED(hr))
					{
						// IMAPI_E_IMAGE_SIZE_LIMIT 0xc0aab120
						message.Format(_T("Failed IFsiDirectoryItem->AddFile(%s)!"),
							(LPCTSTR)pFileObject->GetName());
						pThis->SendMessage(WM_BURN_FINISHED, hr, (LPARAM)(LPCTSTR)message);
						break;
					}
				}
			}
			else if (pObject->IsKindOf(RUNTIME_CLASS(Directory)))
			{
				Directory* pDirObject = (Directory*)pObject;
				hr = rootItem->AddTree(pDirObject->GetPath().AllocSysString(), VARIANT_TRUE);

				if (FAILED(hr))
				{
					// IMAPI_E_IMAGE_SIZE_LIMIT 0xc0aab120
					message.Format(_T("Failed IFsiDirectoryItem->AddTree(%s)!"),
						(LPCTSTR)pDirObject->GetName());
					pThis->SendMessage(WM_BURN_FINISHED, hr, (LPARAM)(LPCTSTR)message);
					break;
				}
			}

			//
			// Did user cancel?
			//
			if (pThis->GetCancelBurning())
			{
				pThis->SendMessage(WM_BURN_FINISHED, 0, (LPARAM)_T("User Canceled!"));
				hr = IMAPI_E_FSI_INTERNAL_ERROR;
			}
		}

		if (SUCCEEDED(hr))
		{
			// Create the result image
			hr = image->CreateResultImage(&imageResult);
			if (SUCCEEDED(hr))
			{
				//
				// Get the stream
				//
				hr = imageResult->get_ImageStream(ppDataStream);
				if (SUCCEEDED(hr))
				{
					returnVal = true;
				}
				else
				{
					pThis->SendMessage(WM_BURN_FINISHED, hr,
						(LPARAM)_T("Failed IFileSystemImageResult->get_ImageStream!"));
				}

			}
			else
			{
				pThis->SendMessage(WM_BURN_FINISHED, hr,
					(LPARAM)_T("Failed IFileSystemImage->CreateResultImage!"));
			}
		}
	}
	else
	{
		pThis->SendMessage(WM_BURN_FINISHED, hr, (LPARAM)_T("Failed IFileSystemImage->getRoot"));
	}

	//
	// Cleanup
	//
	if (image != NULL)
		image->Release();
	if (imageResult != NULL)
		imageResult->Release();
	if (rootItem != NULL)
		rootItem->Release();

	return returnVal;
}

BOOL CCdBurnAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	AddRecordersToComboBox();
	OnLbnSelchangeList();
	enableBurnButton();
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCdBurnAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCdBurnAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCdBurnAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCdBurnAppDlg::AddRecordersToComboBox() {
	DiscMaster discMaster;

	//
    // Cleanup old data on combobox
	//
	int itemCount = m_deviceComboBox.GetCount();
	for (int itemIndex = 0; itemIndex < itemCount; itemIndex++)
	{
		delete (DiscRecorder*)m_deviceComboBox.GetItemDataPtr(itemIndex);
	}
	m_deviceComboBox.ResetContent();
	if (!discMaster.initialize()) {
		AfxMessageBox(discMaster.getError(), MB_OK | MB_ICONERROR);
		EndDialog(IDOK);
		return;
	}
	long totalDevices = discMaster.getNumberOfDevices();
	if (totalDevices == 0 && FAILED(discMaster.getHResult()))
	{
		AfxMessageBox(discMaster.getError(), MB_OK | MB_ICONERROR);
	}
	for (long i = 0; i < totalDevices; i++)
	{
		CString recorderId = discMaster.getId(i);
		if (recorderId.IsEmpty())
		{
			CString errorMessage(discMaster.getError());
			if (!errorMessage.IsEmpty())
			{
				AfxMessageBox(errorMessage, MB_OK | MB_ICONERROR);
				continue;
			}
		}
	
		DiscRecorder* discRecorder = new DiscRecorder();
		ASSERT(discRecorder != NULL);

		if (!discRecorder->initialize(recorderId))
		{
			AfxMessageBox(discRecorder->GetErrorMessage(), MB_OK | MB_ICONERROR);

			if (totalDevices == 1 && FAILED(discRecorder->GetResult())) {

				AfxMessageBox(discRecorder->GetErrorMessage(), MB_OK | MB_ICONERROR);
			}
			delete discRecorder;
		}
		CString volumeList;
		ULONG totalVolumePaths = discRecorder->GetTotalVolumePaths();
		for (ULONG i = 0; i < totalVolumePaths; i++)
		{
			if (i)
				volumeList += _T(",");
			volumeList += discRecorder->GetVolumePath(i);
		}
		CString productID = discRecorder->GetProductID();
		CString strName;
		strName.Format(_T("%s [%s]"), (LPCTSTR)volumeList, (LPCTSTR)productID);
		int index = m_deviceComboBox.AddString(strName);
		m_deviceComboBox.SetItemDataPtr(index, discRecorder);
	}


	

	if(totalDevices > 0)
	{
		m_deviceComboBox.SetCurSel(0);
		OnCbnSelchangeList();
	}	

}

CString CCdBurnAppDlg::GetMediaTypeString(int mediaType)
{
	CString mediaTypeString;
	switch (mediaType)
	{
	case IMAPI_MEDIA_TYPE_UNKNOWN:
	default:
		return _T("Unknown Media Type");

	case IMAPI_MEDIA_TYPE_CDROM:
		m_isCdromSupported = true;
		return _T("CD-ROM or CD-R/RW media");

	case IMAPI_MEDIA_TYPE_CDR:
		m_isCdromSupported = true;
		return _T("CD-R");

	case IMAPI_MEDIA_TYPE_CDRW:
		m_isCdromSupported = true;
		return _T("CD-RW");

	case IMAPI_MEDIA_TYPE_DVDROM:
		m_isDvdSupported = true;
		return _T("DVD ROM");

	case IMAPI_MEDIA_TYPE_DVDRAM:
		m_isDvdSupported = true;
		return _T("DVD-RAM");

	case IMAPI_MEDIA_TYPE_DVDPLUSR:
		m_isDvdSupported = true;
		return _T("DVD+R");

	case IMAPI_MEDIA_TYPE_DVDPLUSRW:
		m_isDvdSupported = true;
		return _T("DVD+RW");

	case IMAPI_MEDIA_TYPE_DVDPLUSR_DUALLAYER:
		m_isDualLayerDvdSupported = true;
		return _T("DVD+R Dual Layer");

	case IMAPI_MEDIA_TYPE_DVDDASHR:
		m_isDvdSupported = true;
		return _T("DVD-R");

	case IMAPI_MEDIA_TYPE_DVDDASHRW:
		m_isDvdSupported = true;
		return _T("DVD-RW");

	case IMAPI_MEDIA_TYPE_DVDDASHR_DUALLAYER:
		m_isDualLayerDvdSupported = true;
		return _T("DVD-R Dual Layer");

	case IMAPI_MEDIA_TYPE_DISK:
		return _T("random-access writes");

	case IMAPI_MEDIA_TYPE_DVDPLUSRW_DUALLAYER:
		m_isDualLayerDvdSupported = true;
		return _T("DVD+RW DL");

	case IMAPI_MEDIA_TYPE_HDDVDROM:
		return _T("HD DVD-ROM");

	case IMAPI_MEDIA_TYPE_HDDVDR:
		return _T("HD DVD-R");

	case IMAPI_MEDIA_TYPE_HDDVDRAM:
		return _T("HD DVD-RAM");

	case IMAPI_MEDIA_TYPE_BDROM:
		return _T("Blu-ray DVD (BD-ROM)");

	case IMAPI_MEDIA_TYPE_BDR:
		return _T("Blu-ray media");

	case IMAPI_MEDIA_TYPE_BDRE:
		return _T("Blu-ray Rewritable media");
	}
	return mediaTypeString;
}


afx_msg void CCdBurnAppDlg::OnCbnSelchangeList(){
	m_isCdromSupported = false;
	m_isDvdSupported = false;
	m_isDualLayerDvdSupported = false;

	m_mediaTypeCombo.ResetContent();

	int index = m_deviceComboBox.GetCurSel();
	if (index < 0) {
		return;
	}

	DiscRecorder* discRecorder =
		(DiscRecorder*)m_deviceComboBox.GetItemDataPtr(index);

	if (discRecorder != NULL)
	{
		DiscFormat2Data discFormatData;
		if (!discFormatData.initialize(discRecorder, CLIENT_NAME))
		{
			return;
		}
		CString supportedMediaTypes;
		for (ULONG i = 0; i < discFormatData.GetTotalNumberSupportedMediaTypes(); i++)
		{
			int mediaType = discFormatData.GetSupportedMediaType(i);
			if (i>0)
				supportedMediaTypes += _T(",");
			supportedMediaTypes += GetMediaTypeString(mediaType);
		}
		m_supportedMediaTypes.SetWindowText(supportedMediaTypes);
	}
	//
		// Add Media Selection
		//
	if (m_isCdromSupported)
	{
		int stringIndex = m_mediaTypeCombo.AddString(_T("700MB CD Media"));
		m_mediaTypeCombo.SetItemData(stringIndex, CD_MEDIA);
	}
	if (m_isDvdSupported)
	{
		int stringIndex = m_mediaTypeCombo.AddString(_T("4.7GB DVD Media"));
		m_mediaTypeCombo.SetItemData(stringIndex, DVD_MEDIA);
	}
	if (m_isDualLayerDvdSupported)
	{
		int stringIndex = m_mediaTypeCombo.AddString(_T("8.5GB Dual-Layer DVD"));
		m_mediaTypeCombo.SetItemData(stringIndex, DL_DVD_MEDIA);
	}
	m_mediaTypeCombo.SetCurSel(0);
	OnCbnSelchangeMediaTypeCombo();
}

void CCdBurnAppDlg::OnLbnSelchangeBurnFileList()
{
	GetDlgItem(IDC_REMOVE)->EnableWindow(m_fileListbox.GetCurSel() != -1);
}

void CCdBurnAppDlg::UpdateCapacity()
{
	//
	// Set the selected media type data
	//
	ULONGLONG totalSize = 0;
	CString maxText;
	if (m_selectedMediaType == CD_MEDIA)
	{
		maxText = _T("700MB");
		totalSize = 700000000;
	}
	else if (m_selectedMediaType == DVD_MEDIA)
	{
		maxText = _T("4.7GB");
		totalSize = 4700000000;
	}
	else if (m_selectedMediaType == DL_DVD_MEDIA)
	{
		maxText = _T("8.5GB");
		totalSize = 8500000000;
	}
	//m_maxText.SetWindowText(maxText);

	//
	// Calculate the size of the files
	//
	ULONGLONG mediaSize = 0;
	int itemCount = m_fileListbox.GetCount();
	for (int itemIndex = 0; itemIndex < itemCount; itemIndex++)
	{
		Base* pObject = (Base*)m_fileListbox.GetItemDataPtr(itemIndex);
		mediaSize += pObject->GetSize();
	}

	m_capacityProgress.SetRange(0, 100);
	if (mediaSize == 0)
	{
		m_capacityProgress.SetPos(0);
#if _MFC_VER >= 0x0900
		m_capacityProgress.SetState(PBST_NORMAL);
#endif
	}
	else
	{
		int percent = (int)((mediaSize * 100) / totalSize);
		if (percent > 100)
		{
			m_capacityProgress.SetPos(100);
#if _MFC_VER >= 0x0900
			m_capacityProgress.SetState(PBST_ERROR);
#endif
		}
		else
		{
			m_capacityProgress.SetPos(percent);
#if _MFC_VER >= 0x0900
			m_capacityProgress.SetState(PBST_NORMAL);
#endif
		}

	}
}

void CCdBurnAppDlg::EnableBurnUI(BOOL enable)
{
	m_deviceComboBox.EnableWindow(enable);
	m_mediaTypeCombo.EnableWindow(enable);
	m_fileListbox.EnableWindow(enable);
	GetDlgItem(IDC_ADD)->EnableWindow(enable);
	GetDlgItem(IDC_FOLDER)->EnableWindow(enable);
	GetDlgItem(IDC_REMOVE)->EnableWindow(enable);
	GetDlgItem(IDC_BURN_BUTTON)->EnableWindow(enable);
	

}

void CCdBurnAppDlg::OnBnClickedButton1(){}

void CCdBurnAppDlg::OnDestroy()
{
	int itemCount = m_fileListbox.GetCount();
	for (int itemIndex = 0; itemIndex < itemCount; itemIndex++)
	{
		delete (Base*)m_fileListbox.GetItemDataPtr(itemIndex);
	}

	itemCount = m_deviceComboBox.GetCount();
	for (int itemIndex = 0; itemIndex < itemCount; itemIndex++)
	{
		delete (DiscRecorder*)m_deviceComboBox.GetItemDataPtr(itemIndex);
	}


	CDialog::OnDestroy();
}

void CCdBurnAppDlg::OnCbnSelchangeCombo2(){}

void CCdBurnAppDlg::OnCbnSelchangeCombo3()
{
	int selectedIndex = m_mediaTypeCombo.GetCurSel();
	if (selectedIndex == -1)
	{
		m_selectedMediaType = -1;
	}
	else
	{
		m_selectedMediaType = (int)m_mediaTypeCombo.GetItemData(selectedIndex);
	}
	UpdateCapacity();
}


void CCdBurnAppDlg::OnBnClickedAdd()
{
	CFileDialog fileDialog(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, _T("All Files (*.*)|*.*||"), NULL, 0);
	if (fileDialog.DoModal() == IDOK) {
		File* pFile = new File(fileDialog.GetPathName());
		int index = m_fileListbox.AddString(pFile->GetName());
		m_fileListbox.SetItemDataPtr(index, pFile);
		UpdateCapacity();
		enableBurnButton();

	}
}
void CCdBurnAppDlg::enableBurnButton()
{
	GetDlgItem(IDC_BURN_BUTTON)->EnableWindow(m_fileListbox.GetCount()>0);
}

void CCdBurnAppDlg::OnBnClickedBurnButton()
{
	if (m_isBurning)
	{

	}
	else
	{
		
		m_isBurning = true;
		UpdateData();
		EnableBurnUI(false);

		AfxBeginThread(BurnThread, this, THREAD_PRIORITY_NORMAL);
	}
}


void CCdBurnAppDlg::OnBnClickedFolder()
{
	BROWSEINFO bi = { 0 };
	bi.hwndOwner = this->m_hWnd; \
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (!pidl)
	{
		return;
	}
	TCHAR selectedPath[MAX_PATH];
	if (SHGetPathFromIDList(pidl, selectedPath))
	{
		Directory* pDirectory = new Directory(selectedPath);
		int index = m_fileListbox.AddString(pDirectory->GetName());
		m_fileListbox.SetItemDataPtr(index, pDirectory);
		UpdateCapacity();
		enableBurnButton();
	}
}


void CCdBurnAppDlg::OnBnClickedRemove()
{
	int selection = m_fileListbox.GetCurSel();
	if (selection != LB_ERR)
	{
		Base* pObject = (Base*)m_fileListbox.GetItemDataPtr(selection);
		delete pObject;
		m_fileListbox.DeleteString(selection);
		UpdateCapacity();
		OnLbnSelchangeList();
		enableBurnButton();
	}
}


void CCdBurnAppDlg::OnLbnSelchangeList()
{
	GetDlgItem(IDC_REMOVE)->EnableWindow(m_fileListbox.GetCurSel() != -1);
}


void CCdBurnAppDlg::OnCbnSelchangeMediaTypeCombo()
{
	int selectedIndex = m_mediaTypeCombo.GetCurSel();
	if (selectedIndex == -1)
	{
		m_selectedMediaType = -1;
	}
	else
	{
		m_selectedMediaType = (int)m_mediaTypeCombo.GetItemData(selectedIndex);
	}

	UpdateCapacity();
}
