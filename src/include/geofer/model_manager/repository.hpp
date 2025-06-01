#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace geofer {

struct ModelDetails {
    std::string provider_name;
    std::string model_name;
    std::string model;
    int32_t context_window;
    int32_t max_output_tokens;
    float temperature;
    std::unordered_map<std::string, std::string> secret;
    std::string tuple_format;
    int batch_size;
};

const std::string OLLAMA = "ollama";
const std::string OPENAI = "openai";
const std::string AZURE = "azure";
const std::string DEFAULT_PROVIDER = "default";
const std::string EMPTY_PROVIDER = "";

enum SupportedProviders {
    GEOFER_OPENAI = 0,
    GEOFER_AZURE,
    GEOFER_OLLAMA,
    GEOFER_UNSUPPORTED_PROVIDER,
    GEOFER_SUPPORTED_PROVIDER_COUNT
};

inline SupportedProviders GetProviderType(std::string provider) {
    std::transform(provider.begin(), provider.end(), provider.begin(), [](unsigned char c) { return std::tolower(c); });
    if (provider == OPENAI || provider == DEFAULT_PROVIDER || provider == EMPTY_PROVIDER)
        return GEOFER_OPENAI;
    if (provider == AZURE)
        return GEOFER_AZURE;
    if (provider == OLLAMA)
        return GEOFER_OLLAMA;

    return GEOFER_UNSUPPORTED_PROVIDER;
}

inline std::string GetProviderName(SupportedProviders provider) {
    switch (provider) {
    case GEOFER_OPENAI:
        return OPENAI;
    case GEOFER_AZURE:
        return AZURE;
    case GEOFER_OLLAMA:
        return OLLAMA;
    default:
        return "";
    }
}

} // namespace geofer