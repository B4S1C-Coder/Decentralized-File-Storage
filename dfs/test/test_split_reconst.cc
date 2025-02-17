#include <sodium.h>
#include "sequential_file_splitter.hh"
#include "stream_encrypt_decrypt.hh"
#include "chunk_metadata.hh"
#include "logger.hh"
#include "util.hh"

void manualReconstruction(fsn::StreamEncryptorDecryptor sed) {
  if (sodium_init() == -1) {
    fsn::logger::consoleLog("Sodium initialization failed.", fsn::logger::FATAL);
    return;
  }

  std::ofstream reconstructedFile("reconstructed.txt", std::ios::app | std::ios::binary);
  if (!reconstructedFile) {
    fsn::logger::consoleLog("Failed to open output file. Aborting ...", fsn::logger::ERROR);
    return;
  }

  for (int i = 1; i < 7; i++) {
    std::string fileName = std::to_string(i) + ".fsnc";
    fsn::logger::consoleLog("Processing " + fileName, fsn::logger::INFO);

    // Load the chunk and construct the metadata
    std::ifstream chunkFile(fileName, std::ios::binary);
    if (!chunkFile) {
      fsn::logger::consoleLog("Error opening " + fileName, fsn::logger::ERROR);
      return;
    }

    fsn::ChunkMetadata chunkMetadata = fsn::ChunkMetadata::loadFromFile(
      chunkFile, crypto_hash_sha512_BYTES, 32
    );

    fsn::logger::consoleLog("Metadata loaded.", fsn::logger::INFO);

    if (chunkMetadata.getDataEnd() < chunkFile.tellg()) {
      fsn::logger::consoleLog("Possibly corrupted chunk, skipping ...", fsn::logger::WARN);
      chunkFile.close();
      continue;
    }

    // Load the encrypted data
    size_t payloadSize = chunkMetadata.getDataEnd() - chunkFile.tellg() + 1;
    std::vector<char> payloadBuffer(payloadSize);
    chunkFile.read(payloadBuffer.data(), payloadSize);
    chunkFile.close();

    // Decrypt the data
    std::vector<char> initialDataHash = chunkMetadata.getHash();
    auto decryptOpt = sed.xchacha20poly1305_decrypt(payloadBuffer, initialDataHash);
    
    if (decryptOpt == std::nullopt) {
      fsn::logger::consoleLog("Decryption failed. Aborting ...", fsn::logger::ERROR);
      reconstructedFile.close();
      return;
    }

    reconstructedFile.write(decryptOpt.value().data(), decryptOpt.value().size());
    fsn::logger::consoleLog("Decrypted data written to disk.", fsn::logger::INFO);
  }
  reconstructedFile.close();
}

int main() {

  fsn::StreamEncryptorDecryptor sed; // This will be used for encryption and decryption
  fsn::SequentialFileSplitter sfs("../res/data.txt", 5);
  sfs.setStreamEncryptorDecryptor(sed);

  sfs.singleThreadedSplit(".");

  manualReconstruction(sed);

  return 0;
}