#pragma once


// FileList
#include "FileList.h"
#include "Directory.h"
#include "File.h"

class FileList : public CListBox
{
	DECLARE_DYNAMIC(FileList)

public:
	FileList();
	virtual ~FileList();
	void PreSubclassWindow();
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

protected:
	DECLARE_MESSAGE_MAP()
};


