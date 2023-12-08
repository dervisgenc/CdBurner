// FileList.cpp : implementation file
//

#include "pch.h"
#include "CdBurnApp.h"
#include "FileList.h"


// FileList

IMPLEMENT_DYNAMIC(FileList, CListBox)

FileList::FileList()
{

}

FileList::~FileList()
{
}

// FileList message handlers

BEGIN_MESSAGE_MAP(FileList, CListBox)
END_MESSAGE_MAP()



void FileList::PreSubclassWindow() {
	CListBox::PreSubclassWindow();
}


BOOL FileList::PreCreateWindow(CREATESTRUCT& cs) {
	cs.style |= LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP;
	return CListBox::PreCreateWindow(cs);

}

void FileList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	if (lpDrawItemStruct->itemID == -1)
		return;

	CString strFile;
	CObject* pObject = (CObject*)GetItemData(lpDrawItemStruct->itemID);
	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return;

	if (!pObject->IsKindOf(RUNTIME_CLASS(Base)))
		return;

	Base* pBaseObject = (Base*)pObject;
	strFile = pBaseObject->GetPath();


	SHFILEINFO shFileInfo = { 0 };
	::SHGetFileInfo(strFile, 0, &shFileInfo, sizeof(SHFILEINFO),
		SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON);

	CRect rect(lpDrawItemStruct->rcItem);
	HBRUSH hBrush;
	COLORREF clrText;
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		hBrush = GetSysColorBrush(COLOR_HIGHLIGHT);
		clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else
	{
		hBrush = GetSysColorBrush(COLOR_WINDOW);
		clrText = GetSysColor(COLOR_WINDOWTEXT);
	}
	::FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, hBrush);

	if (lpDrawItemStruct->itemState & ODS_FOCUS)
		::DrawFocusRect(lpDrawItemStruct->hDC, &rect);

	DrawIconEx(lpDrawItemStruct->hDC, rect.left + 4, rect.top + 4, shFileInfo.hIcon,
		16, 16, 0, NULL, DI_NORMAL);

	COLORREF oldColor = ::SetTextColor(lpDrawItemStruct->hDC, clrText);
	int nOldMode = ::SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);

	rect.left += 24;
	DrawText(lpDrawItemStruct->hDC, shFileInfo.szDisplayName,
		(int)_tcslen(shFileInfo.szDisplayName), &rect,
		DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

	::SetBkMode(lpDrawItemStruct->hDC, nOldMode);
	::SetTextColor(lpDrawItemStruct->hDC, oldColor);
	

}

void FileList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) {
	lpMeasureItemStruct->itemHeight = 20;
}


