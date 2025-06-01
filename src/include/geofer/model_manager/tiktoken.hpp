#pragma once

#include "geofer/core/common.hpp"

namespace geofer {

class Tiktoken {
public:
    static int GetNumTokens(const std::string& str);
};

} // namespace geofer