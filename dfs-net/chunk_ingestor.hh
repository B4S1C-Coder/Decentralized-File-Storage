#pragma once

#include <grpcpp/grpcpp.h>
#include "chunkcomm.grpc.pb.h"
#include "chunkcomm.pb.h"
#include <memory>

namespace fsn {

class ChunkIngestionImpl : public ChunkIngestion::Service {
  virtual ::grpc::Status ingestChunk(::grpc::ServerContext* context, const ::CommData* req, ::CommResp* res);
  virtual ::grpc::Status ejectChunk(::grpc::ServerContext* context, const ::CommChunkReq* req, ::CommChunkRes* res);
};

}