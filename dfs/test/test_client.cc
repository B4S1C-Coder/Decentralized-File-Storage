#include <grpcpp/grpcpp.h>
#include "chunk_transmitter.hh"
#include "util.hh"
#include "logger.hh"
#include <memory>
#include <vector>

int main() {
  auto fileLoadOpt = fsn::util::loadFileIntoBuffer("../res/data.txt");
  
  if (fileLoadOpt == std::nullopt) {
    fsn::logger::consoleLog("Failed to open data file.", fsn::logger::ERROR);
    return 1;    
  }

  std::unique_ptr<std::vector<char>> data = std::move(fileLoadOpt.value());
  
  fsn::logger::consoleLog("Preparing to transmit.");

  fsn::ChunkTransmitter client(
    grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())
  );

  client.ingestChunk(std::move(data));

  fsn::logger::consoleLog("Transmitted Successfully.");

  return 0;
}
