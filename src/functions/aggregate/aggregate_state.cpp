#include "geofer/functions/aggregate/aggregate.hpp"

namespace geofer {

void AggregateFunctionState::Initialize() {}

void AggregateFunctionState::Update(const nlohmann::json& input) { value.push_back(input); }

void AggregateFunctionState::Combine(const AggregateFunctionState& source) {
  for (auto& input : source.value) {
    Update(std::move(input));
  }
}

} // namespace geofer