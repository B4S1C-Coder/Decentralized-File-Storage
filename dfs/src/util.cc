#include <optional>
#include <memory>
#include <vector>
#include <fstream>
#include <sodium.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "util.hh"

std::optional<std::unique_ptr<std::vector<char>>> fsn::util::loadFileIntoBuffer(const std::string& path) {
  std::ifstream file(path, std::ios::binary);

  if(!file.is_open()) {
    return std::nullopt;
  }

  file.seekg(0, std::ios::end);
  size_t fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  std::unique_ptr<std::vector<char>> buffer = std::make_unique<std::vector<char>>(fileSize, 0);
  file.read(buffer->data(), buffer->size());
  file.close();

  return buffer;
}

std::unique_ptr<std::vector<char>> fsn::util::calculateSHA512Hash(std::vector<char>& inputBuffer) {
  std::unique_ptr<std::vector<char>> hashBuffer = std::make_unique<std::vector<char>>();
  hashBuffer->reserve(crypto_hash_sha512_BYTES);

  crypto_hash_sha512(
    reinterpret_cast<unsigned char*>(hashBuffer->data()),
    reinterpret_cast<unsigned char*>(inputBuffer.data()),
    inputBuffer.size()
  );

  return hashBuffer;
}

std::vector<char> fsn::util::primitive_calculateSHA512Hash(std::vector<char>& inputBuffer) {
  std::vector<char> hashBuffer;
  hashBuffer.reserve(crypto_hash_sha512_BYTES);
  hashBuffer.resize(crypto_hash_sha512_BYTES);

  crypto_hash_sha512(
    reinterpret_cast<unsigned char*>(hashBuffer.data()),
    reinterpret_cast<unsigned char*>(inputBuffer.data()),
    inputBuffer.size()
  );

  return hashBuffer;
}

std::unique_ptr<std::vector<char>> fsn::util::generateRandomToken() {
  std::unique_ptr<std::vector<char>> token = std::make_unique<std::vector<char>>(32);
  randombytes_buf(reinterpret_cast<unsigned char*>(token->data()), 32);
  return token;
}

std::vector<char> fsn::util::primitive_generateRandomToken() {
  std::vector<char> token;
  token.reserve(32);
  token.resize(32);
  randombytes_buf(reinterpret_cast<unsigned char*>(token.data()), 32);
  return token;
}

std::string fsn::util::to_paddedString(unsigned long long num, int width) {
  std::ostringstream oss;
  oss << std::setw(width) << std::setfill('0') << num;
  return oss.str();
}

std::string fsn::util::bytesToHex(const std::vector<char>& bytes) {
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');

  for (unsigned char byte: bytes) {
    oss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(byte));
  }

  return oss.str();
}