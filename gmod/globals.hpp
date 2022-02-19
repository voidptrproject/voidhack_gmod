#pragma once

#include <string>
#include <string_view>
#include <any>
#include <algorithm>
#include <map>
#include <filesystem>
#include <codecvt>

namespace internal {
	struct variables_storage_t {
		std::map<std::string, std::any> data;

		inline auto& operator[](std::string_view name) { return data[name.data()]; }
		inline auto& operator[](const std::string& name) { return data[name]; }

		inline bool exists(const std::string_view name) { return data.contains(name.data()); }
	};
}

namespace settings {
	namespace internal {
		::internal::variables_storage_t& get_settings_storage();
	}

	template<typename t> inline t& get(std::string_view name, const t& new_val = t()) {
		if (!internal::get_settings_storage().exists(name))
			internal::get_settings_storage()[name] = new_val;
		return *std::any_cast<t>(&internal::get_settings_storage()[name]);
	}
	template<typename t> inline void set(std::string_view name, const t& val) {
		internal::get_settings_storage()[name] = val;
	}
}

namespace globals {
	namespace internal {
		::internal::variables_storage_t& get_globals_storage();
	}

	template<typename t> inline t& get(std::string_view name, const t& new_val = t()) {
		if (!internal::get_globals_storage().exists(name))
			internal::get_globals_storage()[name] = new_val;
		return *std::any_cast<t>(&internal::get_globals_storage()[name]);
	}
	template<typename t> inline void set(std::string_view name, const t& val) {
		internal::get_globals_storage()[name] = val;
	}
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