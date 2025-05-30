#include <chrono>
#include <format>
#include <iostream>
#include "logger.hh"

fsn::logger::LOG_LEVEL logThreshold = fsn::logger::DEBUG;

void fsn::logger::setLogThreshold(fsn::logger::LOG_LEVEL level) {
  logThreshold = level;
}

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
  if (level < logThreshold) { return; }
  try {
    std::cout << fsn::logger::currentTimestamp() <<
    " - [ " << fsn::logger::logLevelStr(level) << " ] - " << message << "\n";
  } catch(...) {/* Fail Silently, to avoid crashing downstream. */}
}