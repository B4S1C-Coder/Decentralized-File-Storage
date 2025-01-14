#include <iostream>
#include <string>
#include <memory>
#include <sodium.h>
#include "chunk_metadata.hh"
#include "sequential_file_splitter.hh"
#include "util.hh"

std::string dataFilePath = "../res/data.txt";
const size_t tokenLength = 32;

void hardCodedReconstruct() {
  // To verify whether the unencrypted with metadata token split was actually done properly
  // This would be checked by recombining the chunks and manually recombining the chunks whilst separating
  // the metadata and ensuring that metadata was correctly written and can be correctly loaded and parsed.

  // This is the manual recombiniation.

  std::unique_ptr<std::ifstream> chunkFile = std::make_unique<std::ifstream>("1.fsnc", std::ios::binary);
  
  auto metadataLoadingResultPair = fsn::ChunkMetadata::loadFromFile(
    std::move(chunkFile), crypto_hash_sha512_BYTES, tokenLength
  );

  chunkFile = std::move(metadataLoadingResultPair.first);
  fsn::ChunkMetadata metadata = metadataLoadingResultPair.second;

  size_t currentLoc = chunkFile->tellg();

  std::cout << "Current File pointer location> " << currentLoc << "\n";

  // auto bufferOpt = fsn::util::loadFileIntoBuffer("1.fsnc");
}

int main() {
  if (sodium_init() == -1) {
    std::cout << "Sodium initialization failed.\n";
    return -1;
  }

  // fsn::SequentialFileSplitter sfs(dataFilePath, 5);
  // std::cout << "Successful Linking\n";

  hardCodedReconstruct();
  return 0;
}