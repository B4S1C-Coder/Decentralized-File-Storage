#include <grpcpp/grpcpp.h>
#include "chunk_ingestor.hh"
#include "logger.hh"
#include <memory>

int main() {
  fsn::ChunkIngestionImpl chunkIngest;
  grpc::ServerBuilder builder;

  fsn::logger::consoleLog("Server started on localhost:50051", fsn::logger::INFO);
  
  builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());
  builder.RegisterService(&chunkIngest);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  server->Wait();

  return 0;
}
