#pragma once


class Encrypt
{

private:
	static const size_t AES_KEY_LENGTH = 16;
	std::string m_cdID;
	std::vector<unsigned char> m_aesKey;
	std::vector<unsigned char> m_iv;
	EVP_CIPHER_CTX* m_ctx;

	void GenerateRandomAESKey();
	std::string EncryptData(const std::string& data);
public:
	Encrypt(const std::string& cdID);
	virtual ~Encrypt();
	bool EmbedDataToCD(const std::string& data, IStream* cdStream);

};

