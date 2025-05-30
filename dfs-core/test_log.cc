#include <string>
#include "logger.hh"

int main() {

  fsn::logger::consoleLog("Test message. Logs work!");
  
  fsn::logger::setLogThreshold(fsn::logger::INFO);
  // Should not be printed since, threshold is now INFO
  for (int i = 0; i < 10; i++) {
    // Defualt lebel is DEBUG
    fsn::logger::consoleLog("i = " + std::to_string(i));
  }

  for (int k = 0; k < 10; k++) {
    fsn::logger::consoleLog("k = " + std::to_string(k), fsn::logger::INFO);
  }

  fsn::logger::consoleLog("Dangerous Message", fsn::logger::WARN);
  fsn::logger::consoleLog("Very Dangerous Message", fsn::logger::ERROR);
  fsn::logger::consoleLog("Danger Pro Ultra Max Message", fsn::logger::FATAL);

  return 0;
}