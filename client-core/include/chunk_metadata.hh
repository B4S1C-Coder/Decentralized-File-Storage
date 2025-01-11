#include <ostream>
#include <string>

namespace fsn {

class ChunkMetadata {
public:
private:
  size_t m_dataLen;
  size_t m_dataStart;
  size_t m_dataEnd;
  size_t m_hashLen;
  size_t m_tokenLen;
};

}