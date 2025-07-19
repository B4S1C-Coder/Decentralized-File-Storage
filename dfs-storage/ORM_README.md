# Lightweight SQLite ORM

A lightweight, header-only ORM (Object-Relational Mapping) system for SQLite that provides automatic SQL generation, type-safe operations, and a clean API for database operations.

## Features

- **Automatic SQL Generation**: No need to write SQL manually
- **Type-Safe Operations**: Compile-time type checking for field values
- **Flexible Query Building**: Chainable query builder with support for complex conditions
- **Repository Pattern**: High-level abstraction for common database operations
- **Transaction Support**: Built-in transaction management
- **Prepared Statements**: Automatic SQL injection prevention
- **Lightweight**: No external dependencies beyond SQLite

## Quick Start

### Basic ORM Usage

```cpp
#include "orm.hh"
using namespace core::orm;

// Create database
Database db("myapp.db");

// Define table schema
Table userTable("users");
userTable
    .addField(primaryKeyField("id"))
    .addField(textField("name").notNull())
    .addField(textField("email").unique().notNull())
    .addField(integerField("age"))
    .addField(booleanField("active"));

// Create table
db.createTable(userTable);

// Insert data
Record userRecord(userTable);
userRecord.setValue("name", std::string("John Doe"));
userRecord.setValue("email", std::string("john@example.com"));
userRecord.setValue("age", 30);
userRecord.setValue("active", true);

db.insert(userRecord);

// Query data
QueryBuilder query(userTable);
query.select().where("age", ">", 25).andWhere("active", "=", true);

auto results = db.select(query);
for (const auto& record : results) {
    auto name = record.getValue("name");
    if (name.has_value()) {
        std::cout << "Name: " << std::get<std::string>(name.value()) << std::endl;
    }
}
```

### Repository Pattern Usage

```cpp
#include "repository.hh"
using namespace core::repository;

// Define entity
class User {
public:
    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    std::string getEmail() const { return m_email; }
    void setEmail(const std::string& email) { m_email = email; }
    int getAge() const { return m_age; }
    void setAge(int age) { m_age = age; }
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }
    
    std::string getId() const { return std::to_string(m_id); }
    void setId(int id) { m_id = id; }

private:
    int m_id = 0;
    std::string m_name;
    std::string m_email;
    int m_age;
    bool m_active;
};

// Define repository
class UserRepository : public BaseRepository<User> {
public:
    UserRepository(Database& db) : BaseRepository<User>(db, createTable()) {
        m_db.createTable(m_table);
    }
    
    // Custom query methods
    std::optional<User> findByEmail(const std::string& email) {
        return findOneBy([&email](Table& table) {
            QueryBuilder query(table);
            return query.select().where("email", "=", email);
        });
    }
    
    std::vector<User> findActiveUsers() {
        return findBy([](Table& table) {
            QueryBuilder query(table);
            return query.select().where("active", "=", true);
        });
    }

protected:
    void mapEntityToRecord(const User& user, Record& record) override {
        record.setValue("name", user.getName());
        record.setValue("email", user.getEmail());
        record.setValue("age", user.getAge());
        record.setValue("active", user.isActive());
    }
    
    User mapRecordToEntity(const Record& record) override {
        User user;
        
        auto id = record.getValue("id");
        if (id.has_value()) user.setId(std::get<int>(id.value()));
        
        auto name = record.getValue("name");
        if (name.has_value()) user.setName(std::get<std::string>(name.value()));
        
        auto email = record.getValue("email");
        if (email.has_value()) user.setEmail(std::get<std::string>(email.value()));
        
        auto age = record.getValue("age");
        if (age.has_value()) user.setAge(std::get<int>(age.value()));
        
        auto active = record.getValue("active");
        if (active.has_value()) user.setActive(std::get<bool>(active.value()));
        
        return user;
    }

private:
    static Table createTable() {
        Table table("users");
        table
            .addField(primaryKeyField("id"))
            .addField(textField("name").notNull())
            .addField(textField("email").unique().notNull())
            .addField(integerField("age"))
            .addField(booleanField("active"));
        return table;
    }
};

// Usage
Database db("myapp.db");
UserRepository userRepo(db);

User user;
user.setName("Jane Doe");
user.setEmail("jane@example.com");
user.setAge(25);
user.setActive(true);

userRepo.create(user);

auto foundUser = userRepo.findByEmail("jane@example.com");
if (foundUser.has_value()) {
    std::cout << "Found user: " << foundUser->getName() << std::endl;
}
```

