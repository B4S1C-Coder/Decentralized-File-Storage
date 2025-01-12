#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <memory>
#include <utility>

namespace fsn {

class ChunkMetadata {
public:
  ChunkMetadata(size_t dataEnd, std::vector<char>& hash, std::vector<char>& token)
    :m_dataEnd(dataEnd), m_hash(hash), m_token(token) {}

  std::vector<char> construct() {
    
    std::vector<char> metadata(m_hash.size() + m_token.size() + sizeof(size_t));

    metadata.insert(metadata.end(), m_hash.begin(), m_hash.end());
    metadata.insert(metadata.end(), m_token.begin(), m_token.end());
    metadata.insert(metadata.end(), reinterpret_cast<const char*>(&m_dataEnd),
      reinterpret_cast<const char*>(&m_dataEnd) + sizeof(size_t));

    return metadata;
  }

  static std::pair<std::unique_ptr<std::ifstream>, ChunkMetadata> loadFromFile(
    std::unique_ptr<std::ifstream> _if, const size_t hashLen, const size_t tokenLen
  ) {

    std::vector<char> hash(hashLen);
    _if->read(hash.data(), hash.size());

    std::vector<char> token(tokenLen);
    _if->read(token.data(), token.size());

    size_t dataEnd;
    _if->read(reinterpret_cast<char*>(&dataEnd), sizeof(size_t));

    std::pair<std::unique_ptr<std::ifstream>, ChunkMetadata> p = std::make_pair(
      std::move(_if), ChunkMetadata(dataEnd, hash, token));
    return p;
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