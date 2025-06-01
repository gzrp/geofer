#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <utility>
#include <nlohmann/json.hpp>
#include "fmt/format.h"

#include "geofer/core/config.hpp"
#include "geofer/model_manager/repository.hpp"
#include "geofer/model_manager/providers/adapters/openai.hpp"
#include "geofer/model_manager/providers/adapters/azure.hpp"
#include "geofer/model_manager/providers/adapters/ollama.hpp"
#include "geofer/model_manager/providers/handlers/ollama.hpp"
#include "duckdb/main/connection.hpp"

namespace geofer {

class Model {
public:
    explicit Model(const nlohmann::json& model_json);
    explicit Model() = default;
    nlohmann::json CallComplete(const std::string& prompt, const bool json_response = true);
    nlohmann::json CallEmbedding(const std::vector<std::string>& inputs);
    ModelDetails GetModelDetails();

private:
    std::shared_ptr<IProvider> provider_;
    ModelDetails model_details_;
    void ConstructProvider();
    void LoadModelDetails(const nlohmann::json& model_json);
    std::tuple<std::string, std::string, int32_t, int32_t> GetQueriedModel(const std::string& model_name);
    std::string GetSecret(const std::string& secret_name);
};

} // namespace geofer