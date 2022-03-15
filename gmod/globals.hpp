#pragma once

#include <string>
#include <string_view>
#include <variant>
#include <algorithm>
#include <map>
#include <filesystem>
#include <codecvt>
#include <assert.h>

#include "game/misc/color.hpp"

namespace internal {
	enum class EVaribleType {
		None = -1,
		Number,
		Boolean,
		FloatNumber,
		String,
		Color
	};

	struct VariableStorageUnit;
	using VariablesStorage_t = std::vector<VariableStorageUnit>;
	VariablesStorage_t& GetSettingsStorage();

	struct VariableStorageUnit {
		EVaribleType variableType;
		std::string variableName;
		std::variant<c_color*, bool*, int*, float*, std::string*> variableData;
	};
}

namespace settings {
	template<typename T = bool> inline T& GetVariable(std::string_view name) {
		auto var = std::find_if(internal::GetSettingsStorage().begin(), internal::GetSettingsStorage().end(), 
			[&](internal::VariableStorageUnit& u) { return u.variableName == name; });
		assert(var != internal::GetSettingsStorage().end());
		return *std::get<T*>(var->variableData);
	}
	template<typename T = bool> inline T* GetVariablePointer(std::string_view name) {
		auto var = std::find_if(internal::GetSettingsStorage().begin(), internal::GetSettingsStorage().end(),
			[&](internal::VariableStorageUnit& u) { return u.variableName == name; });
		assert(var != internal::GetSettingsStorage().end());
		return std::get<T*>(var->variableData);
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