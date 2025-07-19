#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <functional>
#include <variant>
#include <optional>
#include <sqlite3.h>
#include "dfs-core.hh"

namespace core::orm {

// Forward declarations
class Field;
class Table;
class QueryBuilder;
class Database;

// Field types supported by the ORM
enum class FieldType {
    INTEGER,
    REAL,
    TEXT,
    BLOB,
    BOOLEAN
};

// Field constraints
enum class Constraint {
    PRIMARY_KEY,
    NOT_NULL,
    UNIQUE,
    AUTO_INCREMENT
};

// Represents a database field
class Field {
public:
    Field(const std::string& name, FieldType type);
    
    // Constraint setters
    Field& primaryKey();
    Field& notNull();
    Field& unique();
    Field& autoIncrement();
    
    // Get field definition for CREATE TABLE
    std::string getDefinition() const;
    
    // Get field name
    const std::string& getName() const { return m_name; }
    
    // Get field type
    FieldType getType() const { return m_type; }

private:
    std::string m_name;
    FieldType m_type;
    std::vector<Constraint> m_constraints;
};

// Represents a database table
class Table {
public:
    Table(const std::string& name);
    
    // Add fields
    Table& addField(const Field& field);
    
    // Get table name
    const std::string& getName() const { return m_name; }
    
    // Get all fields
    const std::vector<Field>& getFields() const { return m_fields; }
    
    // Get field by name
    std::optional<Field> getField(const std::string& name) const;
    
    // Generate CREATE TABLE statement
    std::string getCreateStatement() const;
    
    // Generate INSERT statement
    std::string getInsertStatement() const;
    
    // Generate SELECT statement
    std::string getSelectStatement(const std::vector<std::string>& fields = {}) const;
    
    // Generate UPDATE statement
    std::string getUpdateStatement(const std::vector<std::string>& fields = {}) const;
    
    // Generate DELETE statement
    std::string getDeleteStatement() const;

private:
    std::string m_name;
    std::vector<Field> m_fields;
};

// Value types for database operations
using Value = std::variant<int, double, std::string, std::vector<char>, bool>;

// Represents a database record
class Record {
public:
    Record(const Table& table);
    
    // Set field values
    void setValue(const std::string& fieldName, const Value& value);
    
    // Get field value
    std::optional<Value> getValue(const std::string& fieldName) const;
    
    // Get all values as map
    const std::unordered_map<std::string, Value>& getValues() const { return m_values; }
    
    // Get the table this record belongs to
    const Table& getTable() const { return m_table; }

private:
    const Table& m_table;
    std::unordered_map<std::string, Value> m_values;
};

// Query builder for complex queries
class QueryBuilder {
public:
    QueryBuilder(const Table& table);
    
    // SELECT operations
    QueryBuilder& select(const std::vector<std::string>& fields = {});
    QueryBuilder& where(const std::string& field, const std::string& op, const Value& value);
    QueryBuilder& andWhere(const std::string& field, const std::string& op, const Value& value);
    QueryBuilder& orWhere(const std::string& field, const std::string& op, const Value& value);
    QueryBuilder& orderBy(const std::string& field, bool ascending = true);
    QueryBuilder& limit(int count);
    QueryBuilder& offset(int count);
    
    // Build the final query
    std::string build() const;
    
    // Get parameters for prepared statement
    const std::vector<Value>& getParameters() const { return m_parameters; }
    
    // Get the table this query builder is working with
    const Table& getTable() const { return m_table; }

private:
    const Table& m_table;
    std::string m_query;
    std::vector<std::string> m_conditions;
    std::vector<Value> m_parameters;
    std::string m_orderBy;
    std::optional<int> m_limit;
    std::optional<int> m_offset;
    bool m_isSelect;
};

// Main database class
class Database {
public:
    Database(const std::string& path);
    ~Database();
    
    // Table management
    bool createTable(const Table& table);
    bool dropTable(const std::string& tableName);
    bool tableExists(const std::string& tableName);
    
    // CRUD operations
    bool insert(const Record& record);
    std::vector<Record> select(const QueryBuilder& query);
    std::optional<Record> selectOne(const QueryBuilder& query);
    bool update(const Record& record, const std::string& whereField, const Value& whereValue);
    bool remove(const std::string& tableName, const std::string& whereField, const Value& whereValue);
    
    // Raw SQL execution
    bool execute(const std::string& sql);
    std::vector<std::vector<Value>> executeQuery(const std::string& sql);
    
    // Transaction support
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

private:
    std::string m_path;
    sqlite3* m_db;
    
    // Helper methods
    std::string valueToString(const Value& value) const;
    Value columnToValue(sqlite3_stmt* stmt, int column, FieldType type) const;
    bool executePrepared(const std::string& sql, const std::vector<Value>& params);
};

// Utility functions for creating common field types
Field integerField(const std::string& name);
Field realField(const std::string& name);
Field textField(const std::string& name);
Field blobField(const std::string& name);
Field booleanField(const std::string& name);

// Utility function to create primary key field
Field primaryKeyField(const std::string& name = "id");

} // namespace core::orm 