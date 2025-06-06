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

static std::string PostToVisionAPI(const char* image_data, duckdb::idx_t image_size, const std::string &description) {

	try {
		// 构造 multipart 请求体
		cpr::Multipart multipart {
			{"description", description},
			{"file", cpr::Buffer{image_data, image_data + image_size, "image.jpg"}},
		};

		// 发送请求
		auto response = cpr::Post(
			cpr::Url{"http://192.168.56.1:8080/api/v1/image_analyze"},
        	multipart,
        	cpr::Timeout{5000}
		);

		// 基本错误检查
    	if (response.error) {
        	throw std::runtime_error("HTTP error: " + response.error.message);
    	}
    	if (response.status_code != 200) {
        	throw std::runtime_error("HTTP " + std::to_string(response.status_code) + ": " + response.text);
    	}
		// 解析 json 结果

		auto j = nlohmann::json::parse(response.text);
        // std::stringstream ss;
        // ss << "sha256: " << j["sha256"] << ", size: " << j["file_size"]
        //   << ", objects: [";

        // for (const auto& obj : j["objects"]) {
        //    ss << obj["label"].get<std::string>() << "("
        //       << obj["confidence"].get<float>() << "), ";
        //}
        //ss << "]";

        //return ss.str();
		return j.dump();
	} catch (const std::exception &e) {
        throw std::runtime_error(std::string("Exception during POST: ") + e.what());
    }
}

std::string RunSpatialQuery() {
	auto con = Config::GetConnection();
	auto result = con.Query(duckdb_fmt::format(" SELECT h.name AS haoke_name, h.address AS haoke_address, b.name AS bank_name, b.address AS bank_address, round((st_distance(h.location, b.location) / 0.0111) * 1000) AS distance "
        									   " FROM haoke AS h, bank AS b "
										       " WHERE ST_X(h.location) <= ST_X(b.location) "
										       " ORDER BY distance "
										       " LIMIT 5; "));
	nlohmann::json j_rows = nlohmann::json::array();
	// 获取列名列表
	const auto &names = result->names;
	// 遍历每一行
	for (duckdb::idx_t row_idx = 0; row_idx < result->RowCount(); ++row_idx) {
    	nlohmann::json row_json;
   	 	for (duckdb::idx_t col_idx = 0; col_idx < result->ColumnCount(); ++col_idx) {
        	const auto &val = result->GetValue(col_idx, row_idx);
        	if (val.IsNull()) {
            	row_json[names[col_idx]] = nullptr;  // 显式表示 NULL
        	} else {
            	row_json[names[col_idx]] = val.ToString();  // 或使用其他类型解析
        	}
    	}
    	j_rows.push_back(row_json);
	}
    return j_rows.dump();
}

// 逻辑实现，如有必要调用大模型
std::vector<std::string> ImageAnalyze::Operation(duckdb::DataChunk& args) {
	ValidateArguments(args);
    idx_t rows = args.size();
	std::vector<std::string> results;
    results.reserve(args.size());

 	auto &blob_vector = args.data[0];
    auto &desc_vector = args.data[1];
	 // 转成统一格式
    duckdb::UnifiedVectorFormat blob_format;
    duckdb::UnifiedVectorFormat desc_format;
    blob_vector.ToUnifiedFormat(rows, blob_format);
    desc_vector.ToUnifiedFormat(rows, desc_format);

    auto blob_data = reinterpret_cast<duckdb::string_t *>(blob_format.data);
    auto desc_data = reinterpret_cast<duckdb::string_t *>(desc_format.data);

	for (idx_t row = 0; row < rows; row++) {
		if (!blob_format.validity.RowIsValid(row)) {
            results.emplace_back("NULL INPUT");
            continue;
        }
		auto blob_idx = blob_format.sel->get_index(row);
        auto desc_idx = desc_format.sel->get_index(row);

        const auto &blob_str = blob_data[blob_idx];
        const auto &desc_str = desc_data[desc_idx];

        const char *blob_ptr = blob_str.GetData();
        duckdb::idx_t blob_size = blob_str.GetSize();
        std::string desc = desc_str.GetString();

		 try {
            std::string vision_json_str = PostToVisionAPI(blob_ptr, blob_size, desc);
 			auto vision_json = nlohmann::json::parse(vision_json_str);
            bool has_haoke = false, has_bank = false;
            for (const auto& obj : vision_json["objects"]) {
                const std::string label = obj["label"].get<std::string>();
                if (label.find("好客连锁") != std::string::npos) has_haoke = true;
                if (label.find("银行") != std::string::npos) has_bank = true;
            }

 			nlohmann::json final_json;
            final_json["vision"] = vision_json;
            if (has_haoke && has_bank) {
                auto spatial_json = nlohmann::json::parse(RunSpatialQuery());
                final_json["spatial"] = spatial_json;
            } else {
				final_json["spatial"] = nlohmann::json::array(); // 空数组
            }
			results.emplace_back(final_json.dump());
        } catch (const std::exception &ex) {
            results.emplace_back(std::string("Error: ") + ex.what());
        }
	}
	return results;
}

void ImageAnalyze::Execute(duckdb::DataChunk& args, duckdb::ExpressionState& state, duckdb::Vector& result) {

    const auto responses = ImageAnalyze::Operation(args);

    duckdb::idx_t pos = 0;
    for (const auto &res : responses) {
        result.SetValue(pos++, duckdb::Value(res));
    }
}

} // namespace geofer