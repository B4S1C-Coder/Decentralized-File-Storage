#include <grpcpp/grpcpp.h>
#include "chunkcomm.grpc.pb.h"
#include "chunkcomm.pb.h"

#include <memory>
#include <vector>
#include <string>

#include "chunk_transmitter.hh"
#include "util.hh"
#include "logger.hh"

void fsn::ChunkTransmitter::ingestChunk(std::unique_ptr<std::vector<char>> chunk) {
  CommData commdata;
  commdata.set_chunkdata(chunk->data());
  commdata.set_packethash(
    fsn::util::primitive_calculateSHA512Hash(*chunk).data()
  );

  CommResp response;
  grpc::ClientContext context;

  // Making the gRPC call
  grpc::Status status = stub_->ingestChunk(&context, commdata, &response);

  if (status.ok()) {
    std::string message = (response.chunkaccepted()) ? "Chunk Accepted" : "Chunk Rejected";
    fsn::logger::consoleLog(message, fsn::logger::INFO);
    return;
  } else {
    std::string message = "Failed to make gRPC call: " + status.error_message();
    fsn::logger::consoleLog(message, fsn::logger::FATAL);
    return;
  }
}

std::optional<std::unique_ptr<std::vector<char>>> fsn::ChunkTransmitter::ejectChunk(const std::vector<char>& packetHash) {
  CommChunkReq req;
  req.set_packethash(packetHash.data(), packetHash.size());

  CommChunkRes res;
  grpc::ClientContext context;

  grpc::Status status = stub_->ejectChunk(&context, req, &res);

  if (status.ok()) {
    fsn::logger::consoleLog("Chunk Received.", fsn::logger::INFO);
    return std::make_unique<std::vector<char>>(res.chunkdata().begin(), res.chunkdata().end());
  } else {
    fsn::logger::consoleLog("gRPC call failed - " + status.error_message(), fsn::logger::ERROR);
    return std::nullopt;
  }
}