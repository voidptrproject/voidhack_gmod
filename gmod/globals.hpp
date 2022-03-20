#pragma once

#include <string>
#include <string_view>
#include <variant>
#include <algorithm>
#include <map>
#include <filesystem>
#include <codecvt>
#include <assert.h>

#include <json.hpp>

#include "game/misc/color.hpp"

namespace settings {
	enum e_bind_type {
		e_bind_type_none = 0,
		e_bind_type_pressed,
		e_bind_type_just_pressed
	};

	struct bind_variable_t {
		int key;
		e_bind_type bind_type;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(bind_variable_t, key, bind_type);
	};
}

namespace internal {
	enum class EVaribleType {
		None = -1,
		Number,
		Boolean,
		FloatNumber,
		String,
		Color,
		Bind
	};

	struct VariableStorageUnit {
		EVaribleType variableType;
		std::string variableName;
		std::variant<settings::bind_variable_t, c_color, bool, int, float, std::string> variableData;
	};

	using VariablesStorage_t = std::vector<VariableStorageUnit>;
	VariablesStorage_t& GetSettingsStorage();

	template<typename T> inline constexpr EVaribleType GetTypeIndex() {
		if constexpr (std::is_same<T, bool>::value)
			return EVaribleType::Boolean;
		else if constexpr (std::is_integral<T>::value || std::is_enum<T>::value)
			return EVaribleType::Number;
		else if constexpr (std::is_floating_point<T>::value)
			return EVaribleType::FloatNumber;
		else if constexpr (std::is_same<T, std::string>::value || std::is_same<T, const char*>::value || std::is_same<T, char*>::value)
			return EVaribleType::String;
		else if constexpr (std::is_class<T>::value && type_id<T>() == type_id <c_color>())
			return EVaribleType::Color;
		else if constexpr (std::is_class<T>::value && type_id<T>() == type_id <settings::bind_variable_t>())
			return EVaribleType::Bind;
		return EVaribleType::None;
	}

	inline void to_json(nlohmann::json& json, const VariableStorageUnit& u) {
		json["name"] = u.variableName;
		switch (u.variableType) {
		case EVaribleType::Number:
			json["value"] = std::get<int>(u.variableData);
			break;
		case EVaribleType::FloatNumber:
			json["value"] = std::get<float>(u.variableData);
			break;
		case EVaribleType::String:
			json["value"] = std::get<std::string>(u.variableData);
			break;
		case EVaribleType::Boolean:
			json["value"] = std::get<bool>(u.variableData);
			break;
		case EVaribleType::Color:
			json["value"] = std::get<c_color>(u.variableData).get_u32();
			break;
		case EVaribleType::Bind:
			json["value"] = std::get<settings::bind_variable_t>(u.variableData);
			break;
		}
	}
}

namespace settings {
	template<typename T> inline T& GetVariable(std::string_view name) {
		auto var = std::find_if(internal::GetSettingsStorage().begin(), internal::GetSettingsStorage().end(), 
			[&](internal::VariableStorageUnit& u) { return u.variableName == name; });
		assert(var != internal::GetSettingsStorage().end());
		return std::get<T>(var->variableData);
	}
	template<typename T> inline T* GetVariablePointer(std::string_view name) {
		auto var = std::find_if(internal::GetSettingsStorage().begin(), internal::GetSettingsStorage().end(),
			[&](internal::VariableStorageUnit& u) { return u.variableName == name; });
		assert(var != internal::GetSettingsStorage().end());
		return &std::get<T>(var->variableData);
	}

	std::string SaveSettingsToString();
	void SaveSettingsToStream(std::ostream& stream);

	void LoadSettingsFromString(std::string_view data);
	void LoadSettingsFromStream(std::istream& stream);

	enum EVisualSettings {
		EVisualSettings_None = (0 << 0),
		EVisualSettings_Box = (1 << 0),
		EVisualSettings_Name = (1 << 1),
		EVisualSettings_HealthBar = (1 << 2),
		EVisualSettings_UserGroup = (1 << 3),
		EVisualSettings_TeamName = (1 << 4),
	};

	template<typename VarType>
	inline auto CreateVariable(std::string_view name, VarType&& value) {
		internal::VariableStorageUnit tmp;
		tmp.variableName = name;
		tmp.variableType = internal::GetTypeIndex<VarType>();
		tmp.variableData = value;

		return internal::GetSettingsStorage().emplace_back(std::move(tmp));
	}
}

namespace globals {
	
}

namespace env {
	inline std::filesystem::path get_data_path() {
		auto path = (std::filesystem::path(std::getenv("APPDATA")) / L"voidproject") / L"gmod";
		if (!std::filesystem::exists(path)) std::filesystem::create_directories(path);
		return path;
	}

	inline std::string wstring_to_string(const std::wstring& wstr) {
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;
		return converterX.to_bytes(wstr);
	}
}