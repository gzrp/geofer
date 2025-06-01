#include "geofer/functions/scalar/llm_embedding.hpp"
#include "geofer/registry/registry.hpp"

namespace geofer {

void ScalarRegistry::RegisterLlmEmbedding(duckdb::DatabaseInstance& db) {
  duckdb::ExtensionUtil::RegisterFunction(
      db,
      duckdb::ScalarFunction("llm_embedding", {}, duckdb::LogicalType::LIST(duckdb::LogicalType::DOUBLE),
                             LlmEmbedding::Execute, nullptr, nullptr, nullptr, nullptr,
                             duckdb::LogicalType::ANY, duckdb::FunctionStability::VOLATILE));
}

} // namespace geofer