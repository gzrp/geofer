#include "geofer/functions/aggregate/llm_rerank.hpp"
#include "geofer/registry/registry.hpp"

namespace geofer {

void AggregateRegistry::RegisterLlmRerank(duckdb::DatabaseInstance& db) {
    auto string_concat = duckdb::AggregateFunction(
        "llm_rerank", {duckdb::LogicalType::ANY, duckdb::LogicalType::ANY, duckdb::LogicalType::ANY},
        duckdb::LogicalType::VARCHAR, duckdb::AggregateFunction::StateSize<AggregateFunctionState>,
        LlmRerank::Initialize, LlmRerank::Operation, LlmRerank::Combine, LlmRerank::Finalize, LlmRerank::SimpleUpdate);

    duckdb::ExtensionUtil::RegisterFunction(db, string_concat);
}

} // namespace geofer