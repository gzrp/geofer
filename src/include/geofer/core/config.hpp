#pragma once

#include "filesystem.hpp"
#include "geofer/core/common.hpp"
#include "geofer/registry/registry.hpp"
#include <fmt/format.h>

namespace geofer {

enum ConfigType { LOCAL,
                  GLOBAL };

class Config {
public:
    static duckdb::DatabaseInstance* db;          // 当前 db 实例
    static duckdb::DatabaseInstance* global_db;   // 全局 db 实例
    static duckdb::Connection GetConnection(duckdb::DatabaseInstance* db = nullptr);  // 获取当前 db 的连接
    static duckdb::Connection GetGlobalConnection(); // 获取全局 db 实例的连接
    static void Configure(duckdb::DatabaseInstance& db); // 对db 实例进行配置
    static void ConfigureGlobal();  // 对全局实例进行配置
    static void ConfigureTables(duckdb::Connection& con, ConfigType type);  // 对表进行配置
    static void ConfigureLocal(duckdb::DatabaseInstance& db);  // 对当前实例进行配置

    static std::string get_schema_name(); // 获取 schema 名称
	static std::filesystem::path get_global_storage_path();	// 获取全局存储路径
	static std::string get_default_models_table_name();		// 获取模型模型表名称
	static std::string get_user_defined_models_table_name();	// 获取用户自定义模型表名称
	static std::string get_prompts_table_name();				// 获取 prompts 表名称

	constexpr static int32_t default_context_window = 12800;	// llm 一次推理中记住的 总 token 数
	constexpr static int32_t default_max_output_tokens = 4096;  // llm 模型一次最多输出的 总 token 数

private:
	static void SetupGlobalStorageLocation(); 	// 设置全局存储路径
	static void ConfigSchema(duckdb::Connection& con, std::string& schema_name);	// 配置 schema
	static void ConfigPromptTable(duckdb::Connection& con, std::string& schema_name, ConfigType type);	// 配置 prompt 表
	static void ConfigModelTable(duckdb::Connection& con, std::string& schema_name, ConfigType type);	// 配置模型表
	static void SetupDefaultModelsConfig(duckdb::Connection& con, std::string& schema_name);			// 设置默认模型配置
	static void SetupUserDefinedModelsConfig(duckdb::Connection& con, std::string& schema_name);		// 设置用户自定义模型配置
}

} // namaspece geofer