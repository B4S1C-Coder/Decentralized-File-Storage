#pragma once

#include "orm.hh"
#include <memory>
#include <functional>

namespace core::repository {

// Generic repository interface
template<typename T>
class Repository {
public:
    virtual ~Repository() = default;
    
    // CRUD operations
    virtual bool create(const T& entity) = 0;
    virtual std::optional<T> findById(const std::string& id) = 0;
    virtual std::vector<T> findAll() = 0;
    virtual bool update(const T& entity) = 0;
    virtual bool remove(const std::string& id) = 0;
    
    // Custom queries
    virtual std::vector<T> findBy(const std::function<QueryBuilder(Table&)>& queryBuilder) = 0;
    virtual std::optional<T> findOneBy(const std::function<QueryBuilder(Table&)>& queryBuilder) = 0;
};

// Base repository implementation
template<typename T>
class BaseRepository : public Repository<T> {
public:
    BaseRepository(Database& db, const Table& table) 
        : m_db(db), m_table(table) {}
    
    bool create(const T& entity) override {
        Record record(m_table);
        mapEntityToRecord(entity, record);
        return m_db.insert(record);
    }
    
    std::optional<T> findById(const std::string& id) override {
        QueryBuilder query(m_table);
        query.select().where("id", "=", std::stoi(id));
        auto result = m_db.selectOne(query);
        if (result.has_value()) {
            return mapRecordToEntity(result.value());
        }
        return std::nullopt;
    }
    
    std::vector<T> findAll() override {
        QueryBuilder query(m_table);
        query.select();
        auto results = m_db.select(query);
        std::vector<T> entities;
        for (const auto& record : results) {
            entities.push_back(mapRecordToEntity(record));
        }
        return entities;
    }
    
    bool update(const T& entity) override {
        Record record(m_table);
        mapEntityToRecord(entity, record);
        // Assuming entity has getId() method
        return m_db.update(record, "id", std::stoi(entity.getId()));
    }
    
    bool remove(const std::string& id) override {
        return m_db.remove(m_table.getName(), "id", std::stoi(id));
    }
    
    std::vector<T> findBy(const std::function<QueryBuilder(Table&)>& queryBuilder) override {
        QueryBuilder query = queryBuilder(m_table);
        auto results = m_db.select(query);
        std::vector<T> entities;
        for (const auto& record : results) {
            entities.push_back(mapRecordToEntity(record));
        }
        return entities;
    }
    
    std::optional<T> findOneBy(const std::function<QueryBuilder(Table&)>& queryBuilder) override {
        QueryBuilder query = queryBuilder(m_table);
        auto result = m_db.selectOne(query);
        if (result.has_value()) {
            return mapRecordToEntity(result.value());
        }
        return std::nullopt;
    }

protected:
    Database& m_db;
    Table m_table;
    
    // These methods must be implemented by derived classes
    virtual void mapEntityToRecord(const T& entity, Record& record) = 0;
    virtual T mapRecordToEntity(const Record& record) = 0;
};

// Example entity class
class Configuration {
public:
    Configuration() = default;
    
    Configuration(
        const std::string& dbPath,
        size_t maxOccupyDataDirSizeInMB,
        int inactiveDeletionDurationInDays,
        const std::string& nodeSecretKey,
        const std::string& userInterfaceAuthKey,
        const std::string& rootUserName,
        const std::string& rootUserPasswordHash,
        const std::string& blockedIPSCSV
    ) : m_dbPath(dbPath),
        m_maxOccupyDataDirSizeInMB(maxOccupyDataDirSizeInMB),
        m_inactiveDeletionDurationInDays(inactiveDeletionDurationInDays),
        m_nodeSecretKey(nodeSecretKey),
        m_userInterfaceAuthKey(userInterfaceAuthKey),
        m_rootUserName(rootUserName),
        m_rootUserPasswordHash(rootUserPasswordHash),
        m_blockedIPSCSV(blockedIPSCSV) {}
    
    // Getters
    const std::string& getDbPath() const { return m_dbPath; }
    size_t getMaxOccupyDataDirSizeInMB() const { return m_maxOccupyDataDirSizeInMB; }
    int getInactiveDeletionDurationInDays() const { return m_inactiveDeletionDurationInDays; }
    const std::string& getNodeSecretKey() const { return m_nodeSecretKey; }
    const std::string& getUserInterfaceAuthKey() const { return m_userInterfaceAuthKey; }
    const std::string& getRootUserName() const { return m_rootUserName; }
    const std::string& getRootUserPasswordHash() const { return m_rootUserPasswordHash; }
    const std::string& getBlockedIPSCSV() const { return m_blockedIPSCSV; }
    
