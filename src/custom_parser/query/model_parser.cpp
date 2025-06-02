#include "geofer/custom_parser/query/model_parser.hpp"
#include "geofer/core/common.hpp"
#include "geofer/core/config.hpp"
#include <sstream>
#include <stdexcept>

namespace geofer {

void ModelParser::Parse(const std::string& query, std::unique_ptr<QueryStatement>& statement) {
    Tokenizer tokenizer(query);
    auto token = tokenizer.NextToken();
    const auto value = duckdb::StringUtil::Upper(token.value);

    if (token.type == TokenType::KEYWORD) {
        if (value == "CREATE") {
            ParseCreateModel(tokenizer, statement);
        } else if (value == "DELETE") {
            ParseDeleteModel(tokenizer, statement);
        } else if (value == "UPDATE") {
            ParseUpdateModel(tokenizer, statement);
        } else if (value == "GET") {
            ParseGetModel(tokenizer, statement);
        } else {
            throw std::runtime_error("Unknown keyword: " + token.value);
        }
    } else {
        throw std::runtime_error("Excepect a keyword at the beginning of the query.");
    }
}

void ModelParser::ParseCreateModel(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement) {
    auto token = tokenizer.NextToken();
    auto value = duckdb::StringUtil::Upper(token.value);
    // CREATE [GLOABL|LOCAL] MODEL ( <model_name>,<model>, <provider_name>, <model_args_json>)
    std::string catalog;
    if (token.type == TokenType::KEYWORD && (value == "GLOBAL" || value == "LOCAL")) {
        catalog = value == "GLOBAL" ? "geofer_storage." : "";
        token = tokenizer.NextToken();
        value = duckdb::StringUtil::Upper(token.value);
    }
    if (token.type != TokenType::KEYWORD || value != "MODEL") {
        throw std::runtime_error("Expected 'MODEL' after 'CREATE'.");
    }
    token = tokenizer.NextToken();
    if (token.type != TokenType::PARENTHESIS || token.value != "(") {
        throw std::runtime_error("Expected opening parenthesis '(' after 'MODEL'.");
    }
    token = tokenizer.NextToken();
    if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
        throw std::runtime_error("Expected non-empty string literal for model name.");
    }
    auto model_name = token.value;

    token = tokenizer.NextToken();
    if (token.type != TokenType::SYMBOL || token.value != ",") {
        throw std::runtime_error("Expected comma ',' after model name.");
    }
    token = tokenizer.NextToken();
    if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
        throw std::runtime_error("Expected non-empty string literal for model.");
    }
    auto model = token.value;

    token = tokenizer.NextToken();
    if (token.type != TokenType::SYMBOL || token.value != ",") {
        throw std::runtime_error("Expected comma ',' after model.");
    }

    token = tokenizer.NextToken();
    if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
        throw std::runtime_error("Expected non-empty string literal for provider_name.");
    }
    std::string provider_name = token.value;

    token = tokenizer.NextToken();
    if (token.type != TokenType::SYMBOL || token.value != ",") {
        throw std::runtime_error("Expected comma ',' after provider_name.");
    }

    token = tokenizer.NextToken();
    if (token.type != TokenType::JSON || token.value.empty()) {
        throw std::runtime_error("Expected json value for the model_args.");
    }
    auto model_args = nlohmann::json::parse(token.value);
    const std::set<std::string> expected_keys = {"context_window", "max_output_tokens"};
    std::set<std::string> json_keys;
    for (auto it = model_args.begin(); it != model_args.end(); ++it) {
        json_keys.insert(it.key());
    }
    if (json_keys != expected_keys) {
        throw std::runtime_error("Expected keys: context_window, max_output_tokens in model_args.");
    }
    token = tokenizer.NextToken();
    if (token.type != TokenType::PARENTHESIS || token.value != ")") {
        throw std::runtime_error("Expected closing parenthesis ')' after max_output_tokens.");
    }
    token = tokenizer.NextToken();
    if (token.type == TokenType::END_OF_FILE) {
        auto create_statement = std::make_unique<CreateModelStatement>();
        create_statement->catalog = catalog;
        create_statement->model_name = model_name;
        create_statement->model = model;
        create_statement->provider_name = provider_name;
        create_statement->model_args = model_args;
        statement = std::move(create_statement);
    } else {
        throw std::runtime_error("Unexpected characters after the closing parenthesis. Only a semicolon is allowed.");
    }
}

