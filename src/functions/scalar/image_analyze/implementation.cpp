#include "geofer/functions/scalar/image_analyze.hpp"
#include <cpr/cpr.h>

namespace geofer {

// 参数校验
void ImageAnalyze::ValidateArguments(duckdb::DataChunk& args) {
    if (args.ColumnCount() != 2) {
        throw std::runtime_error("analyze_image expects 2 arguments: image_blob, image_desc.");
    }
	const auto &blob_col = args.data[0];
    const auto &desc_col = args.data[1];
	if (blob_col.GetType().id() != duckdb::LogicalTypeId::BLOB) {
 		throw std::runtime_error("First argument to analyze_image must be a BLOB containing raw image bytes");
    }
    if (desc_col.GetType().id() != duckdb::LogicalTypeId::VARCHAR) {
 		throw std::runtime_error("Second argument to analyze_image must be VARCHAR (an image description)");
    }
}

static std::string PostToVisionAPI(const std::vector<uint8_t> &blob) {
    cpr::Buffer buffer(blob.begin(), blob.end(), "upload.jpg");

    cpr::Part file_part{ "file", buffer, "image/jpeg" };

    // 3. 把 cpr::Part 放到一个 vector 里，再用这个 vector 构造 Multipart
    std::vector<cpr::Part> parts;
    parts.push_back(file_part);

    // 如果有其它字段，例如 user_id、description，也可以像下面这样加入：
    // parts.push_back(cpr::Part{ "user_id", "12345" });
    // parts.push_back(cpr::Part{ "description", "内存中的图片上传示例" });

    cpr::Multipart form(parts);
    auto resp = cpr::Post(
        cpr::Url{"http://127.0.0.1:8080/api/v1/image_analyze"},
        form,
        cpr::Timeout{5000}
    );

    // 基本错误检查
    if (resp.error) {
        throw std::runtime_error("HTTP error: " + resp.error.message);
    }
    if (resp.status_code != 200) {
        throw std::runtime_error("HTTP " + std::to_string(resp.status_code) + ": " + resp.text);
    }
    // JSON 校验
    try {
        auto json_resp = nlohmann::json::parse(resp.text);
        return json_resp.dump();
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Invalid JSON returned: ") + e.what());
    }
}


// 逻辑实现，如有必要调用大模型
std::vector<std::string> ImageAnalyze::Operation(duckdb::DataChunk& args) {
	ValidateArguments(args);
    idx_t rows = args.size();
	std::vector<std::string> results;
    results.reserve(args.size());

 	auto &blob_vec = args.data[0];
    auto &desc_vec = args.data[1];
	auto &blob_validity = duckdb::FlatVector::Validity(blob_vec);
    auto &desc_validity = duckdb::FlatVector::Validity(desc_vec);

	for (idx_t i = 0; i < rows; i++) {
		if (!blob_validity.RowIsValid(i)) {
			results.emplace_back("NULL INPUT");
			continue;
		}
        // 获取 BLOB 内容
        duckdb::Value blob_value = blob_vec.GetValue(i);
        duckdb::string_t blob_string = duckdb::StringValue::Get(blob_value);

        const char *blob_data = blob_string.GetData();
        duckdb::idx_t blob_size = blob_string.GetSize();

        std::string desc = desc_vec.GetValue(i).ToString();
		// 模拟分析其
		std::stringstream ss;
		ss << "Analyzed image of size: " << blob_size
           << " bytes with description: '" << desc << "'";
		results.emplace_back(ss.str());
	}

	return results;

}

void ImageAnalyze::Execute(duckdb::DataChunk& args, duckdb::ExpressionState& state, duckdb::Vector& result) {

	const auto responses = Operation(args);

    duckdb::idx_t pos = 0;
    for (auto &res : responses) {
		if (res == "NULL INPUT") {
			result.SetValue(pos++, duckdb::Value()); // 设置为空之
		} else {
			result.SetValue(pos++, duckdb::Value(res));
		}
    }
}

} // namespace geofer