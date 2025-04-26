#include "core/db.hh"
#include "core/dto.hh"
#include "dfs.hh"

int main() {
  fsn::logger::setLogThreshold(fsn::logger::DEBUG);

  core::SQLite3DB db("config_test.db");
  core::dto::ConfigurationDTO conf(
    "config_test.db", 1024, 180, "node-secret", "ui-key", "root", "passwd", ""
  );

  if (db.createTable(conf)) {
    fsn::logger::consoleLog("Table created successfully.");
  } else {
    fsn::logger::consoleLog("Table creation failed.");
  }

  if (db.insertData(conf)) {
    fsn::logger::consoleLog("Data inserted.");
  } else {
    fsn::logger::consoleLog("Data insertion failed.");
  }

  return 0;
}