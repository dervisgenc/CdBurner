// File.cpp : implementation file
//

#include "pch.h"
#include "CdBurnApp.h"
#include "File.h"

#define SECTOR_SIZE	2048
// File

IMPLEMENT_DYNAMIC(File, Base)

File::File(const CString& path) : Base(path)
{
	m_pStream = NULL;
}

File::~File()
{
	if (m_pStream != NULL)
	{
		m_pStream->Release();
	}
}

//CreateStreamOnHGlobal
IStream* File::createStream() {
	if (m_pStream == NULL )
	{
		 SHCreateStreamOnFileEx(m_path,
            STGM_READ | STGM_SHARE_DENY_NONE | STGM_DELETEONRELEASE,
            FILE_ATTRIBUTE_NORMAL,
            FALSE,
            NULL,
            &m_pStream);
	}
    return m_pStream;
}
ULONGLONG File::GetSize() {
	CFileStatus fileStatus;
	if (CFile::GetStatus(m_path, fileStatus)) {
		if (fileStatus.m_size>0)
		{
			return ((fileStatus.m_size / SECTOR_SIZE) + 1) * SECTOR_SIZE;
		}
	}
	return 0;
}

