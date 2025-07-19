#include "orm.hh"
#include "dfs-core.hh"
#include <iostream>

using namespace core::orm;

int main() {
    fsn::logger::setLogThreshold(fsn::logger::DEBUG);
    
    try {
        // Create database
        Database db("test_orm.db");
        
        // Define table schema - much easier than writing SQL manually!
        Table configTable("config");
        configTable
            .addField(primaryKeyField("id"))
            .addField(textField("dbPath").notNull())
            .addField(integerField("maxOccupyDataDirSizeInMB").notNull())
            .addField(integerField("inactiveDeletionDurationInDays").notNull())
            .addField(textField("nodeSecretKey").notNull())
            .addField(textField("userInterfaceAuthKey").notNull())
            .addField(textField("rootUserName").notNull())
            .addField(textField("rootUserPasswordHash").notNull())
            .addField(textField("blockedIPSCSV"));
        
        // Create table automatically
        if (db.createTable(configTable)) {
            fsn::logger::consoleLog("Table created successfully using ORM");
        } else {
            fsn::logger::consoleLog("Failed to create table");
            return 1;
        }
        
        // Insert data - no manual SQL writing needed!
        Record configRecord(configTable);
        configRecord.setValue("dbPath", std::string("test_orm.db"));
        configRecord.setValue("maxOccupyDataDirSizeInMB", 1024);
        configRecord.setValue("inactiveDeletionDurationInDays", 180);
        configRecord.setValue("nodeSecretKey", std::string("node-secret-key"));
        configRecord.setValue("userInterfaceAuthKey", std::string("ui-auth-key"));
        configRecord.setValue("rootUserName", std::string("admin"));
        configRecord.setValue("rootUserPasswordHash", std::string("hashed-password"));
        configRecord.setValue("blockedIPSCSV", std::string(""));
        
        if (db.insert(configRecord)) {
            fsn::logger::consoleLog("Data inserted successfully using ORM");
        } else {
            fsn::logger::consoleLog("Failed to insert data");
            return 1;
        }
        
        // Query data using QueryBuilder - much more flexible than fixed DTO queries!
        QueryBuilder query(configTable);
        query.select().where("rootUserName", "=", std::string("admin"));
        
        auto results = db.select(query);
        if (!results.empty()) {
            fsn::logger::consoleLog("Found " + std::to_string(results.size()) + " records");
            
            for (const auto& record : results) {
                auto dbPath = record.getValue("dbPath");
                auto maxSize = record.getValue("maxOccupyDataDirSizeInMB");
                
                if (dbPath.has_value() && maxSize.has_value()) {
                    std::string path = std::get<std::string>(dbPath.value());
                    int size = std::get<int>(maxSize.value());
                    fsn::logger::consoleLog("DB Path: " + path + ", Max Size: " + std::to_string(size) + "MB");
                }
            }
        }
        
        // More complex query example
        QueryBuilder complexQuery(configTable);
        complexQuery.select({"dbPath", "maxOccupyDataDirSizeInMB", "rootUserName"})
                   .where("maxOccupyDataDirSizeInMB", ">", 500)
                   .andWhere("inactiveDeletionDurationInDays", ">=", 30)
                   .orderBy("maxOccupyDataDirSizeInMB", false) // descending
                   .limit(10);
        
        auto complexResults = db.select(complexQuery);
        fsn::logger::consoleLog("Complex query returned " + std::to_string(complexResults.size()) + " records");
        
        // Update example
        Record updateRecord(configTable);
        updateRecord.setValue("maxOccupyDataDirSizeInMB", 2048);
        updateRecord.setValue("inactiveDeletionDurationInDays", 365);
        
        if (db.update(updateRecord, "rootUserName", std::string("admin"))) {
            fsn::logger::consoleLog("Record updated successfully");
        }
        
        // Demonstrate how much easier this is compared to the old DTO approach
        fsn::logger::consoleLog("=== ORM vs DTO Comparison ===");
        fsn::logger::consoleLog("Old DTO approach required:");
        fsn::logger::consoleLog("1. Manual SQL writing in each DTO class");
        fsn::logger::consoleLog("2. Hardcoded queries that can't be modified");
        fsn::logger::consoleLog("3. Repetitive code for each table");
        fsn::logger::consoleLog("4. No type safety for field values");
        fsn::logger::consoleLog("");
        fsn::logger::consoleLog("New ORM approach provides:");
        fsn::logger::consoleLog("1. Automatic SQL generation");
        fsn::logger::consoleLog("2. Flexible query building");
        fsn::logger::consoleLog("3. Type-safe field operations");
        fsn::logger::consoleLog("4. Reusable table definitions");
        fsn::logger::consoleLog("5. Easy to extend and modify");
        
    } catch (const std::exception& e) {
        fsn::logger::consoleLog("Error: " + std::string(e.what()), fsn::logger::ERROR);
        return 1;
    }
    
    return 0;
} 