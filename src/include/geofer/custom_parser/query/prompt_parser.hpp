#pragma once

#include "geofer/core/common.hpp"
#include "geofer/custom_parser/query_statements.hpp"
#include "geofer/custom_parser/tokenizer.hpp"

#include "fmt/format.h"
#include <memory>
#include <string>
#include <vector>

namespace geofer {

// 创建 prompt 语句
class CreatePromptStatement : public QueryStatement {
public:
    CreatePromptStatement() { type = StatementType::CREATE_PROMPT; };
    std::string catalog;
    std::string prompt_name;
    std::string prompt;
};

class DeletePromptStatement : public QueryStatement {
public:
    DeletePromptStatement() { type = StatementType::DELETE_PROMPT; };
    std::string prompt_name;
};

class UpdatePromptScopeStatement : public QueryStatement {
public:
    UpdatePromptScopeStatement() { type = StatementType::UPDATE_PROMPT_SCOPE; };
    std::string catalog;
    std::string prompt_name;
};

class UpdatePromptStatement : public QueryStatement {
public:
    UpdatePromptStatement() { type = StatementType::UPDATE_PROMPT; };
    std::string prompt_name;
    std::string new_prompt;
};

class GetPromptStatement : public QueryStatement {
public:
    GetPromptStatement() { type = StatementType::GET_PROMPT; };
    std::string prompt_name;
};

class GetAllPromptStatement : public QueryStatement {
public:
    GetAllPromptStatement() { type = StatementType::GET_ALL_PROMPT; };
};

class PromptParser {
public:
    void Parse(const std::string& query, std::unique_ptr<QueryStatement>& query_statement);
    std::string ToSQL(const QueryStatement& statement) const;

private:
    void ParseCreatePrompt(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
    void ParseDeletePrompt(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
    void ParseUpdatePrompt(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
    void ParseGetPrompt(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement);
};

} // namespace geofer
