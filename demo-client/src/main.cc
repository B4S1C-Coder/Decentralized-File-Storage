#include <cstdlib>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "dfs.hh"

void transmitFile(
  fsn::SequentialFileSplitter& fileSplitter,
  fsn::StreamEncryptorDecryptor& encryptionProvider,
  httplib::Client& registry
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

  // Fetch the available nodes
  auto res = registry.Get("/get-nodes");
  std::vector<std::string> nodesUrls;

  if (res) {
    nlohmann::json resJson = nlohmann::json::parse(res->body);

    if (!resJson.is_array()) {
      fsn::logger::consoleLog("Expected array, but didn't get array from registry", fsn::logger::ERROR);
      return;
    }

    if (resJson.empty()) {
      fsn::logger::consoleLog("No nodes are available at the moment", fsn::logger::INFO);
      return;
    }

    for (const auto& item: resJson) {
      nodesUrls.push_back(item["ip"] + ":" + item["port"]);
    }
  } else {
    fsn::logger::consoleLog("Failed to communicate with registry.", fsn::logger::ERROR);
    return;
  }

  // TO-DO: Transmit the chunks


}

int main(int argc, char* argv[]) {

  // Prepare for file split
  fsn::StreamEncryptorDecryptor encryptionProvider;
  fsn::SequentialFileSplitter fileSplitter(argv[2], std::atoi(argv[3]));

  fileSplitter.setStreamEncryptorDecryptor(encryptionProvider);


  return 0;
}