    // Setters
    void setDbPath(const std::string& dbPath) { m_dbPath = dbPath; }
    void setMaxOccupyDataDirSizeInMB(size_t size) { m_maxOccupyDataDirSizeInMB = size; }
    void setInactiveDeletionDurationInDays(int days) { m_inactiveDeletionDurationInDays = days; }
    void setNodeSecretKey(const std::string& key) { m_nodeSecretKey = key; }
    void setUserInterfaceAuthKey(const std::string& key) { m_userInterfaceAuthKey = key; }
    void setRootUserName(const std::string& name) { m_rootUserName = name; }
    void setRootUserPasswordHash(const std::string& hash) { m_rootUserPasswordHash = hash; }
    void setBlockedIPSCSV(const std::string& csv) { m_blockedIPSCSV = csv; }
    
    // For repository compatibility
    std::string getId() const { return std::to_string(m_id); }
    void setId(int id) { m_id = id; }

private:
    int m_id = 0;
    std::string m_dbPath;
    size_t m_maxOccupyDataDirSizeInMB;
    int m_inactiveDeletionDurationInDays;
    std::string m_nodeSecretKey;
    std::string m_userInterfaceAuthKey;
    std::string m_rootUserName;
    std::string m_rootUserPasswordHash;
    std::string m_blockedIPSCSV;
};

// Configuration repository implementation
class ConfigurationRepository : public BaseRepository<Configuration> {
public:
    ConfigurationRepository(Database& db) 
        : BaseRepository<Configuration>(db, createTable()) {
        m_db.createTable(m_table);
    }
    
    // Custom query methods
    std::optional<Configuration> findByRootUserName(const std::string& userName) {
        return findOneBy([&userName](Table& table) {
            QueryBuilder query(table);
            return query.select().where("rootUserName", "=", userName);
        });
    }
    
    std::vector<Configuration> findByMaxSizeGreaterThan(size_t size) {
        return findBy([size](Table& table) {
            QueryBuilder query(table);
            return query.select().where("maxOccupyDataDirSizeInMB", ">", static_cast<int>(size));
        });
    }

protected:
    void mapEntityToRecord(const Configuration& entity, Record& record) override {
        record.setValue("dbPath", entity.getDbPath());
        record.setValue("maxOccupyDataDirSizeInMB", static_cast<int>(entity.getMaxOccupyDataDirSizeInMB()));
        record.setValue("inactiveDeletionDurationInDays", entity.getInactiveDeletionDurationInDays());
        record.setValue("nodeSecretKey", entity.getNodeSecretKey());
        record.setValue("userInterfaceAuthKey", entity.getUserInterfaceAuthKey());
        record.setValue("rootUserName", entity.getRootUserName());
        record.setValue("rootUserPasswordHash", entity.getRootUserPasswordHash());
        record.setValue("blockedIPSCSV", entity.getBlockedIPSCSV());
    }
    
    Configuration mapRecordToEntity(const Record& record) override {
        Configuration config;
        
        auto id = record.getValue("id");
        if (id.has_value()) {
            config.setId(std::get<int>(id.value()));
        }
        
        auto dbPath = record.getValue("dbPath");
        if (dbPath.has_value()) {
            config.setDbPath(std::get<std::string>(dbPath.value()));
        }
        
        auto maxSize = record.getValue("maxOccupyDataDirSizeInMB");
        if (maxSize.has_value()) {
            config.setMaxOccupyDataDirSizeInMB(std::get<int>(maxSize.value()));
        }
        
        auto inactiveDays = record.getValue("inactiveDeletionDurationInDays");
        if (inactiveDays.has_value()) {
            config.setInactiveDeletionDurationInDays(std::get<int>(inactiveDays.value()));
        }
        
        auto nodeKey = record.getValue("nodeSecretKey");
        if (nodeKey.has_value()) {
            config.setNodeSecretKey(std::get<std::string>(nodeKey.value()));
        }
        
        auto uiKey = record.getValue("userInterfaceAuthKey");
        if (uiKey.has_value()) {
            config.setUserInterfaceAuthKey(std::get<std::string>(uiKey.value()));
        }
        
        auto rootUser = record.getValue("rootUserName");
        if (rootUser.has_value()) {
            config.setRootUserName(std::get<std::string>(rootUser.value()));
        }
        
        auto rootPass = record.getValue("rootUserPasswordHash");
        if (rootPass.has_value()) {
            config.setRootUserPasswordHash(std::get<std::string>(rootPass.value()));
        }
        
        auto blockedIPs = record.getValue("blockedIPSCSV");
        if (blockedIPs.has_value()) {
            config.setBlockedIPSCSV(std::get<std::string>(blockedIPs.value()));
        }
        
        return config;
    }

private:
    static Table createTable() {
        Table table("config");
        table
            .addField(primaryKeyField("id"))
            .addField(textField("dbPath").notNull())
            .addField(integerField("maxOccupyDataDirSizeInMB").notNull())
            .addField(integerField("inactiveDeletionDurationInDays").notNull())
            .addField(textField("nodeSecretKey").notNull())
            .addField(textField("userInterfaceAuthKey").notNull())
            .addField(textField("rootUserName").notNull())
            .addField(textField("rootUserPasswordHash").notNull())
            .addField(textField("blockedIPSCSV"));
        return table;
    }
};

} // namespace core::repository 