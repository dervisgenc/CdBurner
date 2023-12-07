#pragma once


// Directory

#include "File.h"

class Directory : Base
{
	DECLARE_DYNAMIC(Directory)
protected:
	CTypedPtrArray<CObArray,File*> m_fileArray;
	CTypedPtrArray<CObArray,Directory*> m_directoryArray;
public:
	Directory(const CString& path);
	virtual ~Directory();

	ULONGLONG GetSize();
};


