#include "geofer/core/config.hpp"
#include "filesystem.hpp"

namespace geofer {

std::string Config::get_prompts_table_name() {
    return "GEOFER_PROMPT_INTERNAL_TABLE";
}

void Config::ConfigPromptTable(duckdb::Connnection& con, std::string& schema_name, const ConfigType type) {
    const std::string table_name = Config::get_prompts_table_name();
    // 查询表是否存在
    auto result = con.Query(duckdb_fmt::format(" SELECT table_name "
                                               " FROM information_schema.tables "
                                               " WHERE table_schema = '{}' "
                                               " AND table_name = '{}'; ",
                                               schema_name, table_name));
    if (result->RowCount() == 0) {
        // 不存在则创建
        con.Query(duckdb_fmt::format(" CREATE TABLE {}.{} ( "
                                     " prompt_name VARCHAR NOT NULL, "
                                     " prompt VARCHAR NOT NULL, "
                                     " updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP , "
                                     " version INT DEFAULT 1, "
                                     " PRIMARY KEY ( prompt_name, version) "
                                     " ); ",
                                     schema_name, table_name));
        if (type == ConfigType::GLOBAL) {
            con.Query(duckdb_fmt::format(" INSERT INTO {}.{} (prompt_name, prompt) "
                                         " VALUES ('hello-world', 'Tell me hello word'); ",
                                         schema_name, table_name));
        }
    }
}

} // namespace geofer


