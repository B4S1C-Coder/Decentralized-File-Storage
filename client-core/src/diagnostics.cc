#include <iostream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "diagnostics.hh"

void fsn::diagnostics::checkNullBytesInBuffer(const std::string& label, std::vector<char>& __buffer) {
  std::cout << "BUFFER DIAGNOSTICS\n"<< label <<"\n------------------------------\n";

  for (char __token: __buffer) {
    if (std::isprint(static_cast<unsigned char>(__token))) { std::cout << __token; }
    else {
      std::cout << "\\x" << std::hex << std::uppercase << std::setw(2)
        << std::setfill('0') << (static_cast<unsigned char>(__token)) << std::dec;
    }
  }

  std::cout << "\n------------------------------" << std::endl;
}