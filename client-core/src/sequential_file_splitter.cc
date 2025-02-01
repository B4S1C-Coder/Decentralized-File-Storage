#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "sequential_file_splitter.hh"
#include "util.hh"
#include "chunk_metadata.hh"
#include "diagnostics.hh"
#include "logger.hh"

fsn::SequentialFileSplitter::SequentialFileSplitter(const std::string& filePath, unsigned int numChunks)
: m_numChunks(numChunks), m_bytesInFile(0), m_bytesPerChunks(0), m_currentFilePath(filePath) {
  
  m_currentFile = std::make_unique<std::ifstream>(filePath, std::ios::binary);
  if (!m_currentFile->is_open()) {
    throw std::runtime_error("Failed to open: " + filePath);
  }

  // Determine file size
  m_currentFile->seekg(0, std::ios::end);
  m_bytesInFile = m_currentFile->tellg();
  m_currentFile->seekg(0, std::ios::beg);

  if (numChunks <= 0) {
    throw std::runtime_error("Invalid numChunks value.");
  }

  m_bytesPerChunks = m_bytesInFile / numChunks;
}

void fsn::SequentialFileSplitter::printStatus() {
  std::cout << "Current File     : " << m_currentFilePath << "\n";
  std::cout << "Is file open     : " << m_currentFile->is_open() << "\n";
  std::cout << "Number of Chunks : " << m_numChunks << "\n";
  std::cout << "Bytes in File    : " << m_bytesInFile << "\n";
  std::cout << "Bytes per Chunks : " << m_bytesPerChunks << std::endl;
}

int fsn::SequentialFileSplitter::singleThreadedSplit(const std::string& outputDirPath) {
  std::vector<char> buffer;
  buffer.reserve(m_bytesPerChunks);
  int chunkCount = 1;

  std::vector<char> token = fsn::util::primitive_generateRandomToken();
  // fsn::diagnostics::checkNullBytesInBuffer("Token Buffer", token);
  fsn::logger::consoleLog("Token generated.");

  while (true) {
    buffer.resize(m_bytesPerChunks);
    m_currentFile->read(buffer.data(), m_bytesPerChunks);
    std::streamsize dataSize = m_currentFile->gcount();
    fsn::logger::consoleLog("Characters Extracted: " + std::to_string(dataSize));

    if (dataSize == 0) {
      break;
    }

    buffer.resize(dataSize);

    // fsn::diagnostics::checkNullBytesInBuffer("Data Buffer", buffer);
    fsn::logger::consoleLog("Data buffer loaded. (UNENCRYPTED)");

    std::string chunkFileName = outputDirPath + "/" + std::to_string(chunkCount++) + ".fsnc";

    // Calculate hash of the data
    std::vector<char> hash = fsn::util::primitive_calculateSHA512Hash(buffer);
    // fsn::diagnostics::checkNullBytesInBuffer("Hash Buffer", hash);
    fsn::logger::consoleLog("Hash Calculated");

    size_t dataEnd = hash.size() + token.size() + sizeof(size_t) + std::to_string(dataSize).size() + buffer.size() - 1;

    // Construct the metadata for the chunk
    ChunkMetadata metadata(dataEnd, hash, token);
    std::vector<char> metedata_bytes = metadata.construct();

    // fsn::diagnostics::checkNullBytesInBuffer("Meta Data Buffer", metedata_bytes);
    fsn::logger::consoleLog("Meta Data constricted. Data End: " + std::to_string(dataEnd));

    // Create the final buffer to be written (without encryption)
    std::vector<char> unencrypted_final;
    unencrypted_final.reserve(metedata_bytes.size() + buffer.size());
    unencrypted_final.insert(unencrypted_final.end(), metedata_bytes.begin(), metedata_bytes.end());
    unencrypted_final.insert(unencrypted_final.end(), buffer.begin(), buffer.end());

    fsn::logger::consoleLog("Final Chunk Size: " + std::to_string(unencrypted_final.size()));
    fsn::logger::consoleLog("Meta Data Buffer Size: " + std::to_string(metedata_bytes.size()));
    fsn::logger::consoleLog("Hash Buffer Size: " + std::to_string(hash.size()));
    fsn::logger::consoleLog("Token Buffer Size: " + std::to_string(token.size()));
    fsn::logger::consoleLog("Data Buffer Size: " + std::to_string(buffer.size()));

    std::ofstream outfile(chunkFileName, std::ios::binary);
    
    if (!outfile.is_open()) {
      std::cout << "Unable to write chunks to disk.\n";
      return -1;
    }

    outfile.write(unencrypted_final.data(), unencrypted_final.size());
    outfile.close();

    fsn::logger::consoleLog("Chunk written to disk.");

    std::cout << "-------------------\n";
  }

  return 0;
}