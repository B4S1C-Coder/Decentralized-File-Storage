#include <sodium.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <utility>
#include "stream_encrypt_decrypt.hh"
#include "sequential_file_splitter.hh"

void sfs_sandbox() {
  fsn::SequentialFileSplitter sfs("data.txt", 5);
  sfs.printStatus();
}

void sed_sandbox() {
  fsn::StreamEncryptorDecryptor sed;
  sed.printStatus();

  std::unique_ptr<unsigned char[]> message = std::make_unique<unsigned char[]>(13);
  auto debug_only_message_copy_raw = message.get(); // Used for debugging only, extremely DANGEROUS!
  std::memcpy(message.get(), "Test message", 13);

  std::unique_ptr<unsigned char[]> messageHash = std::make_unique<unsigned char[]>(crypto_hash_sha512_BYTES);
  crypto_hash_sha512(messageHash.get(), message.get(), 13);

  auto result = sed.encrypt(std::move(message), 13); // After this the raw pointer of message is nullptr
  // so message is essentially no longer valid or more precisely just an empty unique_ptr. It should no
  // longer be used after this line.
  
  auto decryptionResult = sed.decrypt(std::move(result.first), result.second, std::move(messageHash), 13);

  if (!decryptionResult.has_value()) {
    std::cout << "Decryption failed." << std::endl;
    return;
  }

  std::unique_ptr<unsigned char[]> decrypted = std::move(decryptionResult.value().first);

  // Comparing the original and decrypted data
  if (decryptionResult.value().second == 13 && memcmp(debug_only_message_copy_raw, decrypted.get(), 13) == 0) {
    std::cout << "Decryption successful, messages match." << std::endl;
  } else {
    std::cout << "Decrypted messages do not match." << std::endl;
    return;
  }

  std::cout << "Ran successfully ... OK!" << std::endl;
}

void normalSodiumUsage() {

  if (sodium_init() == -1) {
    std::cout << "Failed to initialize sodium." << std::endl;
    return;
  }

  const unsigned char* message = (const unsigned char*)"SCA Secret message";
  const size_t messageLen = strlen(reinterpret_cast<const char*>(message));

  const unsigned char* additionalData = (const unsigned char*)"SCA-069";
  const size_t addDataLen = strlen(reinterpret_cast<const char*>(additionalData));

  unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
  unsigned char key[crypto_aead_aes256gcm_KEYBYTES];

  unsigned char ciphertext[messageLen + crypto_aead_aes256gcm_ABYTES];
  unsigned long long ciphertextLen;

  if (crypto_aead_aes256gcm_is_available() == 0) {
    std::cout << "AES256GCM not available on this CPU" << std::endl;
    abort();
  }

  crypto_aead_aes256gcm_keygen(key);
  randombytes_buf(nonce, sizeof(nonce));

  // Encrypt the message
  crypto_aead_aes256gcm_encrypt(
    ciphertext, &ciphertextLen,
    message, messageLen,
    additionalData, addDataLen,
    NULL, nonce, key
  );

  unsigned char decrypted[messageLen];
  unsigned long long decryptedLen;

  // Decrypt the message
  if (ciphertextLen < crypto_aead_aes256gcm_ABYTES ||
      crypto_aead_aes256gcm_decrypt(decrypted, &decryptedLen,
      NULL, ciphertext, ciphertextLen,
      additionalData, addDataLen, nonce, key)
  ) {
    std::cout << "Message forged!" << std::endl;
    abort();
  }

  if (decryptedLen == messageLen &&
      memcmp(message, decrypted, messageLen) == 0) {
    std::cout << "Decryption successful, messages match!" << std::endl;
    std::cout << decrypted << std::endl;
  } else {
    std::cout << "Decryption failed, messages do not match!" << std::endl;
  }

  std::cout << "Ran without runtime errors." << std::endl;
}

int main() {
  // sed_sandbox();
  sfs_sandbox();
  return 0;
}