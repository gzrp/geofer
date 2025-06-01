#pragma once

#include <string>

namespace geofer {

// token 类型
enum class TokenType {
    KEYWORD,
    STRING_LITERAL,
    JSON,
    SYMBOL,
    NUMBER,
    PARENTHESIS,
    END_OF_FILE,
    UNKNOWN
};

// Token 结构体
struct Token {
    TokenType type;
    std::string value;
}

// tokenizer
class Tokenizer {
public:
    Tokenizer(const std::string& query);
    Token NextToken();
    std::string GetQuery();

    void SkipWhitespace();
    Token ParseStringLiteral();
    Token ParseKeyword();
    Token ParseSymbol();
    Token ParseNumber();
    Token ParseParenthesis();
    Token ParseJson();
    Token GetNextToken();

private:
    std::string _query;
    int _position;
};

// 工具函数：转化 token 类型到字符串
std::string TokenTypeToString(TokenType type);

} // namespace geofer