void ModelParser::ParseDeleteModel(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement) {
    auto token = tokenizer.NextToken();
    auto value = duckdb::StringUtil::Upper(token.value);
    // DELETE MODEL <model_name>[;|,]
    if (token.type != TokenType::KEYWORD || value != "MODEL") {
        throw std::runtime_error("Unknown keyword: " + token.value);
    }

    token = tokenizer.NextToken();
    if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
        throw std::runtime_error("Expected non-empty string literal for model name.");
    }
    auto model_name = token.value;

    token = tokenizer.NextToken();
    if (token.type == TokenType::SYMBOL || token.value == ";") {
        auto delete_statement = std::make_unique<DeleteModelStatement>();
        delete_statement->model_name = model_name;
        statement = std::move(delete_statement);
    } else {
        throw std::runtime_error("Unexpected characters after the closing parenthesis. Only a semicolon is allowed.");
    }
}

void ModelParser::ParseUpdateModel(Tokenizer& tokenizer, std::unique_ptr<QueryStatement>& statement) {
    auto token = tokenizer.NextToken();
    auto value = duckdb::StringUtil::Upper(token.value);
    // UPDATE MODEL <model_name> TO [GLOABL|LOCAL];
    // UPDATE MODEL ( <model_name>, <new_model>, <provider_name>, <model_args_json>)
    if (token.type != TokenType::KEYWORD || value != "MODEL") {
        throw std::runtime_error("Expected 'MODEL' after 'UPDATE'.");
    }
    token = tokenizer.NextToken();
    if (token.type == TokenType::STRING_LITERAL) {
        auto model_name = token.value;
        token = tokenizer.NextToken();
        if (token.type != TokenType::KEYWORD || duckdb::StringUtil::Upper(token.value) != "TO") {
            throw std::runtime_error("Expect 'TO' after model name.");
        }
        token = tokenizer.NextToken();
        value = duckdb::StringUtil::Upper(token.value);
        if (token.type != TokenType::KEYWORD || (value != "GLOBAL" && value != "LOCAL")) {
            throw std::runtime_error("Expected 'GLOBAL' or 'LOCAL' after 'TO'.");
        }
        auto catalog = value == "GLOBAL" ? "geofer_storage." : "";

        token = tokenizer.NextToken();
        if (token.type == TokenType::SYMBOL || token.value == ";") {
            auto update_statement = std::make_unique<UpdateModelScopeStatement>();
            update_statement->model_name = model_name;
            update_statement->catalog = catalog;
            statement = std::move(update_statement);
        } else {
            throw std::runtime_error("Unexpected characters after the closing parenthesis. Only a semicolon is allowed.");
        }
    } else {
        if (token.type != TokenType::PARENTHESIS || token.value != "(") {
            throw std::runtime_error("Expected opening parenthesis '(' after 'MODEL'.");
        }
        token = tokenizer.NextToken();
        if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
            throw std::runtime_error("Expected non-empty string literal for model name.");
        }
        auto model_name = token.value;
        token = tokenizer.NextToken();
        if (token.type != TokenType::SYMBOL || token.value != ",") {
            throw std::runtime_error("Expected comma ',' after model name.");
        }
        token = tokenizer.NextToken();
        if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
            throw std::runtime_error("Expected non-empty string literal for model.");
        }
        auto new_model = token.value;

        token = tokenizer.NextToken();
        if (token.type != TokenType::SYMBOL || token.value != ",") {
            throw std::runtime_error("Expected comma ',' after model.");
        }

        token = tokenizer.NextToken();
        if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
            throw std::runtime_error("Expected non-empty string literal for provider_name.");
        }
        auto provider_name = token.value;

        token = tokenizer.NextToken();
        if (token.type != TokenType::SYMBOL || token.value != ",") {
            throw std::runtime_error("Expected comma ',' after provider_name.");
        }
        token = tokenizer.NextToken();
        if (token.type != TokenType::JSON || token.value.empty()) {
            throw std::runtime_error("Expected json value for the model_args.");
        }
        auto new_model_args = nlohmann::json::parse(token.value);
        const std::set<std::string> expected_keys = {"context_window", "max_output_tokens"};
        std::set<std::string> json_keys;
        for (auto it = new_model_args.begin(); it != new_model_args.end(); ++it) {
            json_keys.insert(it.key());
        }
        if (json_keys != expected_keys) {
            throw std::runtime_error("Expected keys: context_window, max_output_tokens in model_args.");
        }
        token = tokenizer.NextToken();
        if (token.type != TokenType::PARENTHESIS || token.value != ")") {
            throw std::runtime_error("Expected closing parenthesis ')' after new max_output_tokens.");
        }

        token = tokenizer.NextToken();
        if (token.type == TokenType::END_OF_FILE) {
            auto update_statement = std::make_unique<UpdateModelStatement>();
            update_statement->new_model = new_model;
            update_statement->model_name = model_name;
            update_statement->provider_name = provider_name;
            update_statement->new_model_args = new_model_args;
            statement = std::move(update_statement);
        } else {
            throw std::runtime_error("Unexpected characters after the closing parenthesis. Only a semicolon is allowed.");
        }
    }
}

