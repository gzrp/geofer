#include "geofer/functions/batch_response_builder.hpp"

namespace geofer {

std::vector<nlohmann::json> CastVectorOfStructsToJson(const duckdb::Vector& struct_vector, const int size) {
    std::vector<nlohmann::json> vector_json;
    for (auto i = 0; i < size; i++) {
        nlohmann::json json;
        for (auto j = 0; j < static_cast<int>(duckdb::StructType::GetChildCount(struct_vector.GetType())); j++) {
            const auto key = duckdb::StructType::GetChildName(struct_vector.GetType(), j);
            auto value = duckdb::StructValue::GetChildren(struct_vector.GetValue(i))[j].ToString();
            json[key] = value;
        }
        vector_json.push_back(json);
    }
    return vector_json;
}

} // namespace geofer