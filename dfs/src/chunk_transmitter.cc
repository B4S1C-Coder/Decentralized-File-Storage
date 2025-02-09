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