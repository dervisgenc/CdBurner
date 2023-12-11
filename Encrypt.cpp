// Encrypt.cpp : implementation file
//

#include "pch.h"
#include "CdBurnApp.h"
#include "Encrypt.h"


// Encrypt



Encrypt::Encrypt(const std::string& cdID) : m_cdID(cdID)
{
    // Şifreleme için bir EVP_CIPHER_CTX yapısı oluştur
    m_ctx = EVP_CIPHER_CTX_new();
    if (m_ctx == NULL) {
        // Hata işleme
    }
}

Encrypt::~Encrypt()
{
    // EVP_CIPHER_CTX yapısını serbest bırak
    EVP_CIPHER_CTX_free(m_ctx);
}

void Encrypt::GenerateRandomAESKey()
{
    // AES anahtarını ve IV'yi rastgele oluştur
    m_aesKey.resize(AES_KEY_LENGTH);
    m_iv.resize(AES_BLOCK_SIZE);
    RAND_bytes(reinterpret_cast<unsigned char*>(&m_aesKey[0]), AES_KEY_LENGTH);
    RAND_bytes(reinterpret_cast<unsigned char*>(&m_iv[0]), AES_BLOCK_SIZE);
}

std::string Encrypt::EncryptData(const std::string& data) {
    // Veriyi AES ile şifrele
    std::string encryptedData;
    int len, outlen;

    // Şifreleme işlemini başlat
    if (!EVP_EncryptInit(m_ctx, EVP_aes_256_cbc(), reinterpret_cast<const unsigned char*>(m_aesKey.data()), reinterpret_cast<const unsigned char*>(m_iv.data()))) {
        // Hata işleme
    }

    // Veriyi şifrele
    encryptedData.resize(data.length() + AES_BLOCK_SIZE);  // AES_BLOCK_SIZE kadar genişletildi
    if (!EVP_EncryptUpdate(m_ctx, reinterpret_cast<unsigned char*>(&encryptedData[0]), &len, reinterpret_cast<const unsigned char*>(data.data()), data.length())) {
        // Hata işleme
    }
    outlen = len;

    // Şifreleme işlemini bitir
    if (!EVP_EncryptFinal(m_ctx, reinterpret_cast<unsigned char*>(&encryptedData[0]) + len, &len)) {
        // Hata işleme
    }
    outlen += len;

    // Şifreli veriyi yeniden boyutlandır
    encryptedData.resize(outlen);

    return encryptedData;
}


bool Encrypt::EmbedDataToCD(const std::string& data, IStream* cdStream) {
    // Veriyi şifrele
    std::string encryptedData = EncryptData(data);

    // Şifreli veriyi base64 formatına dönüştür
    std::string encodedData;
    encodedData.resize(EVP_ENCODE_LENGTH(encryptedData.size()));
    int len;
    EVP_EncodeBlock(reinterpret_cast<unsigned char*>(&encodedData[0]), reinterpret_cast<const unsigned char*>(encryptedData.data()), encryptedData.size());
    len = strlen(encodedData.c_str());

    // CD'ye benzersiz tanımlayıcıyı ekleyerek veriyi göm
    std::string embeddedData = m_cdID + encodedData;

    // Gömülen veriyi CD'ye yaz
    ULONG bytesWritten;
    cdStream->Write(embeddedData.c_str(), embeddedData.size(), &bytesWritten);

    // Gömme işlemi başarıyla tamamlandıysa true döndür
    return bytesWritten == embeddedData.size();
}




