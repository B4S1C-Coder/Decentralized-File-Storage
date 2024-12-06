#pragma once

#include <string>
#include <utility>
#include <optional>
#include <fstream>
#include <sodium.h>

namespace fsn {

class StreamEncryptorDecryptor {
public:
  StreamEncryptorDecryptor(
    std::optional<std::pair<std::string, std::string>> keyFileNonceFile = std::nullopt
  );
  void printStatus();
private:
  unsigned char m_nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];
  unsigned char m_key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];
};

}