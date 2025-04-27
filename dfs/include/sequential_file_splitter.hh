#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <stdexcept>
#include <vector>
#include "stream_encrypt_decrypt.hh"

namespace fsn {

class SequentialFileSplitter {
public:
  SequentialFileSplitter(const std::string& filePath, unsigned int numChunks);
  void printStatus();
  int singleThreadedSplit(const std::string& outputDirPath);
  void setStreamEncryptorDecryptor(fsn::StreamEncryptorDecryptor sed);

  std::vector<std::string> getChunkFilePaths() { return m_chunkFiles; }

private:
  std::unique_ptr<std::ifstream> m_currentFile;
  unsigned int m_numChunks;
  size_t m_bytesPerChunks;
  size_t m_bytesInFile;
  std::string m_currentFilePath;
  fsn::StreamEncryptorDecryptor m_streamEncDec;
  std::vector<std::string> m_chunkFiles;
};

}