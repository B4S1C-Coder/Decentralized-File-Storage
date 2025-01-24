#include <iostream>
#include <endian.h>
#include <cstdint>

int main() {
  
  size_t testDataEnd = 6667;
  uint64_t le = htole64(testDataEnd);
  size_t backToHost = le64toh(le);

  std::cout << "Orignal: " << testDataEnd << "\n";
  std::cout << "LE     : " << le << "\n";
  std::cout << "Host   : " << backToHost << "\n";

  return 0;
}