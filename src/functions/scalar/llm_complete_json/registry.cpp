#include "geofer/functions/scalar/llm_complete_json.hpp"
#include "geofer/registry/registry.hpp"

namespace geofer {

void ScalarRegistry::RegisterLlmCompleteJson(duckdb::DatabaseInstance& db) {
  duckdb::ExtensionUtil::RegisterFunction(
      db, duckdb::ScalarFunction("llm_complete_json", {}, duckdb::LogicalType::JSON(), LlmCompleteJson::Execute,
                                 nullptr, nullptr, nullptr, nullptr, duckdb::LogicalType::ANY));
}

} // namespace geofer