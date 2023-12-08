
// CdBurnAppDlg.h : header file
//

#pragma once
#include "FileList.h"

// CCdBurnAppDlg dialog
class CCdBurnAppDlg : public CDialogEx
{
// Construction
public:
	CCdBurnAppDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CDBURNAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	bool	m_isCdromSupported;
	bool	m_isDvdSupported;
	bool	m_isDualLayerDvdSupported;
	int		m_selectedMediaType;
	bool	m_isBurning;
	bool	m_cancelBurn;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//Implementation
	afx_msg void OnCbnSelchangeList();
	afx_msg void OnLbnSelchangeBurnFileList();
	afx_msg void OnBnClickedAddFilesButton();
	afx_msg void OnBnClickedAddFolderButton();
	void OnCbnSelchangeCombo2();
	void OnBnClickedButton1();
	CComboBox m_deviceComboBox;
	FileList m_fileListbox;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBurnButton();
	afx_msg LRESULT OnImapiUpdate(WPARAM, LPARAM);
	afx_msg LRESULT OnBurnStatusMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnBurnFinished(WPARAM, LPARAM);
	CString GetMediaTypeString(int);
	CStatic m_progressText;
	CStatic m_estimatedTime;
	CStatic m_timeLeft;
	CProgressCtrl m_progressCtrl;
	CStatic m_bufferText;
	CProgressCtrl m_bufferCtrl;
	CStatic m_supportedMediaTypes;
	CProgressCtrl m_capacityProgress;
	CStatic m_maxText;
	CComboBox m_mediaTypeCombo;
	void UpdateCapacity();
	afx_msg void OnCbnSelchangeMediaTypeCombo();
	afx_msg void OnBnClickedRemoveFilesButton();
	CString m_volumeLabel;
	BOOL m_closeMedia;
	BOOL m_ejectWhenFinished;
	void AddRecordersToComboBox();


	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedButton1();
	//afx_msg void OnCbnSelchangeCombo2();
	//afx_msg void OnCbnSelchangeList();
};
