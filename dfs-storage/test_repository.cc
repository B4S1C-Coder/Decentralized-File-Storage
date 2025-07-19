#include "repository.hh"
#include "dfs-core.hh"
#include <iostream>

using namespace core::repository;

int main() {
    fsn::logger::setLogThreshold(fsn::logger::DEBUG);
    
    try {
        // Create database
        Database db("test_repository.db");
        
        // Create repository - this automatically creates the table!
        ConfigurationRepository configRepo(db);
        
        // Create configuration entity - much cleaner than raw SQL!
        Configuration config(
            "test_repository.db",
            2048,
            365,
            "super-secret-node-key",
            "ui-auth-key-123",
            "admin",
            "hashed-admin-password",
            "192.168.1.100,10.0.0.50"
        );
        
        // Save to database
        if (configRepo.create(config)) {
            fsn::logger::consoleLog("Configuration saved successfully using repository");
        } else {
            fsn::logger::consoleLog("Failed to save configuration");
            return 1;
        }
        
        // Find by root user name - custom repository method
        auto foundConfig = configRepo.findByRootUserName("admin");
        if (foundConfig.has_value()) {
            fsn::logger::consoleLog("Found config for admin user");
            fsn::logger::consoleLog("DB Path: " + foundConfig->getDbPath());
            fsn::logger::consoleLog("Max Size: " + std::to_string(foundConfig->getMaxOccupyDataDirSizeInMB()) + "MB");
            fsn::logger::consoleLog("Blocked IPs: " + foundConfig->getBlockedIPSCSV());
        }
        
        // Find all configurations with size > 1000MB
        auto largeConfigs = configRepo.findByMaxSizeGreaterThan(1000);
        fsn::logger::consoleLog("Found " + std::to_string(largeConfigs.size()) + " configs with size > 1000MB");
        
        // Update configuration
        if (foundConfig.has_value()) {
            foundConfig->setMaxOccupyDataDirSizeInMB(4096);
            foundConfig->setInactiveDeletionDurationInDays(730);
            
            if (configRepo.update(*foundConfig)) {
                fsn::logger::consoleLog("Configuration updated successfully");
            }
        }
        
        // Find all configurations
        auto allConfigs = configRepo.findAll();
        fsn::logger::consoleLog("Total configurations in database: " + std::to_string(allConfigs.size()));
        
        // Custom query using lambda
        auto customResults = configRepo.findBy([](Table& table) {
            QueryBuilder query(table);
            return query.select({"rootUserName", "maxOccupyDataDirSizeInMB"})
                       .where("inactiveDeletionDurationInDays", ">=", 180)
                       .orderBy("maxOccupyDataDirSizeInMB", false);
        });
        
        fsn::logger::consoleLog("Custom query returned " + std::to_string(customResults.size()) + " results");
        
        // Demonstrate the power of the repository pattern
        fsn::logger::consoleLog("=== Repository Pattern Benefits ===");
        fsn::logger::consoleLog("1. Clean separation of concerns");
        fsn::logger::consoleLog("2. Type-safe entity operations");
        fsn::logger::consoleLog("3. Automatic table creation");
        fsn::logger::consoleLog("4. Custom query methods");
        fsn::logger::consoleLog("5. Easy to test and mock");
        fsn::logger::consoleLog("6. No SQL strings in business logic");
        fsn::logger::consoleLog("7. Reusable across the application");
        
        // Compare with old DTO approach
        fsn::logger::consoleLog("");
        fsn::logger::consoleLog("=== Old DTO Approach ===");
        fsn::logger::consoleLog("// Had to write this for every table:");
        fsn::logger::consoleLog("class ConfigurationDTO : public DTO {");
        fsn::logger::consoleLog("  std::string getCreateTableStatement() const override {");
        fsn::logger::consoleLog("    return \"CREATE TABLE IF NOT EXISTS config (...)\";");
        fsn::logger::consoleLog("  }");
        fsn::logger::consoleLog("  std::string getInsertStatement() const override {");
        fsn::logger::consoleLog("    return \"INSERT INTO config (...) VALUES (...)\";");
        fsn::logger::consoleLog("  }");
        fsn::logger::consoleLog("  // ... more manual SQL");
        fsn::logger::consoleLog("};");
        
        fsn::logger::consoleLog("");
        fsn::logger::consoleLog("=== New Repository Approach ===");
        fsn::logger::consoleLog("// Just define the entity and table schema:");
        fsn::logger::consoleLog("class Configuration { /* entity with getters/setters */ };");
        fsn::logger::consoleLog("class ConfigurationRepository : public BaseRepository<Configuration> {");
        fsn::logger::consoleLog("  // Automatic CRUD operations!");
        fsn::logger::consoleLog("  // Custom query methods!");
        fsn::logger::consoleLog("  // Type safety!");
        fsn::logger::consoleLog("};");
        
    } catch (const std::exception& e) {
        fsn::logger::consoleLog("Error: " + std::string(e.what()), fsn::logger::ERROR);
        return 1;
    }
    
    return 0;
} 