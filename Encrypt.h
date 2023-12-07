#pragma once


// Encrypt
//EncrytpionInterface
class Encrypt 
{
	
protected:
	IStream* m_stream;
	Encrypt();
	
public:
	virtual ~Encrypt();
	virtual void EncryptData() = 0;

};


