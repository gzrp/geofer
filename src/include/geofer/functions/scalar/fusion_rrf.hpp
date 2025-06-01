#pragma once

#include "geofer/functions/scalar/scalar.hpp"

namespace geofer {

/**
 * Performs Reciprocal Rank Fusion (RRF) to merge lists based multiple scoring sources,
 * as proposed by Cormack et al. in https://doi.org/10.1145/1571941.1572114.
 * They established that an rrf_constant of 60 was "near-optimal, but that the choice was not critical"
 */
class FusionRRF : public ScalarFunctionBase {
public:
    /**
     * Implements the RRF logic
     * @param args Provided by core DuckDB when the function is executed in the CLI. Contains the arguments
     * @return A vector of rankings for the entries, which will be processed by FusionRRF::Execute
     */
    static std::vector<double> Operation(duckdb::DataChunk& args);
    static void Execute(duckdb::DataChunk& args, duckdb::ExpressionState& state, duckdb::Vector& result);
};

} // namespace geofer