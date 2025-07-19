#include "orm.hh"
#include <sstream>
#include <algorithm>
#include <cstring>

namespace core::orm {

// Field implementation
Field::Field(const std::string& name, FieldType type) 
    : m_name(name), m_type(type) {}

Field& Field::primaryKey() {
    m_constraints.push_back(Constraint::PRIMARY_KEY);
    return *this;
}

Field& Field::notNull() {
    m_constraints.push_back(Constraint::NOT_NULL);
    return *this;
}

Field& Field::unique() {
    m_constraints.push_back(Constraint::UNIQUE);
    return *this;
}

Field& Field::autoIncrement() {
    m_constraints.push_back(Constraint::AUTO_INCREMENT);
    return *this;
}

std::string Field::getDefinition() const {
    std::stringstream ss;
    ss << m_name << " ";
    
    switch (m_type) {
        case FieldType::INTEGER:
            ss << "INTEGER";
            break;
        case FieldType::REAL:
            ss << "REAL";
            break;
        case FieldType::TEXT:
            ss << "TEXT";
            break;
        case FieldType::BLOB:
            ss << "BLOB";
            break;
        case FieldType::BOOLEAN:
            ss << "INTEGER"; // SQLite doesn't have BOOLEAN, use INTEGER
            break;
    }
    
    for (const auto& constraint : m_constraints) {
        switch (constraint) {
            case Constraint::PRIMARY_KEY:
                ss << " PRIMARY KEY";
                break;
            case Constraint::NOT_NULL:
                ss << " NOT NULL";
                break;
            case Constraint::UNIQUE:
                ss << " UNIQUE";
                break;
            case Constraint::AUTO_INCREMENT:
                ss << " AUTOINCREMENT";
                break;
        }
    }
    
    return ss.str();
}

// Table implementation
Table::Table(const std::string& name) : m_name(name) {}

Table& Table::addField(const Field& field) {
    m_fields.push_back(field);
    return *this;
}

std::optional<Field> Table::getField(const std::string& name) const {
    for (const auto& field : m_fields) {
        if (field.getName() == name) {
            return field;
        }
    }
    return std::nullopt;
}

std::string Table::getCreateStatement() const {
    std::stringstream ss;
    ss << "CREATE TABLE IF NOT EXISTS " << m_name << " (";
    
    for (size_t i = 0; i < m_fields.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << m_fields[i].getDefinition();
    }
    
    ss << ");";
    return ss.str();
}

std::string Table::getInsertStatement() const {
    std::stringstream ss;
    ss << "INSERT INTO " << m_name << " (";
    
    for (size_t i = 0; i < m_fields.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << m_fields[i].getName();
    }
    
    ss << ") VALUES (";
    
    for (size_t i = 0; i < m_fields.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "?";
    }
    
    ss << ");";
    return ss.str();
}

std::string Table::getSelectStatement(const std::vector<std::string>& fields) const {
    std::stringstream ss;
    ss << "SELECT ";
    
    if (fields.empty()) {
        ss << "*";
    } else {
        for (size_t i = 0; i < fields.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << fields[i];
        }
    }
    
    ss << " FROM " << m_name;
    return ss.str();
}

std::string Table::getUpdateStatement(const std::vector<std::string>& fields) const {
    std::stringstream ss;
    ss << "UPDATE " << m_name << " SET ";
    
    if (fields.empty()) {
        for (size_t i = 0; i < m_fields.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << m_fields[i].getName() << " = ?";
        }
    } else {
        for (size_t i = 0; i < fields.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << fields[i] << " = ?";
        }
    }
    
    return ss.str();
}

std::string Table::getDeleteStatement() const {
    return "DELETE FROM " + m_name;
}

// Record implementation
Record::Record(const Table& table) : m_table(table) {}

void Record::setValue(const std::string& fieldName, const Value& value) {
    m_values[fieldName] = value;
}

std::optional<Value> Record::getValue(const std::string& fieldName) const {
    auto it = m_values.find(fieldName);
    if (it != m_values.end()) {
        return it->second;
    }
    return std::nullopt;
}

// QueryBuilder implementation
QueryBuilder::QueryBuilder(const Table& table) 
    : m_table(table), m_isSelect(false) {}

QueryBuilder& QueryBuilder::select(const std::vector<std::string>& fields) {
    m_query = m_table.getSelectStatement(fields);
    m_isSelect = true;
    return *this;
}

QueryBuilder& QueryBuilder::where(const std::string& field, const std::string& op, const Value& value) {
    m_conditions.clear();
    m_parameters.clear();
    m_conditions.push_back(field + " " + op + " ?");
    m_parameters.push_back(value);
    return *this;
}

QueryBuilder& QueryBuilder::andWhere(const std::string& field, const std::string& op, const Value& value) {
    m_conditions.push_back("AND " + field + " " + op + " ?");
    m_parameters.push_back(value);
    return *this;
}

QueryBuilder& QueryBuilder::orWhere(const std::string& field, const std::string& op, const Value& value) {
    m_conditions.push_back("OR " + field + " " + op + " ?");
    m_parameters.push_back(value);
    return *this;
}

QueryBuilder& QueryBuilder::orderBy(const std::string& field, bool ascending) {
    m_orderBy = " ORDER BY " + field + (ascending ? " ASC" : " DESC");
    return *this;
}

QueryBuilder& QueryBuilder::limit(int count) {
    m_limit = count;
    return *this;
}

QueryBuilder& QueryBuilder::offset(int count) {
    m_offset = count;
    return *this;
}

std::string QueryBuilder::build() const {
    std::stringstream ss;
    ss << m_query;
    
    if (!m_conditions.empty()) {
        ss << " WHERE ";
        for (size_t i = 0; i < m_conditions.size(); ++i) {
            if (i > 0) ss << " ";
            ss << m_conditions[i];
        }
    }
    
    if (!m_orderBy.empty()) {
        ss << m_orderBy;
    }
    
    if (m_limit.has_value()) {
        ss << " LIMIT " << m_limit.value();
    }
    
    if (m_offset.has_value()) {
        ss << " OFFSET " << m_offset.value();
    }
    
    ss << ";";
    return ss.str();
}

// Database implementation
Database::Database(const std::string& path) : m_path(path), m_db(nullptr) {
    if (sqlite3_open(path.c_str(), &m_db) != SQLITE_OK) {
        const std::string errorMsg = sqlite3_errmsg(m_db);
        const std::string msg = "Unable to open SQLite3 DB: " + path + ", " + errorMsg;
        fsn::logger::consoleLog(msg, fsn::logger::FATAL);
        throw std::runtime_error(msg);
    }
}

Database::~Database() {
    if (m_db) {
        sqlite3_close(m_db);
    }
}

bool Database::createTable(const Table& table) {
    return execute(table.getCreateStatement());
}

bool Database::dropTable(const std::string& tableName) {
    return execute("DROP TABLE IF EXISTS " + tableName + ";");
}

bool Database::tableExists(const std::string& tableName) {
    std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=?;";
    auto result = executeQuery(sql);
    return !result.empty();
}

bool Database::insert(const Record& record) {
    std::string sql = record.getTable().getInsertStatement();
    std::vector<Value> params;
    
    for (const auto& field : record.getTable().getFields()) {
        auto value = record.getValue(field.getName());
        if (value.has_value()) {
            params.push_back(value.value());
        } else {
            // Use null for missing values
            params.push_back(std::string(""));
        }
    }
    
    return executePrepared(sql, params);
}

std::vector<Record> Database::select(const QueryBuilder& query) {
    std::vector<Record> records;
    std::string sql = query.build();
    auto params = query.getParameters();
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        fsn::logger::consoleLog("Failed to prepare statement: " + sql, fsn::logger::ERROR);
        return records;
    }
    
