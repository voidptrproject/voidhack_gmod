#include "globals.hpp"

#include <json.hpp>

#define SETTINGS_VAR(name, type, def) static Variable<type> name(#name, def)

namespace internal {
	static inline ::internal::VariablesStorage_t settings;

	template<typename T> inline constexpr EVaribleType GetTypeIndex() {
		if constexpr (std::is_same<T, bool>::value)
			return EVaribleType::Boolean;
		else if constexpr (std::is_integral<T>::value || std::is_enum<T>::value)
			return EVaribleType::Number;
		else if constexpr (std::is_floating_point<T>::value)
			return EVaribleType::FloatNumber;
		else if constexpr (std::is_same<T, std::string>::value || std::is_same<T, const char*>::value || std::is_same<T, char*>::value)
			return EVaribleType::String;
		else if constexpr (std::is_class<T>::value)
			return EVaribleType::Color;
		return EVaribleType::None;
	}

	inline void to_json(nlohmann::json& json, const VariableStorageUnit& u) {
		json["name"] = u.variableName;
		switch (u.variableType) {
		case EVaribleType::Number:
			json["value"] = *std::get<int*>(u.variableData);
			break;
		case EVaribleType::FloatNumber:
			json["value"] = *std::get<float*>(u.variableData);
			break;
		case EVaribleType::String:
			json["value"] = *std::get<std::string*>(u.variableData);
			break;
		case EVaribleType::Boolean:
			json["value"] = *std::get<bool*>(u.variableData);
			break;
		case EVaribleType::Color:
			json["value"] = std::get<c_color*>(u.variableData)->get_u32();
			break;
		}
	}
}

::internal::VariablesStorage_t& internal::GetSettingsStorage() {
	return ::internal::settings;
}

template<typename VarType> struct Variable {
	VarType data;
	internal::VariableStorageUnit* myStorageUnit;

	Variable(std::string_view name, VarType&& value) : data(value) {
		internal::VariableStorageUnit tmp;
		tmp.variableName = name;
		tmp.variableType = internal::GetTypeIndex<VarType>();
		tmp.variableData = &this->data;

		myStorageUnit = &internal::GetSettingsStorage().emplace_back(std::move(tmp));
	}

	operator VarType& () { return data; }
	operator VarType* () { return &data; }

	inline VarType* GetPointer() { return &data; }
};

namespace Variables {
	SETTINGS_VAR(Esp, bool, false);
	SETTINGS_VAR(EspVisualSettings, int, 0);

	SETTINGS_VAR(BunnyHop, bool, false);
	SETTINGS_VAR(ObserversHUD, bool, false);
	SETTINGS_VAR(InformationHUD, bool, false);
}

std::string settings::SaveSettingsToString() {
	return nlohmann::json(internal::settings).dump();
}

void settings::SaveSettingsToStream(std::ostream& stream) {
	stream.write(SaveSettingsToString().c_str(), SaveSettingsToString().size());
}

void settings::LoadSettingsFromString(std::string_view data) {
	auto json = nlohmann::json::parse(data);
	for (auto i = json.begin(); i != json.end(); ++i) {
		auto set = std::find_if(internal::settings.begin(), internal::settings.end(), 
			[&](const internal::VariableStorageUnit& u) { return u.variableName == i->at("name").get<std::string>(); });
		if (set != internal::settings.end()) {
			switch (set->variableType) {
			case internal::EVaribleType::Number:
				*std::get<int*>(set->variableData) = i->at("value").get<int>();
				break;
			case internal::EVaribleType::FloatNumber:
				*std::get<float*>(set->variableData) = i->at("value").get<float>();
				break;
			case internal::EVaribleType::String:
				*std::get<std::string*>(set->variableData) = i->at("value").get<std::string>();
				break;
			case internal::EVaribleType::Boolean:
				*std::get<bool*>(set->variableData) = i->at("value").get<bool>();
				break;
			case internal::EVaribleType::Color:
				*std::get<c_color*>(set->variableData) = c_color(ImGui::ColorConvertU32ToFloat4(i->at("value").get<ImU32>()));
				break;
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
