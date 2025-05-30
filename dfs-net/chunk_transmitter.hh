#pragma once

#include <grpcpp/grpcpp.h>
#include "chunkcomm.grpc.pb.h"
#include "chunkcomm.pb.h"

#include <memory>
#include <vector>
#include <optional>

namespace fsn {

class ChunkTransmitter {
public:
  ChunkTransmitter(std::shared_ptr<grpc::Channel> channel) : stub_(ChunkIngestion::NewStub(channel)) {}
  void ingestChunk(std::unique_ptr<std::vector<char>> chunk);
  std::optional<std::unique_ptr<std::vector<char>>> ejectChunk(const std::vector<char>& packetHash);

private:
  std::unique_ptr<ChunkIngestion::Stub> stub_;
};

}