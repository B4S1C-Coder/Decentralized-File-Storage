#include <cstdlib>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "dfs.hh"

void transmitFile(
  fsn::SequentialFileSplitter& fileSplitter,
  fsn::StreamEncryptorDecryptor& encryptionProvider,
  std::vector<fsn::ChunkTransmitter>& fileTransmitters
) {
  // Perform the split
  if (fileSplitter.singleThreadedSplit("./chunks") == 0) {
    fsn::logger::consoleLog("File split successful", fsn::logger::INFO);
  } else {
    fsn::logger::consoleLog("Split failed.", fsn::logger::ERROR);
    return;
  }

  // Save key and nonce for future reconstruction and decryption
  encryptionProvider.dumpKeyAndNonce();

  // Extract the chunks file paths
  std::vector<std::string> chunkFilePaths = fileSplitter.getChunkFilePaths();
  std::string manifestPayload = "";

  for (const auto& chunkFilePath: chunkFilePaths) {
    manifestPayload += (chunkFilePath + "\n");
  }

  std::ofstream manifestFile("manifest.txt", std::ios::binary);
  if (manifestFile) {
    manifestFile.write(manifestPayload.data(), manifestPayload.size());
    manifestFile.close();
  } else {
    fsn::logger::consoleLog("Failed to write manifest file.", fsn::logger::WARN);
    std::cout << manifestPayload << "\n";
  }

  int totalNodes = fileTransmitters.size();

  // TO-DO: Transmit the chunks
  for (int i = 0; i < chunkFilePaths.size(); i++) {

    auto file = fsn::util::loadFileIntoBuffer(chunkFilePaths[i]);
    
    if (file == std::nullopt) {
      fsn::logger::consoleLog("Unable to read chunk file.", fsn::logger::ERROR);
      return;
    }
    std::unique_ptr<std::vector<char>> data = std::move(file.value());
    fileTransmitters[i % totalNodes].ingestChunk(std::move(data));
    fsn::logger::consoleLog("Transmitted - " + chunkFilePaths[i], fsn::logger::INFO);
  }

}

int main(int argc, char* argv[]) {

  // Prepare for file split
  fsn::StreamEncryptorDecryptor encryptionProvider;
  fsn::SequentialFileSplitter fileSplitter(argv[2], std::atoi(argv[3]));

  fileSplitter.setStreamEncryptorDecryptor(encryptionProvider);

  // Fetch the available nodes (& establish gRPC channels with them)
  httplib::Client registry("http://localhost:5000");

  auto res = registry.Get("/get-nodes");
  std::vector<fsn::ChunkTransmitter> fileTransmitters;

  if (res) {
    nlohmann::json resJson = nlohmann::json::parse(res->body);

    if (!resJson.is_array()) {
      fsn::logger::consoleLog("Expected array, but didn't get array from registry", fsn::logger::ERROR);
      return 1;
    }

    if (resJson.empty()) {
      fsn::logger::consoleLog("No nodes are available at the moment", fsn::logger::INFO);
      return 2;
    }

    for (const auto& item: resJson) {
      std::string ip = item["ip"].get<std::string>();
      std::string port = std::to_string(item["port"].get<int>());
      std::string address = ip + ":" + port;
      
      fileTransmitters.push_back(
        fsn::ChunkTransmitter(
          grpc::CreateChannel(address, grpc::InsecureChannelCredentials())
        )
      );
    }
  } else {
    fsn::logger::consoleLog("Failed to communicate with registry.", fsn::logger::ERROR);
    return 3;
  }

  if (std::string(argv[1]) == "--transmit") {
    transmitFile(fileSplitter, encryptionProvider, fileTransmitters);
  }

  return 0;
}