#include <string>
#include <utility>
#include <optional>
#include <fstream>
#include <sodium.h>
#include <iostream>
#include <memory>
#include <cstring>
#include <vector>
#include "stream_encrypt_decrypt.hh"
#include <dfs-core.hh>

fsn::StreamEncryptorDecryptor::StreamEncryptorDecryptor(
  std::optional<std::pair<std::string, std::string>> keyFileNonceFile
) {

  if (sodium_init() == -1) {
    std::cout << "Sodium initialization failed." << std::endl;
    abort();
  }

  if (keyFileNonceFile == std::nullopt) {
    // Generate key and nonce
    crypto_aead_xchacha20poly1305_ietf_keygen(m_key);
    randombytes_buf(m_nonce, sizeof(m_nonce));
  } else {
    // Load key and nonce from file
    auto [keyFile, nonceFile] = keyFileNonceFile.value();
    std::ifstream keyStream(keyFile, std::ios::binary);
    std::ifstream nonceStream(nonceFile, std::ios::binary);

    if (!keyStream || !nonceStream) {
      std::string error_message = "Failed to open: ";
      if (!keyStream) { error_message += " "; error_message += keyFile; }
      if (!nonceStream) { error_message += " "; error_message += nonceFile; }
      throw std::runtime_error(error_message);
    }

    keyStream.read(reinterpret_cast<char*>(m_key), sizeof(m_key));
    nonceStream.read(reinterpret_cast<char*>(m_nonce), sizeof(m_nonce));
  }
}

void fsn::StreamEncryptorDecryptor::printStatus() {
  if (m_key)
    std::cout << "m_key is present" << std::endl;
  if (m_nonce)
    std::cout << "nonce is present" << std::endl;
}

void fsn::StreamEncryptorDecryptor::dumpKeyAndNonce() {
  std::ofstream keyFile("key.bin", std::ios::binary);

  if (keyFile) {
    keyFile.write(reinterpret_cast<const char*>(m_key), sizeof(m_key));
    keyFile.close();
  } else {
    fsn::logger::consoleLog("Failed to dump key.", fsn::logger::ERROR);
  }

  std::ofstream nonceFile("nonce.bin", std::ios::binary);

  if (nonceFile) {
    nonceFile.write(reinterpret_cast<const char*>(m_nonce), sizeof(m_nonce));
    nonceFile.close();
  } else {
    fsn::logger::consoleLog("Failed to dump nonce", fsn::logger::ERROR);
  }
}

std::pair<std::unique_ptr<unsigned char[]>, size_t> fsn::StreamEncryptorDecryptor::encrypt(
  std::unique_ptr<unsigned char[]> message, size_t messageLen
) {
  std::unique_ptr<unsigned char[]> encrypted = std::make_unique<unsigned char[]>(
    messageLen + crypto_aead_xchacha20poly1305_ietf_ABYTES);

  unsigned long long encrypted_len; // essentially equal to the length of unsigned char[] above

  // Calculating SHA512 hash of message to server as additional data to enforce integrity
  unsigned char messageHash[crypto_hash_sha512_BYTES];
  crypto_hash_sha512(messageHash, message.get(), messageLen);

  crypto_aead_xchacha20poly1305_ietf_encrypt(
    encrypted.get(), &encrypted_len, message.get(), messageLen,
    messageHash, sizeof(messageHash), NULL, m_nonce, m_key
  );

  // std::make_pair was causing some problems with std::unique_ptr<unsigned char[]>
  return std::pair<std::unique_ptr<unsigned char[]>, size_t>(std::move(encrypted), encrypted_len);
}

std::vector<char> fsn::StreamEncryptorDecryptor::xchacha20poly1305_encrypt(std::vector<char>& inputBuffer) {

  std::vector<char> encryptedBuffer(inputBuffer.size() + crypto_aead_xchacha20poly1305_ietf_ABYTES);
  std::vector<char> inputBufferHash = fsn::util::primitive_calculateSHA512Hash(inputBuffer);

  unsigned long long encrypted_len = 0;

  crypto_aead_xchacha20poly1305_ietf_encrypt(
    reinterpret_cast<unsigned char*>(encryptedBuffer.data()), &encrypted_len,
    reinterpret_cast<unsigned char*>(inputBuffer.data()), inputBuffer.size(),
    reinterpret_cast<unsigned char*>(inputBufferHash.data()), inputBufferHash.size(), NULL, m_nonce, m_key
  );

  encryptedBuffer.resize(encrypted_len);

  return encryptedBuffer;
}

std::optional<std::vector<char>> fsn::StreamEncryptorDecryptor::xchacha20poly1305_decrypt(
  std::vector<char>& encryptedBuffer, std::vector<char>& messageHash
) {
  std::vector<char> decryptedBuffer(encryptedBuffer.size());
  unsigned long long decrypted_len = 0;

  bool decryptionSuccess = (crypto_aead_xchacha20poly1305_ietf_decrypt(
    reinterpret_cast<unsigned char*>(decryptedBuffer.data()), &decrypted_len, NULL,
    reinterpret_cast<unsigned char*>(encryptedBuffer.data()), encryptedBuffer.size(),
    reinterpret_cast<unsigned char*>(messageHash.data()), crypto_hash_sha512_BYTES, m_nonce, m_key
  ) == 0);

  if (decryptionSuccess) {
    decryptedBuffer.resize(decrypted_len);
    return decryptedBuffer;
  } else {
    return std::nullopt;
  }
}

std::optional<std::pair<std::unique_ptr<unsigned char[]>, size_t>> fsn::StreamEncryptorDecryptor::decrypt(
  std::unique_ptr<unsigned char[]> encrypted, size_t encryptedLen,
  std::unique_ptr<unsigned char[]> messasgeSHA512Hash, size_t messageLen
) {
  std::unique_ptr<unsigned char[]> decrypted = std::make_unique<unsigned char[]>(messageLen);
  unsigned long long decrypted_len;

  if (crypto_aead_xchacha20poly1305_ietf_decrypt(
    decrypted.get(), &decrypted_len, NULL,
    encrypted.get(), encryptedLen,
    messasgeSHA512Hash.get(), crypto_hash_sha512_BYTES,
    m_nonce, m_key) != 0) {
      // Message has been forged. Decryption failed.
      return std::nullopt;
    }
  
  return std::pair<std::unique_ptr<unsigned char[]>, size_t>(std::move(decrypted), decrypted_len);
}