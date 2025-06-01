#include "geofer/core/config.hpp"
#include "filesystem.hpp"
#include "geofer/secret_manager/secret_manager.hpp"
#include <fmt/format.h>

namespace geofer {

// 当前实例
duckdb::DatabaseInstance* Config::db;

// 获取 schema 名称
std::string Config::get_schema_name() {
    return "geofer_config";
}

// 获取存储路径
std::filesystem::path Config::get_global_storage_path() {
#ifdef _WIN32
    const char* homeDir = getenv("USERPROFILE");
#else
    const char* homeDir = getenv("HOME");
    const char* homeDir = getenv("HOME");
#endif
    if (homeDir == nullptr) {
        throw std::runtime_error("Could not find home directory");
    }
    return std::filesystem::path(homeDir) / ".duckdb" / "geofer_storage" / "geofer.db";
}

// 获取连接
duckdb::Connection Config::GetConnection(duckdb::DatabaseInstance* db) {
    if (db) {
        Config::db = db;
    }
    // 构造连接对象并返回
    duckdb::Connection con(*Config::db);
    return con;
}

// 获取全局连接
duckdb::Connection Config::GetGlobalConnection() {
    const duckdb::DuckDB db(Config::get_global_storage_path().string());
    duckdb::Connection con(*db.instance);
    return con;
}

// 设置全局存储路径
void Config::SetupGlobalStorageLocation() {
    const auto geofer_global_path = get_global_storage_path();
    const auto geoferDir = geofer_global_path.parent_path();
    if (!std::filesystem::exists(geoferDir)) {
        try {
            std::filesystem::create_directory(geoferDir);
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error creating directories: " << e.what() << std::endl;
        }
    }
}

// 配置 schema
void Config::ConfigSchema(duckdb::Connection& con, std::string& schema_name) {
    // 查询 schema_name 是否已经存在
    auto result = con.Query(duckdb_fmt::format(" SELECT * "
                                               " FROM information_schema.schemata "
                                               " WHERE schema_name = '{}'; ",
                                               schema_name));
    if (result->RowCount() == 0) {
        // schema 不存在, 创建 schema
        con.Query(duckdb_fmt::format("CREATE SCHEMA {};", schema_name));
    }
}

// 配置全局
void Config::ConfigureGlobal() {
    auto con = Config::GetGlobalConnection();
    ConfigureTables(con, ConfigType::GLOBAL);
}

// 配置本地
void Config::ConfigureLocal(duckdb::DatabaseInstance& db) {
    auto con = Config::GetConnection(&db);
    ConfigureTables(con, ConfigType::LOCAL);
    con.Query(
        duckdb_fmt::format("ATTACH DATABASE '{}' AS geofer_storage;", Config::get_global_storage_path().string())
    );
}

void Config::ConfigureTables(duckdb::Connection& con, const ConfigType type) {
    con.BeginTransaction();
    std::string schema = Config::get_schema_name();
    ConfigSchema(con, schema);
    ConfigModelTable(con, schema, type);
    ConfigPromptTable(con, schema, type);
    con.Commit();
}

// 注册 db
void Config::Configure(duckdb::DatabaseInstance& db) {
    Registry::Register(db);
    SecretManager::Register(db);
    if (const auto db_path = db.config.options.database_path; db_path != get_global_storage_path().string()) {
        SetupGlobalStorageLocation();
        ConfigureGlobal();
        ConfigureLocal(db);
    }
}

} // namespace geofer