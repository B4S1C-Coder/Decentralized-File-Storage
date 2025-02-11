#include <iostream>
#include <string>
#include <memory>
#include <optional>
#include <sodium.h>
#include "stream_encrypt_decrypt.hh"
#include "util.hh"

int main() {

  if (sodium_init() == -1) {
    std::cout << "Sodium initialization failed.\n";
    return -1;
  }

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

  // Calculate final hash (hash of the decrypted data)
  std::vector<char> finalHash = fsn::util::primitive_calculateSHA512Hash(decryptOpt.value());

  bool hashesMatch = (sodium_memcmp(initialHash.data(), finalHash.data(), crypto_hash_sha512_BYTES) == 0);

  if (hashesMatch) {
    std::cout << "Hashes match.\n";
  } else {
    std::cout << "Hashes do not match.\n";
  }

  return 0;
}