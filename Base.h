#pragma once

// Base command target

class Base : public CObject
{
protected:
	CString m_path;
	Base(const CString& filepath);
public:
	virtual ~Base();
	inline CString GetPath(){return m_path;}
	virtual CString GetName();
	virtual ULONGLONG GetSize() = 0;
};


