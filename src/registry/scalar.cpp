#include "geofer/registry/scalar.hpp"

namespace geofer {

// Register 方法实现，注册所有的私有函数
void ScalarRegistry::Register(duckdb::DatabaseInstance& db) {
    RegisterLlmEmbedding(db);
}

} // namespace geofer