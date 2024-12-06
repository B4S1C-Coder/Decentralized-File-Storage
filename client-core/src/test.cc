#include <sodium.h>
#include <cstring>
#include <iostream>
#include "stream_encrypt_decrypt.hh"

int main() {

  if (sodium_init() == -1) {
    return 1;
  }

  fsn::StreamEncryptorDecryptor sed;
  sed.printStatus();

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

  return 0;
}