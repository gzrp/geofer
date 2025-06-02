#include "geofer/functions/scalar/llm_complete.hpp"
#include "geofer/registry/registry.hpp"

namespace geofer {

void ScalarRegistry::RegisterLlmComplete(duckdb::DatabaseInstance& db) {
  // 注冊函數
  // 函数名，必须参数列表 {}，返回类型，执行函数，，，，，可变参数列表
  duckdb::ExtensionUtil::RegisterFunction(db, duckdb::ScalarFunction("llm_complete", {}, duckdb::LogicalType::VARCHAR,
                                                                     LlmComplete::Execute, nullptr, nullptr, nullptr,
                                                                     nullptr, duckdb::LogicalType::ANY));
}

} // namespace geofer