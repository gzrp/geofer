#pragma once

#include "geofer/functions/aggregate/aggregate.hpp"

namespace geofer {

class LlmRerank : public AggregateFunctionBase {
public:
  explicit LlmRerank() = default;

  int GetAvailableTokens();
  nlohmann::json SlidingWindow(nlohmann::json& tuples);
  std::vector<int> RerankBatch(const nlohmann::json& tuples);

  static void Initialize(const duckdb::AggregateFunction& function, duckdb::data_ptr_t state_p) {
    AggregateFunctionBase::Initialize<LlmRerank>(function, state_p);
  }
  static void Operation(duckdb::Vector inputs[], duckdb::AggregateInputData& aggr_input_data, idx_t input_count,
                        duckdb::Vector& states, idx_t count) {
    AggregateFunctionBase::Operation<LlmRerank>(inputs, aggr_input_data, input_count, states, count);
  }
  static void SimpleUpdate(duckdb::Vector inputs[], duckdb::AggregateInputData& aggr_input_data, idx_t input_count,
                           duckdb::data_ptr_t state_p, idx_t count) {
    AggregateFunctionBase::SimpleUpdate<LlmRerank>(inputs, aggr_input_data, input_count, state_p, count);
  }
  static void Combine(duckdb::Vector& source, duckdb::Vector& target, duckdb::AggregateInputData& aggr_input_data,
                      idx_t count) {
    AggregateFunctionBase::Combine<LlmRerank>(source, target, aggr_input_data, count);
  }
  static void Finalize(duckdb::Vector& states, duckdb::AggregateInputData& aggr_input_data, duckdb::Vector& result,
                       idx_t count, idx_t offset);
};

} // namespace geofer