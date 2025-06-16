#include "geofer/functions/scalar/image_analyze.hpp"
#include <cpr/cpr.h>

namespace geofer {

// 物体检测框
struct ObjectBox {
    std::string label;
    float confidence;
    float x1, y1, x2, y2;
};

// 相对经度位置枚举
enum class RelativeLongitude {
    WEST = -1,
    EAST = 1
};

// 计算两个检查框的相对经度关系，仅判断东西, 返回结果是 a 相较于 b 的 某侧
RelativeLongitude compare_relative_longitude(const ObjectBox& a, const ObjectBox& b, const ObjectBox& sunset) {
    float sunset_cx = (sunset.x1 + sunset.x2) / 2.0f;
    float a_cx = (a.x1 + a.x2) / 2.0f;
    float b_cx = (b.x1 + b.x2) / 2.0f;
    float a_dist = std::abs(a_cx - sunset_cx);
    float b_dist = std::abs(b_cx - sunset_cx);
    return (a_dist < b_dist) ? RelativeLongitude::WEST : RelativeLongitude::EAST;
}

static std::vector<ObjectBox> PostToVisionAPI(const char* image_data, duckdb::idx_t image_size, const std::string &description) {
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
        std::vector<ObjectBox> objects;

        for (const auto& item : j["objects"]) {
            ObjectBox obj;
            obj.label = item.at("label").get<std::string>();
            obj.confidence = item.at("confidence").get<float>();
            const auto& bbox = item.at("bbox");
            if (bbox.size() != 4) {
                throw std::runtime_error("bbox must have 4 elements");
            }
            obj.x1 = bbox[0];
            obj.y1 = bbox[1];
            obj.x2 = bbox[2];
            obj.y2 = bbox[3];
            objects.push_back(obj);
        }
		return objects;
	} catch (const std::exception &e) {
        throw std::runtime_error(std::string("Exception during POST: ") + e.what());
    }
}

std::string GenerateSpatialSQL(const ObjectBox& box_a, const ObjectBox& box_b, const ObjectBox* sunset) {
    std::string direction_condition;
    if (sunset != nullptr) {
        RelativeLongitude relative = compare_relative_longitude(box_a, box_b, *sunset);
        if (relative == RelativeLongitude::WEST) { // a 在 b 西侧
            direction_condition = " AND ST_X(a.location) <= ST_X(b.location) ";
        } else {
            direction_condition = " AND ST_X(b.location) <= ST_X(a.location) ";
        }
    }
    std::string sql =
        " WITH geo_a AS ( "
        "    SELECT * FROM geo_table WHERE name LIKE '%" + box_a.label + "%' "
        " ), geo_b AS ( "
        "    SELECT * FROM geo_table WHERE name LIKE '%" + box_b.label + "%' "
        " ) "
        " SELECT "
        "    a.name AS a_name, "
        "    a.address AS a_address, "
        "    b.name AS b_name, "
        "    b.address AS b_address, "
        "    ROUND((st_distance(a.location, b.location) / 0.0111) * 1000) AS distance "
        " FROM geo_a AS a "
        " JOIN geo_b AS b ON 1=1 "
        " WHERE 1=1 " + direction_condition + " "
        " ORDER BY distance "
        " LIMIT 5;";

    return sql;
}

std::string RunSpatialQuery(std::string sql) {
	auto con = Config::GetConnection();
	auto result = con.Query(duckdb_fmt::format(sql));
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
             // 调用 API 获取结构 ObjectBox 列表
             std::vector<ObjectBox> objects = PostToVisionAPI(blob_ptr, blob_size, desc);

             // 查找落日
		     std::vector<ObjectBox> candidates;
		     ObjectBox sunset_box;
		     bool has_sunset = false;
		     nlohmann::json vision_json;
		     vision_json["objects"] = nlohmann::json::array();
             for (const auto &obj : objects) {
                 vision_json["objects"].push_back({
                    {"label", obj.label},
                    {"confidence", obj.confidence},
                    {"bbox", {obj.x1, obj.y1, obj.x2, obj.y2}}
                });
                 if (obj.label.find("落日") != std::string::npos) {
                     sunset_box = obj;
                     has_sunset = true;
                 } else {
                     candidates.push_back(obj);
                 }
             }

		     nlohmann::json spatial_array  = nlohmann::json::array();
		     for (size_t i = 0; i < candidates.size(); ++i) {
		         for (size_t j = i + 1; j < candidates.size(); ++j) {
		             auto sql = GenerateSpatialSQL(candidates[i], candidates[j], has_sunset ? &sunset_box : nullptr);
		             auto res = RunSpatialQuery(sql);
		             nlohmann::json spatial_item;
		             spatial_item["pair"] = {candidates[i].label, candidates[j].label};
		             spatial_item["results"] = nlohmann::json::parse(res);
		             spatial_array.push_back(spatial_item);
                 }
             }

 			nlohmann::json final_json;
            final_json["vision"] = vision_json;
		    final_json["spatial"] = spatial_array;
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