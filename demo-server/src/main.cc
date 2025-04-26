#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include "dfs.hh"

int main() {
  httplib::Client cli("http://localhost:5000");

  std::string body = R"({"name":"ChatGPT", "type":"AI"})";
  httplib::Headers headers = {
    { "Content-Type", "application/json" }
  };

  auto res = cli.Post("/test", headers, body, "application/json");

  if (res) {
    fsn::logger::consoleLog("Status code: " + std::to_string(res->status));
    fsn::logger::consoleLog("Response body: " + res->body);

    // {"message":"POST received","data":{"name":"frfr","type":"frf"}}
    nlohmann::json jsonRes = nlohmann::json::parse(res->body);
    auto data = jsonRes["data"];

    fsn::logger::consoleLog("Parsed name: " + data["name"].get<std::string>());
    fsn::logger::consoleLog("Parsed type: " + data["type"].get<std::string>());

  } else {
    std::string errStr = std::to_string(static_cast<int>(res.error()));
    fsn::logger::consoleLog("POST request failed: " + errStr, fsn::logger::ERROR);
  }

  return 0;
}
