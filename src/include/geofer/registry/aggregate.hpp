#pragma once

#include "geofer/core/common.hpp"

namespace geofer {

// 聚合行数统一注册入口
class AggregateRegistry {

public:
    static void Register(duckdb::DatabaseInstance& db);

private:
    static void RegisterLlmFirst(duckdb::DatabaseInstance& db);
    static void RegisterLlmLast(duckdb::DatabaseInstance& db);
    static void RegisterLlmRerank(duckdb::DatabaseInstance& db);
    static void RegisterLlmReduce(duckdb::DatabaseInstance& db);
    static void RegisterLlmReduceJson(duckdb::DatabaseInstance& db);
};

} // namespace geofer