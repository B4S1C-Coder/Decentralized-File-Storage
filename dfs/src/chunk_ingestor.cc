#include <grpcpp/grpcpp.h>
#include "chunkcomm.grpc.pb.h"
#include "chunkcomm.pb.h"
#include <memory>
#include <sodium.h>
#include <vector>
#include <fstream>
#include "chunk_ingestor.hh"
#include "util.hh"
#include "logger.hh"

::grpc::Status fsn::ChunkIngestionImpl::ingestChunk(::grpc::ServerContext* context, const ::CommData* req, ::CommResp* res) {
  
  // Extract data, grpc represents "bytes" (defined in proto) as "const std::string"
  std::vector<char> chunkData(req->chunkdata().begin(), req->chunkdata().end());
  std::vector<char> packetHash(req->packethash().begin(), req->packethash().end());

  if (packetHash.size() != crypto_hash_sha512_BYTES) {
    res->set_chunkaccepted(false);
    return grpc::Status(::grpc::StatusCode::INVALID_ARGUMENT, "Invalid packet hash size.");
  }

  // Verify hash of chunk data
  std::vector<char> calculatedHash = fsn::util::primitive_calculateSHA512Hash(chunkData);
  bool hashesMatch = (
    sodium_memcmp(calculatedHash.data(), packetHash.data(), crypto_hash_sha512_BYTES) == 0
  );

  if (!hashesMatch) {
    res->set_chunkaccepted(false);
    return grpc::Status::CANCELLED;
  }

  // Store the chunk on disk (TO-DO: Make file name unique)
  std::ofstream chunkFile("data/chunk_" + fsn::util::bytesToHex(packetHash) + ".bin", std::ios::app | std::ios::binary);
  chunkFile.write(chunkData.data(), chunkData.size());
  chunkFile.close();

  res->set_chunkaccepted(true);
  return grpc::Status::OK;
}