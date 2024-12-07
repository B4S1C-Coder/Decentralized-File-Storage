#include <string>
#include <utility>
#include <optional>
#include <fstream>
#include <sodium.h>
#include <iostream>
#include <memory>
#include <cstring>
#include "stream_encrypt_decrypt.hh"

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