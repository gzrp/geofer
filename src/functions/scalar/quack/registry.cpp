#include "geofer/functions/scalar/quack.hpp"
#include "geofer/registry/registry.hpp"

namespace geofer {

void ScalarRegistry::RegisterQuack(duckdb::DatabaseInstance& db) {
  duckdb::ExtensionUtil::RegisterFunction(
      db, duckdb::ScalarFunction("quack",
                                 {duckdb::LogicalType::VARCHAR}, duckdb::LogicalType::VARCHAR, Quack::Execute));
}

} // namespace geofer