#include "geofer/registry/aggregate.hpp"

namespace geofer {

// Register 方法实现，注册所有的私有函数
void AggregateRegistry::Register(duckdb::DatabaseInstance& db) {
    RegisterLlmFirst(db);
    RegisterLlmLast(db);
    RegisterLlmRerank(db);
    RegisterLlmReduce(db);
    RegisterLlmReduceJson(db);
}

} // namespace geofer
