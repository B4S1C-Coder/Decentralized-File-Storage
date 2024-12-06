#include <string>
#include <utility>
#include <optional>
#include <fstream>
#include <sodium.h>
#include <iostream>
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