## Field Types

The ORM supports the following field types:

- `integerField()` - INTEGER
- `realField()` - REAL
- `textField()` - TEXT
- `blobField()` - BLOB
- `booleanField()` - INTEGER (SQLite doesn't have BOOLEAN)

## Field Constraints

- `.primaryKey()` - Primary key constraint
- `.notNull()` - NOT NULL constraint
- `.unique()` - UNIQUE constraint
- `.autoIncrement()` - AUTOINCREMENT (SQLite)

## Query Builder

The QueryBuilder provides a fluent interface for building complex queries:

```cpp
QueryBuilder query(table);
query.select({"name", "email"})           // Select specific fields
     .where("age", ">", 18)               // WHERE age > 18
     .andWhere("active", "=", true)       // AND active = true
     .orWhere("admin", "=", true)         // OR admin = true
     .orderBy("name", true)               // ORDER BY name ASC
     .limit(10)                           // LIMIT 10
     .offset(20);                         // OFFSET 20

auto results = db.select(query);
```

## Transactions

```cpp
Database db("myapp.db");

db.beginTransaction();
try {
    // Multiple operations
    db.insert(record1);
    db.insert(record2);
    db.update(record3);
    
    db.commitTransaction();
} catch (...) {
    db.rollbackTransaction();
    throw;
}
```

## Comparison with Old DTO Approach

### Old DTO Approach (Manual SQL)

```cpp
class ConfigurationDTO : public DTO {
public:
    std::string getCreateTableStatement() const override {
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
    
    std::string getInsertStatement() const override {
        return "INSERT INTO config(dbPath, maxOccupyDataDirSizeInMB, "
               "inactiveDeletionDurationInDays, nodeSecretKey, "
               "userInterfaceAuthKey, rootUserName, rootUserPasswordHash, "
               "blockedIPSCSV) VALUES (\"" + dbpath + "\", " + 
               std::to_string(maxOccupyDataDirSizeInMB) + ", " +
               std::to_string(inactiveDeletionDurationInDays) + ", \"" +
               nodeSecretKey + "\", \"" + userInterfaceAuthKey + "\", \"" +
               rootUserName + "\", \"" + rootUserPasswordHash + "\", \"" +
               blockedIPSCSV + "\");";
    }
    
    // ... more manual SQL for each operation
};
```

### New ORM Approach (Automatic SQL)

```cpp
// Define table once
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

// All SQL is generated automatically!
db.createTable(configTable);  // Automatic CREATE TABLE
db.insert(record);           // Automatic INSERT
db.select(query);            // Automatic SELECT
db.update(record);           // Automatic UPDATE
db.remove(table, "id", 1);   // Automatic DELETE
```

## Benefits

1. **No Manual SQL**: Automatic SQL generation eliminates errors
2. **Type Safety**: Compile-time checking prevents type mismatches
3. **Flexible Queries**: QueryBuilder supports complex conditions
4. **Reusable**: Table definitions can be reused across the application
5. **Maintainable**: Changes to schema only require updating one place
6. **Testable**: Easy to mock and test database operations
7. **Performance**: Uses prepared statements for better performance
8. **Security**: Automatic SQL injection prevention

## Building and Testing

```bash
# Build the library
mkdir build && cd build
cmake ..
make

# Run ORM tests
./test_orm

# Run repository pattern tests
./test_repository
```

## Migration from DTOs

To migrate from the old DTO approach:

1. Replace DTO classes with entity classes
2. Define table schemas using the ORM
3. Replace manual SQL calls with ORM operations
4. Use QueryBuilder for complex queries
5. Consider using the Repository pattern for higher-level abstraction

The ORM is designed to be a drop-in replacement that provides much better developer experience while maintaining the same functionality. 