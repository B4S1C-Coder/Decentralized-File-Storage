#include <chrono>
#include <format>
#include <iostream>
#include "logger.hh"

std::string fsn::logger::logLevelStr(fsn::logger::LOG_LEVEL level) {
  switch (level) {
    case fsn::logger::INFO:
      return "INFO ";
    case fsn::logger::DEBUG:
      return "DEBUG";
    case fsn::logger::WARN:
      return "WARN ";
    case fsn::logger::ERROR:
      return "ERROR";
    case fsn::logger::FATAL:
      return "FATAL";
    default:
      return "UNKWN";
  }
}

void fsn::logger::consoleLog(std::string_view message, fsn::logger::LOG_LEVEL level) {
  std::string timestamp = fsn::logger::currentTimestamp();
  std::cout << timestamp << " - [ " << fsn::logger::logLevelStr(level) << " ] - " << message << "\n";
}