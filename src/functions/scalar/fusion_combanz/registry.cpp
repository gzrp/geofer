#include "geofer/functions/scalar/fusion_combanz.hpp"
#include "geofer/registry/registry.hpp"

namespace geofer {

void ScalarRegistry::RegisterFusionCombANZ(duckdb::DatabaseInstance& db) {
  duckdb::ExtensionUtil::RegisterFunction(
      db, duckdb::ScalarFunction("fusion_combanz", {}, duckdb::LogicalType::VARCHAR, FusionCombANZ::Execute, nullptr,
                                 nullptr, nullptr, nullptr, duckdb::LogicalType::ANY,
                                 duckdb::FunctionStability::VOLATILE, duckdb::FunctionNullHandling::SPECIAL_HANDLING));
}

} // namespace geofer