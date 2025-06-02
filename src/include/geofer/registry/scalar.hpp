#pragma once

#include "geofer/core/common.hpp"

namespace geofer {

// 标量函数统一注册入口
class ScalarRegistry {
public:
    static void Register(duckdb::DatabaseInstance& db);

private:
    static void RegisterLlmCompleteJson(duckdb::DatabaseInstance& db);
    static void RegisterLlmComplete(duckdb::DatabaseInstance& db);
    static void RegisterLlmEmbedding(duckdb::DatabaseInstance& db);
    static void RegisterLlmFilter(duckdb::DatabaseInstance& db);
    static void RegisterFusionRRF(duckdb::DatabaseInstance& db);
    static void RegisterFusionCombANZ(duckdb::DatabaseInstance& db);
    static void RegisterFusionCombMED(duckdb::DatabaseInstance& db);
    static void RegisterFusionCombMNZ(duckdb::DatabaseInstance& db);
    static void RegisterFusionCombSUM(duckdb::DatabaseInstance& db);
    static void RegisterQuack(duckdb::DatabaseInstance& db);
};

} // namespace geofer