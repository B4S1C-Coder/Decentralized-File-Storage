#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <grpcpp/grpcpp.h>
#include <memory>
#include "dfs.hh"

struct nodeState {
  std::string ip;
  std::string domain;
  int port;
  size_t occupied;

  std::string getJsonString() {
    nlohmann::json jsonObj;
    jsonObj["ip"] = ip;
    jsonObj["domain"] = domain;
    jsonObj["port"] = port;
    jsonObj["occupied"] = occupied;

    return jsonObj.dump();
  }
};

size_t getDirectorySize(const std::filesystem::path& dirPath) {
  size_t size = 0;

  for (const auto& entry: std::filesystem::recursive_directory_iterator(dirPath)) {
    if (std::filesystem::is_regular_file(entry.status())) {
      size += std::filesystem::file_size(entry);
    }
  }

  return size;
}

void updateNodeStateOnRegistry(httplib::Client& client, const std::string& body) {
  httplib::Headers headers = {
    { "Content-Type", "application/json" }
  };

  auto res = client.Post("/register-node", headers, body, "application/json");

  if (res && res->status == 204) {
    fsn::logger::consoleLog("State updated on registry.", fsn::logger::INFO);
  } else {
    fsn::logger::consoleLog("Failed to updated state on registry.", fsn::logger::WARN);
  }
}

void setup(httplib::Client& client, nodeState& ns) {
  std::filesystem::path path = "data";
  try {
    if (std::filesystem::create_directory(path)) {
      fsn::logger::consoleLog("Data directory created.");
    } else {
      fsn::logger::consoleLog("Data directory already exists", fsn::logger::WARN);
    }
  } catch (const std::filesystem::filesystem_error& e) {
    fsn::logger::consoleLog(e.what(), fsn::logger::ERROR);
  }

  ns.occupied = getDirectorySize(path);

  // Register the server with the registry
  updateNodeStateOnRegistry(client, ns.getJsonString());
}

int main(int argc, char* argv[]) {

  if (argc < 4) {
    fsn::logger::consoleLog("In sufficient arguments", fsn::logger::ERROR);
    return 1;
  }

  httplib::Client cli("http://localhost:5000");
  nodeState ns = { argv[1], argv[2], std::atoi(argv[3]), 0 };

  setup(cli, ns);

  fsn::ChunkIngestionImpl chunkIngest;
  grpc::ServerBuilder builder;

  const std::string serverCompleteDomain = ns.domain + ":" + std::to_string(ns.port);

  fsn::logger::consoleLog("Server started and listening on " + serverCompleteDomain, fsn::logger::INFO);

  builder.AddListeningPort(serverCompleteDomain, grpc::InsecureServerCredentials());
  builder.RegisterService(&chunkIngest);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  server->Wait();
  
  return 0;
}
