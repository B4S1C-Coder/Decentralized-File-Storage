#pragma once

#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <type_traits>
#include <stdexcept>
#include <sqlite3.h>
#include "dfs-core.hh"
#include "dto.hh"

// SQLite3 Database Wrapper, for easy database operations
namespace core {

// template<typename T>
// concept DerivedFromDTO = std::is_base_of<core::dto::DTO, T>::value;

class SQLite3DB {
public:
  SQLite3DB(const std::string& path);
  bool createTable(const std::string& tname, const std::vector<std::pair<std::string, std::string>>& fields);
  bool createTable(const core::dto::DTO& dto);
  bool insertData(const core::dto::DTO& dto);

  // template<DerivedFromDTO T>
  // std::unique_ptr<T> fetchObject(const T& dto);

private:
  std::string m_dbFilePath;
  std::unique_ptr<sqlite3, decltype(&sqlite3_close)> m_db;
};
}