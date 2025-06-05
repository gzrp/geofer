#pragma once

#include "geofer/functions/scalar/scalar.hpp"

namespace geofer {

class ImageAnalyze : public ScalarFunctionBase {
public:
    static void ValidateArguments(duckdb::DataChunk& args);
    static std::vector<std::string> Operation(ducikdb::DataChunk& args);
    static void Execute(duckdb::DataChunk& args, duckdb::ExpressionState& state, duckdb::Vector& result);
};

} // namespace geofer