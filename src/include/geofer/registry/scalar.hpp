#pragma once

#include "geofer/core/common.hpp"

namespace geofer {

// 标量函数统一注册入口
class ScalarRegistry {
public:
    static void Register(duckdb::DatabaseInstance& db);

private:
    static void RegisterLlmEmbedding(duckdb::DatabaseInstance& db);
};

} // namespace geofer