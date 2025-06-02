#include "geofer/registry/scalar.hpp"

namespace geofer {

// Register 方法实现，注册所有的私有函数
void ScalarRegistry::Register(duckdb::DatabaseInstance& db) {
    RegisterLlmCompleteJson(db);
    RegisterLlmComplete(db);
    RegisterLlmEmbedding(db);
    RegisterLlmFilter(db);
    RegisterFusionRRF(db);
    RegisterFusionCombANZ(db);
    RegisterFusionCombMED(db);
    RegisterFusionCombMNZ(db);
    RegisterFusionCombSUM(db);
}

} // namespace geofer