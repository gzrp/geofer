#include "geofer/functions/scalar/image_analyze.hpp"
#include "geofer/registry/registry.hpp"

namespace geofer {

void ScalarRegistry::RegisterImageAnalyze(duckdb::DatabaseInstance& db) {
    duckdb::ExtensionUtil::RegisterFunction(
      db, duckdb::ScalarFunction("image_analyze",                    // 注册函数名
                                 {duckdb::LogicalType::BLOB,         // 参数1：图片二进制
                                  duckdb::LogicalType::VARCHAR},      // 参数2：图片描述
							      duckdb::LogicalType::VARCHAR,
                                  ImageAnalyze::Execute));           // 执行函数
}

} // namespace geofer