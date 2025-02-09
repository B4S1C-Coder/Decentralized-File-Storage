#pragma once

#include <optional>
#include <memory>
#include <vector>
#include <fstream>
#include <string>
#include <sodium.h>

namespace fsn::util {

  std::optional<std::unique_ptr<std::vector<char>>> loadFileIntoBuffer(const std::string& path);
  std::unique_ptr<std::vector<char>> calculateSHA512Hash(std::vector<char>& inputBuffer);
  std::vector<char> primitive_calculateSHA512Hash(std::vector<char>& inputBuffer);
  std::unique_ptr<std::vector<char>> generateRandomToken();
  std::vector<char> primitive_generateRandomToken();

}