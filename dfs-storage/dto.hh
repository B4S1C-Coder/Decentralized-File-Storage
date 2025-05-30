#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <any>
#include <sqlite3.h>
#include "db.hh"

// Various DTOs for internal use, to be used by SQLite3 Database Wrapper

namespace core::dto {

class DTO {
public:
  virtual std::string getCreateTableStatement() const = 0;
  virtual std::string getInsertStatement() const = 0;
  virtual std::string getFetchOneStatement() const = 0;
  
  // template<typename T>
  // requires core::DerivedFromDTO<T>
  // virtual std::unique_ptr<T> buildDTOFromStmt(sqlite3_stmt* stmt) = 0;
};

class ConfigurationDTO : public DTO {
public:
  ConfigurationDTO(
    const std::string& dbPath,
    size_t maxOccupyDataDirSizeInMB,
    int inactiveDeletionDurationInDays,
    const std::string& nodeSecretKey,
    const std::string& userInterfaceAuthKey,
    const std::string& rootUserName,
    const std::string& rootUserPasswordHash,
    const std::string& blockedIPSCSV
  ): 
    dbpath(dbPath),
    maxOccupyDataDirSizeInMB(maxOccupyDataDirSizeInMB),
    inactiveDeletionDurationInDays(inactiveDeletionDurationInDays),
    nodeSecretKey(nodeSecretKey),
    userInterfaceAuthKey(userInterfaceAuthKey),
    rootUserName(rootUserName),
    rootUserPasswordHash(rootUserPasswordHash),
    blockedIPSCSV(blockedIPSCSV) {}

  inline std::string getCreateTableStatement() const override {
    return "CREATE TABLE IF NOT EXISTS config ("
              "dbPath TEXT,"
              "maxOccupyDataDirSizeInMB INTEGER,"
              "inactiveDeletionDurationInDays INTEGER,"
              "nodeSecretKey TEXT,"
              "userInterfaceAuthKey TEXT,"
              "rootUserName TEXT,"
              "rootUserPasswordHash TEXT,"
              "blockedIPSCSV TEXT);";
  }

  inline std::string getInsertStatement() const override {
    return "INSERT INTO config(dbPath, maxOccupyDataDirSizeInMB, inactiveDeletionDurationInDays," 
          "nodeSecretKey, userInterfaceAuthKey, rootUserName, rootUserPasswordHash, blockedIPSCSV)"
          " VALUES (\"" + dbpath + "\", " + std::to_string(maxOccupyDataDirSizeInMB) + ", " +
          std::to_string(inactiveDeletionDurationInDays) + ", \"" + nodeSecretKey + "\", \"" +
          userInterfaceAuthKey + "\", \"" + rootUserName + "\", \"" + rootUserPasswordHash + "\", \"" +
          blockedIPSCSV + "\");";
  }

  inline std::string getFetchOneStatement() const override {
    return "SELECT dbPath, maxOccupyDataDirSizeInMB, inactiveDeletionDurationInDays, nodeSecretKey,"
    " userInterfaceAuthKey, rootUserName, rootUserPasswordHash, blockedIPSCSV FROM config LIMIT 1;";
  }

  // std::unique_ptr<DTO> buildDTOFromStmt(sqlite3_stmt* stmt) override;

private:
  std::string dbpath;
  size_t maxOccupyDataDirSizeInMB;
  int inactiveDeletionDurationInDays;
  std::string nodeSecretKey;
  std::string userInterfaceAuthKey;
  std::string rootUserName;
  std::string rootUserPasswordHash;
  std::string blockedIPSCSV;
};

}