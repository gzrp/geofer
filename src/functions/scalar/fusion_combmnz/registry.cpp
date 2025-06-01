#include "geofer/functions/scalar/fusion_combmnz.hpp"
#include "geofer/registry/registry.hpp"

namespace geofer {

void ScalarRegistry::RegisterFusionCombMNZ(duckdb::DatabaseInstance& db) {
  duckdb::ExtensionUtil::RegisterFunction(
      db, duckdb::ScalarFunction("fusion_combmnz", {}, duckdb::LogicalType::DOUBLE, FusionCombMNZ::Execute, nullptr,
                                 nullptr, nullptr, nullptr, duckdb::LogicalType::ANY,
                                 duckdb::FunctionStability::VOLATILE, duckdb::FunctionNullHandling::SPECIAL_HANDLING));
}

} // namespace geofer