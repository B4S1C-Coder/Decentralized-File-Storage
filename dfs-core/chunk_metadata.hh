#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <memory>
#include <utility>
#include <iostream>
#include <endian.h>
#include "util.hh"

namespace fsn {

class ChunkMetadata {
public:
  ChunkMetadata(size_t dataEnd, std::vector<char>& hash, std::vector<char>& token)
    :m_dataEnd(dataEnd), m_hash(hash), m_token(token) {}

  std::vector<char> construct() {
    
    std::vector<char> metadata;

    // std::string dataEndStr = std::to_string(m_dataEnd);
    std::string dataEndStr = fsn::util::to_paddedString(m_dataEnd);
    size_t metadataSize = m_hash.size() + m_token.size() + dataEndStr.size() + sizeof(size_t);

    metadata.reserve(metadataSize);
    // metadata.resize(metadataSize);

    metadata.insert(metadata.end(), m_hash.begin(), m_hash.end());
    metadata.insert(metadata.end(), m_token.begin(), m_token.end());
    // metadata.insert(metadata.end(), reinterpret_cast<const char*>(&m_dataEnd),
    //   reinterpret_cast<const char*>(&m_dataEnd) + sizeof(size_t));

    // Converting to little-endian since size of size_t would differ from system to system
    // uint64_t dataEndLE = htole64(m_dataEnd);

    // std::copy(reinterpret_cast<const char*>(&dataEndLE),
    //   reinterpret_cast<const char*>(&dataEndLE) + sizeof(uint64_t), metadata.begin() + m_hash.size() + m_token.size());

    size_t strLen = dataEndStr.size();
    metadata.insert(metadata.end(), reinterpret_cast<const char*>(&strLen), reinterpret_cast<const char*>(&strLen) + sizeof(size_t));
    metadata.insert(metadata.end(), dataEndStr.begin(), dataEndStr.end());    

    return metadata;
  }

  static ChunkMetadata loadFromFile(
    std::ifstream& _if, const size_t hashLen, const size_t tokenLen
  ) {

    std::vector<char> hash(hashLen);
    _if.read(hash.data(), hash.size());

    std::vector<char> token(tokenLen);
    _if.read(token.data(), token.size());

    size_t strLen;
    _if.read(reinterpret_cast<char*>(&strLen), sizeof(size_t));

    // size_t dataEnd;
    // _if.read(reinterpret_cast<char*>(&dataEnd), sizeof(size_t));

    // uint64_t dataEndLE;
    // _if.read(reinterpret_cast<char*>(&dataEndLE), sizeof(uint64_t));
    // size_t dataEnd = le64toh(dataEndLE);

    std::vector<char> dataEndChars(strLen);
    _if.read(dataEndChars.data(), strLen);
    std::string dataEndStr(dataEndChars.begin(), dataEndChars.end());

    size_t dataEnd;
    try {
      dataEnd = std::stoull(dataEndStr);
    } catch (const std::exception& e) {
      throw std::runtime_error("Failed to convert data end string to integer: " + std::string(e.what()));
    }

    return ChunkMetadata(dataEnd, hash, token);
  }

  void printStatus() {
    std::cout << "METADATA STATUS\n-----------------\n";
    std::cout << "Data End : " << m_dataEnd <<"\n";
    std::cout << "Hash Size: " << m_hash.size() << "\n";
    std::cout << "Tokn Size: " << m_token.size() << "\n";
    std::cout << "-----------------\n";
  }

  size_t getDataEnd() const { return m_dataEnd;  }
  std::vector<char> getHash() const { return m_hash; }
  std::vector<char> getToken() const { return m_token; }

private:
  size_t m_dataEnd;
  std::vector<char> m_hash;
  std::vector<char> m_token;
};

}