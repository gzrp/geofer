#pragma once

#include "geofer/functions/scalar/scalar.hpp"

namespace geofer {

/**
 * Performs the CombANZ algorithm to merge lists based multiple scoring sources,
 * as proposed by Fox et al. It averages the normalized scores for each entry (when found by the scoring system).
 * Good for balancing contributions from multiple sources without being too heavily influenced by one scoring system.
 * Reference: Combination of Multiple Searches. Edward A. Fox and Joseph A. Shaw. NIST, 1993.
 */
class FusionCombANZ : public ScalarFunctionBase {
public:
    static std::vector<double> Operation(duckdb::DataChunk& args);
    static void Execute(duckdb::DataChunk& args, duckdb::ExpressionState& state, duckdb::Vector& result);
};

} // namespace geofer