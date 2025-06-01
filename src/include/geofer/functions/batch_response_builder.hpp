#pragma once
#include <nlohmann/json.hpp>
#include "geofer/core/common.hpp"
#include "geofer/model_manager/model.hpp"
#include "geofer/prompt_manager/prompt_manager.hpp"
#include "geofer/model_manager/tiktoken.hpp"

namespace geofer {

std::vector<nlohmann::json> CastVectorOfStructsToJson(const duckdb::Vector& struct_vector, int size);

} // namespace geofer