    // Bind parameters
    for (size_t i = 0; i < params.size(); ++i) {
        const auto& param = params[i];
        int paramIndex = i + 1;
        
        std::visit([stmt, paramIndex](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>) {
                sqlite3_bind_int(stmt, paramIndex, value);
            } else if constexpr (std::is_same_v<T, double>) {
                sqlite3_bind_double(stmt, paramIndex, value);
            } else if constexpr (std::is_same_v<T, std::string>) {
                sqlite3_bind_text(stmt, paramIndex, value.c_str(), -1, SQLITE_TRANSIENT);
            } else if constexpr (std::is_same_v<T, std::vector<char>>) {
                sqlite3_bind_blob(stmt, paramIndex, value.data(), value.size(), SQLITE_TRANSIENT);
            } else if constexpr (std::is_same_v<T, bool>) {
                sqlite3_bind_int(stmt, paramIndex, value ? 1 : 0);
            }
        }, param);
    }
    
    // Execute and fetch results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Record record(query.getTable());
        int columnCount = sqlite3_column_count(stmt);
        
        for (int i = 0; i < columnCount; ++i) {
            const char* columnName = sqlite3_column_name(stmt, i);
            int columnType = sqlite3_column_type(stmt, i);
            
            if (columnType == SQLITE_NULL) continue;
            
            Value value;
            switch (columnType) {
                case SQLITE_INTEGER:
                    value = sqlite3_column_int(stmt, i);
                    break;
                case SQLITE_FLOAT:
                    value = sqlite3_column_double(stmt, i);
                    break;
                case SQLITE_TEXT:
                    value = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
                    break;
                case SQLITE_BLOB:
                    {
                        const void* blob = sqlite3_column_blob(stmt, i);
                        int size = sqlite3_column_bytes(stmt, i);
                        std::vector<char> data(static_cast<const char*>(blob), 
                                             static_cast<const char*>(blob) + size);
                        value = data;
                    }
                    break;
            }
            
            record.setValue(columnName, value);
        }
        
        records.push_back(std::move(record));
    }
    
    sqlite3_finalize(stmt);
    return records;
}

