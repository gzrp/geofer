#pragma once

#include "geofer/core/common.hpp"
#include "geofer/registry/aggregate.hpp"
#include "geofer/registry/scalar.hpp"

namespace geofer {

// 注册函数统一入口
class Registry {
public:
    static void Register(duckdb::DatabaseInstance& db);

private:
    static void RegisterAggregateFunctions(duckdb::DatabaseInstance& db);
    static void RegisterScalarFunctions(duckdb::DatabaseInstance& db);
};

} // namesapce geofer


