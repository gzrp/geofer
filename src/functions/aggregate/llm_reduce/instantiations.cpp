#include "geofer/functions/aggregate/llm_reduce.hpp"

namespace geofer {

template void AggregateFunctionBase::Initialize<LlmReduce>(const duckdb::AggregateFunction& function,
                                                           duckdb::data_ptr_t state_p);
template void AggregateFunctionBase::Operation<LlmReduce>(duckdb::Vector[], duckdb::AggregateInputData&, idx_t,
                                                          duckdb::Vector&, idx_t);
template void AggregateFunctionBase::SimpleUpdate<LlmReduce>(duckdb::Vector[], duckdb::AggregateInputData&, idx_t,
                                                             duckdb::data_ptr_t, idx_t);
template void AggregateFunctionBase::Combine<LlmReduce>(duckdb::Vector&, duckdb::Vector&, duckdb::AggregateInputData&,
                                                        idx_t);
template void LlmReduce::Finalize<AggregateFunctionType::REDUCE>(duckdb::Vector&, duckdb::AggregateInputData&,
                                                                 duckdb::Vector&, idx_t, idx_t);
template void LlmReduce::Finalize<AggregateFunctionType::REDUCE_JSON>(duckdb::Vector&, duckdb::AggregateInputData&,
                                                                      duckdb::Vector&, idx_t, idx_t);

} // namespace geofer