void ModelParser::ParseGetModel(Tokenizer &tokenizer, std::unique_ptr<QueryStatement>& statement) {
    auto token = tokenizer.NextToken();
    auto value = duckdb::StringUtil::Upper(token.value);
    // GET MODEL|MODELS;
    // GET MODEL <model_name>;
    if (token.type != TokenType::KEYWORD || (value != "MODEL" && value != "MODELS")) {
        throw std::runtime_error("Expected 'MODEL' after 'GET'.");
    }
    token = tokenizer.NextToken();
    if (token.type == TokenType::SYMBOL || token.value == ";") {
        auto get_all_statement = std::make_unique<GetAllModelStatement>();
        statement = std::move(get_all_statement);
    } else {
        if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
            throw std::runtime_error("Expected non-empty string literal for model name.");
        }
        auto model_name = token.value;

        token = tokenizer.NextToken();
        if (token.type == TokenType::SYMBOL || token.value == ";") {
            auto get_statement = std::make_unique<GetModelStatement>();
            get_statement->model_name = model_name;
            statement = std::move(get_statement);
        } else {
            throw std::runtime_error("Unexpected characters after the model name. Only a semicolon is allowed.");
        }
    }
}

std::string ModelParser::ToSQL(const QueryStatement& statement) const {
    std::string query;
    switch (statement.type) {
    case StatementType::CREATE_MODEL: {
        const auto& create_stmt = static_cast<const CreateModelStatement&>(statement);
        auto con = Config::GetConnection();
        auto result = con.Query(duckdb_fmt::format(
            " SELECT model_name "
            " FROM geofer_storage.geofer_config.GEOFER_MODEL_DEFAULT_INTERNAL_TABLE "
            " WHERE model_name = '{}' "
            " UNION ALL "
            " SELECT model_name "
            " FROM {}geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
            " WHERE model_name = '{}';",
            create_stmt.model_name, create_stmt.catalog.empty() ? "" : "geofer_storage.", create_stmt.model_name));
        if (result->RowCount() != 0) {
            throw std::runtime_error(duckdb_fmt::format("Model '{}' already exist.", create_stmt.model_name));
        }

        query = duckdb_fmt::format(" INSERT INTO "
                                 " {}geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
                                 " (model_name, model, provider_name, model_args) "
                                 " VALUES ('{}', '{}', '{}', '{}');",
                                 create_stmt.catalog, create_stmt.model_name, create_stmt.model,
                                 create_stmt.provider_name, create_stmt.model_args.dump());
        break;
    }
    case StatementType::DELETE_MODEL: {
        const auto& delete_stmt = static_cast<const DeleteModelStatement&>(statement);
        auto con = Config::GetConnection();

        con.Query(duckdb_fmt::format(" DELETE FROM geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
                                     " WHERE model_name = '{}';",
                                     delete_stmt.model_name));

        query = duckdb_fmt::format(" DELETE FROM "
                                   " geofer_storage.geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
                                   " WHERE model_name = '{}';",
                                   delete_stmt.model_name);
        break;
    }
	case StatementType::UPDATE_MODEL: {
		const auto& update_stmt = static_cast<const UpdateModelStatement&>(statement);
		auto con = Config::GetConnection();
		// 获取模型
		auto result = con.Query(
			duckdb_fmt::format( " SELECT model_name, 'global' AS scope "
								" FROM geofer_storage.geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
								" WHERE model_name = '{}' "
								" UNION ALL "
								" SELECT model_name, 'local' AS	scope "
								" FROM geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
								" Where model_name = '{}';",
								update_stmt.model_name, update_stmt.model_name));
		if (result->RowCount() == 0) {
			throw std::runtime_error(duckdb_fmt::format("Model '{}' does not exist.", update_stmt.model_name));
		}
		auto catalog = result->GetValue(1, 0).ToString() == "global" ? "geofer_storage." : "";

		query = duckdb_fmt::format(" UPDATE {}geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
								   " SET model = '{}', provider_name = '{}', "
								   " model_args = '{}' WHERE model_name = '{}'; ",
								   catalog, update_stmt.new_model, update_stmt.provider_name,
								   update_stmt.new_model_args.dump(), update_stmt.model_name);
		break;
	}

	case StatementType::UPDATE_MODEL_SCOPE: {
        const auto& update_stmt = static_cast<const UpdateModelScopeStatement&>(statement);
        auto con = Config::GetConnection();
        auto result = con.Query(duckdb_fmt::format(" SELECT model_name "
                                         		   " FROM {}geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
                                                   " WHERE model_name = '{}';",
                                                   update_stmt.catalog, update_stmt.model_name));
        if (result->RowCount() != 0) {
            throw std::runtime_error(duckdb_fmt::format("Model '{}' already exist in {} storage.",
									 update_stmt.model_name,
                                     update_stmt.catalog == "geofer_storage." ? "global" : "local"));
        }

        con.Query(duckdb_fmt::format(" INSERT INTO {}geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
                                     " (model_name, model, provider_name, model_args) "
                                     " SELECT model_name, model, provider_name, model_args "
                                     " FROM {}geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
                                     " WHERE model_name = '{}'; ",
                                     update_stmt.catalog,
                                     update_stmt.catalog == "geofer_storage." ? "" : "geofer_storage.",
                                     update_stmt.model_name));

        query = duckdb_fmt::format(" DELETE FROM {}geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
                                   " WHERE model_name = '{}'; ",
                                   update_stmt.catalog == "geofer_storage." ? "" : "geofer_storage.",
                                   update_stmt.model_name);
        break;
    }
	case StatementType::GET_MODEL: {
        const auto& get_stmt = static_cast<const GetModelStatement&>(statement);
        query = duckdb_fmt::format(" SELECT 'global' AS scope, * "
                                   " FROM geofer_storage.geofer_config.GEOFER_MODEL_DEFAULT_INTERNAL_TABLE "
                                   " WHERE model_name = '{}' "
                                   " UNION ALL "
                                   " SELECT 'global' AS scope, * "
                                   " FROM geofer_storage.geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
                                   " WHERE model_name = '{}' "
                                   " UNION ALL "
                                   " SELECT 'local' AS scope, * "
                                   " FROM geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
                                   " WHERE model_name = '{}';",
                                   get_stmt.model_name, get_stmt.model_name, get_stmt.model_name);
        break;
    }
 	case StatementType::GET_ALL_MODEL: {
        query = duckdb_fmt::format(" SELECT 'global' AS scope, * "
                                   " FROM geofer_storage.geofer_config.GEOFER_MODEL_DEFAULT_INTERNAL_TABLE "
                                   " UNION ALL "
                                   " SELECT 'global' AS scope, * "
                                   " FROM geofer_storage.geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE "
                                   " UNION ALL "
                                   " SELECT 'local' AS scope, * "
                                   " FROM geofer_config.GEOFER_MODEL_USER_DEFINED_INTERNAL_TABLE;"
                               	   );
        break;
    }
	default:
        throw std::runtime_error("Unknown statement type.");
    }

    return query;
}

} // namespace geofer