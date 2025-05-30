#include "db.hh"

core::SQLite3DB::SQLite3DB(const std::string& path): m_dbFilePath(path), m_db(nullptr, sqlite3_close) {
  sqlite3* db = nullptr;

  if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
    const std::string errorMsg = sqlite3_errmsg(db);
    const std::string msg = "Unable to open SQLite3 DB: " + path + ", " + errorMsg;
    fsn::logger::consoleLog(msg, fsn::logger::FATAL);
    throw std::runtime_error(msg);
  }

  m_db.reset(db);
}

bool core::SQLite3DB::createTable(const std::string& tname, const std::vector<std::pair<std::string, std::string>>& fields) {
  std::string sqlStatement = "CREATE TABLE IF NOT EXISTS " + tname + " (" + fields[0].first + " " + fields[0].second + " PRIMARY KEY";
  char* errMsg = nullptr;

  // first element (i.e. element at index 0) in fields will be treated as primary key
  for (int i = 1; i < fields.size(); i++) {
    sqlStatement += (", " + fields[i].first + " " + fields[i].second);
  }

  sqlStatement += ");";

  fsn::logger::consoleLog(sqlStatement);

  int rc = sqlite3_exec(m_db.get(), sqlStatement.c_str(), nullptr, nullptr, &errMsg);

  if (rc != SQLITE_OK) {
    fsn::logger::consoleLog(errMsg, fsn::logger::ERROR);
    sqlite3_free(errMsg);
    return false;
  }

  fsn::logger::consoleLog("Created table - " + tname);

  return true;
}

bool core::SQLite3DB::createTable(const core::dto::DTO& dto) {
  char* errMsg;
  const std::string sqlStatement = dto.getCreateTableStatement();

  fsn::logger::consoleLog(sqlStatement);
  int rc = sqlite3_exec(m_db.get(), sqlStatement.c_str(), nullptr, nullptr, &errMsg);

  if (rc != SQLITE_OK) {
    fsn::logger::consoleLog(errMsg, fsn::logger::ERROR);
    sqlite3_free(errMsg);
    return false;
  }

  fsn::logger::consoleLog("Created table via DTO");
  return true;
}

bool core::SQLite3DB::insertData(const core::dto::DTO& dto) {
  char* errMsg;
  const std::string sqlStatement = dto.getInsertStatement();

  fsn::logger::consoleLog(sqlStatement);
  int rc = sqlite3_exec(m_db.get(), sqlStatement.c_str(), nullptr, nullptr, &errMsg);

  if (rc != SQLITE_OK) {
    fsn::logger::consoleLog(errMsg, fsn::logger::ERROR);
    sqlite3_free(errMsg);
    return false;
  }

  fsn::logger::consoleLog("Data inserted via DTO");
  return true;
}

// template<typename T>
// requires core::DerivedFromDTO<T>
// std::unique_ptr<T> core::SQLite3DB::fetchObject(const T& dto) {
  
//   const std::string sqlStatement = dto.getFetchOneStatement();
//   sqlite3_stmt* stmt;

//   int rc = sqlite3_prepare_v2(m_db.get(), sqlStatement.c_str(), -1, &stmt, 0);
//   if (rc != SQLITE_OK) {
//     sqlite3_finalize(stmt);
//     fsn::logger::consoleLog("SQL Query preparation failed.", fsn::logger::ERRROR);
//     throw std::runtime_error("SQL Query preparation failed.");
//   }

//   rc = sqlite3_step(stmt);
//   if (rc != SQLITE_ROW) {
//     sqlite3_finalize(stmt);
//     fsn::logger::consoleLog("No Row Returned.", fsn::logger::ERROR);
//     throw std::runtime_error("No row returned.");
//   }



//   std::unique_ptr<T> obp = dto.buildDTOFromStmt(stmt);
//   sqlite3_finalize(stmt);

//   return obp;
// }