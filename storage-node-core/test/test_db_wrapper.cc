#include "core/db.hh"
#include "dfs.hh"

int main() {
  fsn::logger::setLogThreshold(fsn::logger::DEBUG);

  core::SQLite3DB db("test.db");
  bool result = db.createTable("TestTable", {{"id", "INTEGER"}, {"name", "TEXT"}});

  if (result) {
    fsn::logger::consoleLog("Table created successfully");
  } else {
    fsn::logger::consoleLog("Table creation failed");
  }

  return 0;
}