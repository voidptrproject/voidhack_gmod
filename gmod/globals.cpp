#include "globals.hpp"

#include <Windows.h>

namespace detail {
	template<typename T>
	struct type_id_ptr {
		static const T* const id;
	};

	template<typename T>
	const T* const type_id_ptr<T>::id = nullptr;

}

using type_id_t = const void*;

template <typename T>
constexpr auto type_id() noexcept -> type_id_t {
	return &detail::type_id_ptr<T>::id;
}

::internal::VariablesStorage_t& internal::GetSettingsStorage() {
	static ::internal::VariablesStorage_t settings;
	return settings;
}

std::string settings::SaveSettingsToString() {
	return nlohmann::json(internal::GetSettingsStorage()).dump();
}

void settings::SaveSettingsToStream(std::ostream& stream) {
	stream.write(SaveSettingsToString().c_str(), SaveSettingsToString().size());
}

void settings::LoadSettingsFromString(std::string_view data) {
	auto json = nlohmann::json::parse(data);
	for (auto i = json.begin(); i != json.end(); ++i) {
		auto set = std::find_if(internal::GetSettingsStorage().begin(), internal::GetSettingsStorage().end(),
			[&](const internal::VariableStorageUnit& u) { return u.variableName == i->at("name").get<std::string>(); });
		if (set != internal::GetSettingsStorage().end()) {
			switch (set->variableType) {
			case internal::EVaribleType::Number:
				set->variableData = i->at("value").get<int>();
				continue;
			case internal::EVaribleType::FloatNumber:
				set->variableData = i->at("value").get<float>();
				continue;
			case internal::EVaribleType::String:
				set->variableData = i->at("value").get<std::string>();
				continue;
			case internal::EVaribleType::Boolean:
				set->variableData = i->at("value").get<bool>();
				continue;
			case internal::EVaribleType::Color:
				set->variableData = c_color(ImGui::ColorConvertU32ToFloat4(i->at("value").get<ImU32>()));
				continue;
			case internal::EVaribleType::Bind:
				set->variableData = i->at("value").get<bind_variable_t>();
				continue;
			}
		}
	}
}

void settings::LoadSettingsFromStream(std::istream& stream) {
	std::string data;
	std::string line;
	while (std::getline(stream, line))
		data.append(line);
	LoadSettingsFromString(data);
}
