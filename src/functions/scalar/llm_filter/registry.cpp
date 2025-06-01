#include "geofer/functions/scalar/llm_filter.hpp"
#include "geofer/registry/registry.hpp"

namespace geofer {

void ScalarRegistry::RegisterLlmFilter(duckdb::DatabaseInstance& db) {
  duckdb::ExtensionUtil::RegisterFunction(
      db, duckdb::ScalarFunction("llm_filter",
                                 {duckdb::LogicalType::ANY, duckdb::LogicalType::ANY, duckdb::LogicalType::ANY},
                                 duckdb::LogicalType::VARCHAR, LlmFilter::Execute));
}

} // namespace geofer