std::optional<Record> Database::selectOne(const QueryBuilder& query) {
    auto records = select(query);
    if (records.empty()) {
        return std::nullopt;
    }
    return records[0];
}

bool Database::update(const Record& record, const std::string& whereField, const Value& whereValue) {
    std::string sql = record.getTable().getUpdateStatement() + " WHERE " + whereField + " = ?;";
    std::vector<Value> params;
    
    for (const auto& field : record.getTable().getFields()) {
        auto value = record.getValue(field.getName());
        if (value.has_value()) {
            params.push_back(value.value());
        }
    }
    
    params.push_back(whereValue);
    return executePrepared(sql, params);
}

bool Database::remove(const std::string& tableName, const std::string& whereField, const Value& whereValue) {
    std::string sql = "DELETE FROM " + tableName + " WHERE " + whereField + " = ?;";
    return executePrepared(sql, {whereValue});
}

bool Database::execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        fsn::logger::consoleLog("SQL execution failed: " + std::string(errMsg), fsn::logger::ERROR);
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

std::vector<std::vector<Value>> Database::executeQuery(const std::string& sql) {
    std::vector<std::vector<Value>> results;
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        fsn::logger::consoleLog("Failed to prepare query: " + sql, fsn::logger::ERROR);
        return results;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<Value> row;
        int columnCount = sqlite3_column_count(stmt);
        
        for (int i = 0; i < columnCount; ++i) {
            int columnType = sqlite3_column_type(stmt, i);
            
            if (columnType == SQLITE_NULL) {
                row.push_back(std::string(""));
                continue;
            }
            
            Value value;
            switch (columnType) {
                case SQLITE_INTEGER:
                    value = sqlite3_column_int(stmt, i);
                    break;
                case SQLITE_FLOAT:
                    value = sqlite3_column_double(stmt, i);
                    break;
                case SQLITE_TEXT:
                    value = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
                    break;
                case SQLITE_BLOB:
                    {
                        const void* blob = sqlite3_column_blob(stmt, i);
                        int size = sqlite3_column_bytes(stmt, i);
                        std::vector<char> data(static_cast<const char*>(blob), 
                                             static_cast<const char*>(blob) + size);
                        value = data;
                    }
                    break;
            }
            
            row.push_back(value);
        }
        
        results.push_back(std::move(row));
    }
    
    sqlite3_finalize(stmt);
    return results;
}

bool Database::beginTransaction() {
    return execute("BEGIN TRANSACTION;");
}

bool Database::commitTransaction() {
    return execute("COMMIT;");
}

bool Database::rollbackTransaction() {
    return execute("ROLLBACK;");
}

bool Database::executePrepared(const std::string& sql, const std::vector<Value>& params) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        fsn::logger::consoleLog("Failed to prepare statement: " + sql, fsn::logger::ERROR);
        return false;
    }
    
    // Bind parameters
    for (size_t i = 0; i < params.size(); ++i) {
        const auto& param = params[i];
        int paramIndex = i + 1;
        
        std::visit([stmt, paramIndex](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>) {
                sqlite3_bind_int(stmt, paramIndex, value);
            } else if constexpr (std::is_same_v<T, double>) {
                sqlite3_bind_double(stmt, paramIndex, value);
            } else if constexpr (std::is_same_v<T, std::string>) {
                sqlite3_bind_text(stmt, paramIndex, value.c_str(), -1, SQLITE_TRANSIENT);
            } else if constexpr (std::is_same_v<T, std::vector<char>>) {
                sqlite3_bind_blob(stmt, paramIndex, value.data(), value.size(), SQLITE_TRANSIENT);
            } else if constexpr (std::is_same_v<T, bool>) {
                sqlite3_bind_int(stmt, paramIndex, value ? 1 : 0);
            }
        }, param);
    }
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

// Utility functions
Field integerField(const std::string& name) {
    return Field(name, FieldType::INTEGER);
}

Field realField(const std::string& name) {
    return Field(name, FieldType::REAL);
}

Field textField(const std::string& name) {
    return Field(name, FieldType::TEXT);
}

Field blobField(const std::string& name) {
    return Field(name, FieldType::BLOB);
}

Field booleanField(const std::string& name) {
    return Field(name, FieldType::BOOLEAN);
}

Field primaryKeyField(const std::string& name) {
    return Field(name, FieldType::INTEGER).primaryKey().autoIncrement();
}

} // namespace core::orm 