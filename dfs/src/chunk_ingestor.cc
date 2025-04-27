#include <grpcpp/grpcpp.h>
#include "chunkcomm.grpc.pb.h"
#include "chunkcomm.pb.h"
#include <memory>
#include <sodium.h>
#include <vector>
#include <fstream>
#include <string>
#include "chunk_ingestor.hh"
#include "util.hh"
#include "logger.hh"

::grpc::Status fsn::ChunkIngestionImpl::ingestChunk(::grpc::ServerContext* context, const ::CommData* req, ::CommResp* res) {
  
  // Extract data, grpc represents "bytes" (defined in proto) as "const std::string"
  std::vector<char> chunkData(req->chunkdata().begin(), req->chunkdata().end());
  std::vector<char> packetHash(req->packethash().begin(), req->packethash().end());

  if (packetHash.size() != crypto_hash_sha512_BYTES) {
    fsn::logger::consoleLog("packetHash size = " + std::to_string(packetHash.size()));
    fsn::logger::consoleLog("Chunk rejected as invalid hash provided.", fsn::logger::WARN);
    res->set_chunkaccepted(false);
    return grpc::Status(::grpc::StatusCode::INVALID_ARGUMENT, "Invalid packet hash size.");
  }

  // Verify hash of chunk data
  std::vector<char> calculatedHash = fsn::util::primitive_calculateSHA512Hash(chunkData);
  bool hashesMatch = (
    sodium_memcmp(calculatedHash.data(), packetHash.data(), crypto_hash_sha512_BYTES) == 0
  );

  if (!hashesMatch) {
    fsn::logger::consoleLog("Chunk rejected as hashes do not match.", fsn::logger::WARN);
    res->set_chunkaccepted(false);
    return grpc::Status::CANCELLED;
  }

  // Store the chunk on disk (TO-DO: Make file name unique)
  std::ofstream chunkFile("data/chunk_" + fsn::util::bytesToHex(packetHash) + ".bin", std::ios::app | std::ios::binary);
  chunkFile.write(chunkData.data(), chunkData.size());
  chunkFile.close();

  fsn::logger::consoleLog("Chunk Accepted.", fsn::logger::INFO);

  res->set_chunkaccepted(true);
  return grpc::Status::OK;
}

::grpc::Status fsn::ChunkIngestionImpl::ejectChunk(::grpc::ServerContext* context, const ::CommChunkReq* req, ::CommChunkRes* res) {
  std::vector<char> packetHash(req->packethash().begin(), req->packethash().end());

  if (packetHash.size() != crypto_hash_sha512_BYTES) {
    fsn::logger::consoleLog("packetHash size = " + std::to_string(packetHash.size()));
    fsn::logger::consoleLog("Invalid hash provided. Request rejected.", fsn::logger::WARN);
    res->set_chunkdata("");
    return grpc::Status(::grpc::StatusCode::INVALID_ARGUMENT, "Invalid packet hash size");
  }

  std::string fileName = "data/chunk_" + fsn::util::bytesToHex(packetHash) + ".bin";

  std::ifstream chunkFile(fileName, std::ios::binary);
  if (!chunkFile) {
    fsn::logger::consoleLog("Chunk not found.", fsn::logger::ERROR);
    res->set_chunkdata("");
    return grpc::Status(::grpc::StatusCode::NOT_FOUND, "Chunk not found.");
  }

  chunkFile.seekg(0, std::ios::end);
  std::streamsize chunkSize = chunkFile.tellg();
  chunkFile.seekg(0, std::ios::beg);

  std::vector<char> chunkData(chunkSize);
  if (!chunkFile.read(chunkData.data(), chunkSize)) {
    fsn::logger::consoleLog("Unable to read chunk", fsn::logger::ERROR);
    res->set_chunkdata("");
    return grpc::Status(::grpc::StatusCode::INTERNAL, "Unable to read chunk.");
  }

  res->set_chunkdata(chunkData.data(), chunkData.size());
  return grpc::Status::OK;
}