#pragma once

#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <stdexcept>
#include <sqlite3.h>
#include "dfs.hh"

// SQLite3 Database Wrapper, for easy database operations
namespace core {
class SQLite3DB {
public:
  SQLite3DB(const std::string& path);
  bool createTable(const std::string& tname, const std::vector<std::pair<std::string, std::string>>& fields);
private:
  std::string m_dbFilePath;
  std::unique_ptr<sqlite3, decltype(&sqlite3_close)> m_db;
};
}