#pragma once


// File

#include "Base.h"
class File : public Base 
{
	DECLARE_DYNAMIC(File)
protected:
	IStream *m_pStream; // stream for file
public:
	File(const CString& filepath);
	virtual ~File();

	virtual ULONGLONG GetSize();
	IStream* createStream();

};


