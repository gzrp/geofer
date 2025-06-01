#include "geofer/core/config.hpp"
#include "filesystem.hpp"

namespace geofer {

std::string Config::get_default_models_table_name() {
    return "GEOFER_MODEL_DEFAULT_INTERNAL_TABLE";
}

std::string Config::get_user_defined_models_table_name() {
    return "GROFER_MODEL_USER_DEFINED_INTERNAL_TABLE";
}

void Config::SetupDefaultModelsConfig(duckdb::Connection& con, std::string& schema_name) {
    const std::string table_name = Config::get_default_models_table_name();
    // 查询表模型表是否存在
    auto result = con.Query(duckdb_fmt::format(" SELECT table_name "
                                               " FROM information_schema.tables "
                                               " WHERE table_schema = '{}' "
                                               " AND table_name = '{}'; ",
                                               schema_name, table_name ));
    if (result->RowCount() == 0) {
        // 不存在，则创建
        con.Query(duckdb_fmt::format(" INSTALL JSON; "
                                     " LOAD JSON; "
                                     " CREATE TABLE {}.{} ( "
                                     " model_name VARCHAR NOT NULL PRIMARY KEY, "
                                     " model VARCHAR NOT NULL, "
                                     " provider_name VARCHAR NOT NULL, "
                                     " model_args JSON NOT NULL "
                                     " ); ",
                                     schema_name, table_name ));
        // 出入默认支持的模型
        con.Query(duckdb_fmt::format(
            " INSERT INTO {}.{} (model_name, model, provider_name, model_args) "
            " VALUES "
            " ('default', 'gpt-4o-mini', 'openai', '{{\"context_window\":128000,\"max_output_tokens\":16384}}'),"
            " ('gpt-4o-mini', 'gpt-4o-mini', 'openai', '{{\"context_window\":128000,\"max_output_tokens\":16384}}'),"
            " ('gpt-4o', 'gpt-4o', 'openai', '{{\"context_window\":128000,\"max_output_tokens\":16384}}'),"
            " ('text-embedding-3-large', 'text-embedding-3-large', 'openai', '{{\"context_window\":{},\"max_output_tokens\":{}}}'),"
            " ('text-embedding-3-small', 'text-embedding-3-small', 'openai', '{{\"context_window\":{},\"max_output_tokens\":{}}}'),",
            schema_name, table_name, Config::default_context_window, Config::default_max_output_tokens,
            Config::default_context_window, Config::default_max_output_tokens));
    }
}

void Config::SetupUserDefinedModelsConfig(duckdb::Connection& con, std::string& schema_name) {
    const std::string table_name = Config::get_user_defined_models_table_name();
    // 查询表名是否存在
    auto result = con.Query(duckdb_fmt::format(" SELECT table_name "
                                               " FROM information_schema.tables "
                                               " WHERE table_schema = '{}' "
                                               " AND table_name = '{}'; ",
                                               schema_name, table_name ));
    if (result->RowCount() == 0) {
        // 不存在则创建
        con.Query(duckdb_fmt::format(" INSTALL JSON; "
                                     " LOAD JSON; "
                                     " CREATE TABLE {}.{} ( "
                                     " model_name VARCHAR NOT NULL PRIMARY KEY, "
                                     " model VARCHAR NOT NULL, "
                                     " provider_name VARCHAR NOT NULL, "
                                     " model_args JSON NOT NULL "
                                     " ); "
                                     schema_name, table_name ));
    }
}

void Config::ConfigModelTable(duckdb::Connection& con, std::string& schema_name, const ConfigType type) {
    if (type == ConfigType::GlOBAL) {
        SetupDefaultModelsConfig(con, schema_name);
    }
    SetupUserDefinedModelsConfig(con, schema_name);
}

} // namespace geofer