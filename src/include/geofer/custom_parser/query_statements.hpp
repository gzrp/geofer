#pragma once

#include "geofer/core/common.hpp"

#include <memory>
#include <string>
#include <vector>

namespace geofer {

// 语句类型
enum class StatementType {
    CREATE_MODEL,
    DELETE_MODEL,
    UPDATE_MODEL,
    UPDATE_MODEL_SCOPE,
    GET_MODEL,
    GET_ALL_MODEL,
    CREATE_PROMPT,
    DELETE_PROMPT,
    UPDATE_PROMPT,
    UPDATE_PROMPT_SCOPE,
    GET_PROMPT,
    GET_ALL_PROMPT,
};

// 语句抽象基类
class QueryStatement {
public:
    virtual ~QueryStatement() = default;
    StatementType type;
};

// CREATE DUCK
class CreateDuckStatement: public QueryStatement {
public:
    std::string duck_string;
};

} // namespace geofer
