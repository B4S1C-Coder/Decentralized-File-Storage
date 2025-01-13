#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "sequential_file_splitter.hh"
#include "util.hh"
#include "chunk_metadata.hh"

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
  std::vector<char> buffer(m_bytesPerChunks, 0);
  int chunkCount = 1;

  auto token = std::move(fsn::util::generateRandomToken());

  while (true) {
    m_currentFile->read(buffer.data(), buffer.size());
    std::streamsize dataSize = m_currentFile->gcount();

    if (dataSize == 0) {
      break;
    }

    std::string chunkFileName = outputDirPath + "/" + std::to_string(chunkCount++) + ".fsnc";

    // Calculate hash of the data
    auto hash = std::move(fsn::util::calculateSHA512Hash(buffer));

    // Construct the metadata for the chunk
    ChunkMetadata metadata(hash->size() + token->size() + dataSize, *hash, *token);
    std::vector<char> metedata_bytes = metadata.construct();

    // Create the final buffer to be written (without encryption)
    std::vector<char> unencrypted_final(metedata_bytes.size() + buffer.size());
    unencrypted_final.insert(unencrypted_final.end(), metedata_bytes.begin(), metedata_bytes.end());
    unencrypted_final.insert(unencrypted_final.end(), buffer.begin(), buffer.end());

    std::ofstream outfile(chunkFileName, std::ios::binary);
    
    if (!outfile.is_open()) {
      std::cout << "Unable to write chunks to disk.\n";
      return -1;
    }

    outfile.write(unencrypted_final.data(), unencrypted_final.size());
    outfile.close();

    // Encrypt the buffer here (ignore for now)

    // DEBUG only
    std::cout << dataSize << "\n";
    std::cout << "\n-------------------\n";

    for (auto i: buffer) {
      std::cout << i;
    }

    std::cout << "\n-------------------\n";
  }

  return 0;
}