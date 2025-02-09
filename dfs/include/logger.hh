#pragma once

#include <chrono>
#include <format>
#include <iostream>

namespace fsn::logger {
  enum LOG_LEVEL {
    INFO, WARN, ERROR, FATAL, DEBUG
  };

  inline std::string currentTimestamp() {
    const auto now = std::chrono::system_clock::now();
    return std::format("{:%FT%TZ}", now);
  }

  std::string logLevelStr(LOG_LEVEL level);

  void consoleLog(std::string_view message, LOG_LEVEL level=DEBUG);

}