#include <sqlite3.h>
#include <iostream>

int main() {
  sqlite3* db;
  int rc = sqlite3_open(":memory:", &db);

  if (rc) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    return rc;
  } else {
    std::cout << "Opened in-memory database successfully\n";
  }

  sqlite3_close(db);
  return 0;
}