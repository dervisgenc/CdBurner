// Directory.cpp : implementation file
//

#include "pch.h"
#include "CdBurnApp.h"
#include "Directory.h"


// Directory


IMPLEMENT_DYNAMIC(Directory, Base)

Directory::Directory(const CString& path) : Base(path)
{
	CFileFind fileFind;
	BOOL work = fileFind.FindFile(path + _T("\\*.*"));
	while (work)
	{	
		work = fileFind.FindNextFileW();
		if (fileFind.IsDots())
		{
			continue;
		}
		if (fileFind.IsDirectory())
		{
			Directory* pDirectory = new Directory(fileFind.GetFilePath());
		}
		else {
			Directory* pDirectory = new Directory(fileFind.GetFilePath());
		}

	}
}

Directory::~Directory()
{
	INT_PTR directoryCount = m_directoryArray.GetCount();
	for (INT_PTR i = 0; i < directoryCount; i++)
	{
		delete m_directoryArray.GetAt(i);
	}
	INT_PTR fileCount = m_fileArray.GetCount();
	for (INT_PTR j = 0; j < fileCount; j++)
	{
		delete m_fileArray.GetAt(j);
	}

}

// Directory message handlers

ULONGLONG Directory::GetSize() {
	ULONGLONG sizeOnDisc=0;
	
	INT_PTR directoryCount = m_directoryArray.GetCount();
	for (INT_PTR i = 0; i < directoryCount; i++)
	{
		sizeOnDisc += m_directoryArray.GetAt(i)->GetSize();
	}
	INT_PTR fileCount = m_fileArray.GetCount();
	for (INT_PTR j = 0; j < fileCount; j++)
	{
		sizeOnDisc += m_fileArray.GetAt(j)->GetSize();
	}
	return sizeOnDisc;
}

