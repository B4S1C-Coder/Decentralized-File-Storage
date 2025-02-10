#pragma once

#include <string>
#include <utility>
#include <optional>
#include <fstream>
#include <memory>
#include <cstring>
#include <vector>
#include <sodium.h>

namespace fsn {

class StreamEncryptorDecryptor {
public:
  StreamEncryptorDecryptor(
    std::optional<std::pair<std::string, std::string>> keyFileNonceFile = std::nullopt
  );
  void printStatus();

  std::pair<std::unique_ptr<unsigned char[]>, size_t> encrypt(
    std::unique_ptr<unsigned char[]> message, size_t messageLen
  );

  std::vector<char> xchacha20poly1305_encrypt(std::vector<char>& inputBuffer);
  std::optional<std::vector<char>> xchacha20poly1305_decrypt(std::vector<char>& encryptedBuffer, std::vector<char>& messageHash);

  std::optional<std::pair<std::unique_ptr<unsigned char[]>, size_t>> decrypt(
    std::unique_ptr<unsigned char[]> encrypted, size_t encryptedLen,
    std::unique_ptr<unsigned char[]> messasgeSHA512Hash, size_t messageLen
  );

private:
  unsigned char m_nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];
  unsigned char m_key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];
};

}