#include "geofer/functions/scalar/fusion_combsum.hpp"
#include "geofer/registry/registry.hpp"

namespace geofer {

void ScalarRegistry::RegisterFusionCombSUM(duckdb::DatabaseInstance& db) {
  duckdb::ExtensionUtil::RegisterFunction(
      db, duckdb::ScalarFunction("fusion_combsum", {}, duckdb::LogicalType::DOUBLE, FusionCombSUM::Execute, nullptr,
                                 nullptr, nullptr, nullptr, duckdb::LogicalType::ANY,
                                 duckdb::FunctionStability::VOLATILE, duckdb::FunctionNullHandling::SPECIAL_HANDLING));
}

} // namespace geofer