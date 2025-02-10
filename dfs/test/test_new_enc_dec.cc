#include <iostream>
#include <string>
#include <memory>
#include <optional>
#include <sodium.h>
#include "stream_encrypt_decrypt.hh"
#include "util.hh"

int main() {
  const std::string dataFilePath = "../res/data.txt";
  fsn::StreamEncryptorDecryptor sed;

  auto fileLoadOpt = fsn::util::loadFileIntoBuffer(dataFilePath);
  if (fileLoadOpt == std::nullopt) {
    std::cout << "Failed to open data file.\n";
    return 1;
  }

  std::unique_ptr<std::vector<char>> data = std::move(fileLoadOpt.value());

  // Calculate initial hash
  std::vector<char> initialHash = fsn::util::primitive_calculateSHA512Hash(*data);

  // Encrypt the data
  std::vector<char> encryptedData = sed.xchacha20poly1305_encrypt(*data);
  // Decrypt the data
  auto decryptOpt = sed.xchacha20poly1305_decrypt(encryptedData, initialHash);
  if (decryptOpt == std::nullopt) {
    std::cout << "Decryption failed.\n";
    return 2;
  }

  return 0;
}