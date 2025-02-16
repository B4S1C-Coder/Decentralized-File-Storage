#include <iostream>
#include <string>
#include <memory>
#include <sodium.h>
#include "chunk_metadata.hh"
#include "sequential_file_splitter.hh"
#include "util.hh"
#include "diagnostics.hh"
#include "logger.hh"

std::string dataFilePath = "../res/data.txt";
const size_t tokenLength = 32;

void hardCodedReconstruct() {

  if (sodium_init() == -1) {
    std::cout << "Sodium initialization failed.\n";
    return;
  }

  // To verify whether the unencrypted with metadata token split was actually done properly
  // This would be checked by recombining the chunks and manually recombining the chunks whilst separating
  // the metadata and ensuring that metadata was correctly written and can be correctly loaded and parsed.

  // This is the manual recombiniation.
  std::ifstream chunkFile("1.fsnc", std::ios::binary);

  fsn::ChunkMetadata chunkMetadata = fsn::ChunkMetadata::loadFromFile(
    chunkFile, crypto_hash_sha512_BYTES, tokenLength
  );
  size_t currentLoc = chunkFile.tellg();

  fsn::logger::consoleLog("Current File pointer location> " + std::to_string(currentLoc));

  std::vector<char> hash = chunkMetadata.getHash();
  std::vector<char> tokn = chunkMetadata.getToken();

  fsn::diagnostics::checkNullBytesInBuffer("Hash Buffer", hash);
  fsn::diagnostics::checkNullBytesInBuffer("Tokn Buffer", tokn);

  fsn::logger::consoleLog("Data End> " + std::to_string(chunkMetadata.getDataEnd()));
  
  // Extract Data
  size_t datSize = chunkMetadata.getDataEnd() - currentLoc + 1;

  fsn::logger::consoleLog("Preparing to read " + std::to_string(datSize) + " bytes.");

  std::vector<char> rawData(datSize);
  // rawData.reserve(chunkMetadata.getDataEnd() - currentLoc + 1);
  chunkFile.read(rawData.data(), datSize);

  
  // Compare hash of raw data with stored hash

  if (hash.empty() || hash.size() != crypto_hash_sha512_BYTES) {
    fsn::logger::consoleLog(
      "Error: Invaild hash size: " + std::to_string(hash.size()) + "bytes. Expected: "
      + std::to_string(crypto_hash_sha512_BYTES) + "bytes.", fsn::logger::ERROR);
    return;
  }

  fsn::diagnostics::checkNullBytesInBuffer("Data Buffer", rawData);

  std::vector<char> calculatedHash = fsn::util::primitive_calculateSHA512Hash(rawData);
  fsn::logger::consoleLog("Hash calculated.");
  bool hashCompSuccess = (sodium_memcmp(calculatedHash.data(), hash.data(), crypto_hash_sha512_BYTES) == 0);
  fsn::logger::consoleLog("Hash comparision completed.");

  // char hashHexReprRaw[128];
  // sodium_bin2hex(hashHexReprRaw, 128, reinterpret_cast<unsigned char*>(hash.data()), crypto_hash_sha512_BYTES);

  if (hashCompSuccess) {
    std::cout << "Hashes match.\n";
  } else {
    std::cout << "Hashes do not match.\n";
  }
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