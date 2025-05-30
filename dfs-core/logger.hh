#pragma once

#include <chrono>
#include <format>
#include <iostream>

namespace fsn::logger {
  enum LOG_LEVEL {
    DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3, FATAL = 4
  };

  inline std::string currentTimestamp() {
    const auto now = std::chrono::system_clock::now();
    return std::format("{:%FT%TZ}", now);
  }

  std::string logLevelStr(LOG_LEVEL level);

  void consoleLog(std::string_view message, LOG_LEVEL level=DEBUG);

  void setLogThreshold(LOG_LEVEL level);

}