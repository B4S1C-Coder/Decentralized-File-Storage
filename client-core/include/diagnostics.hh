#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

namespace fsn::diagnostics {
  void checkNullBytesInBuffer(const std::string& label, std::vector<char>& __buffer);
}