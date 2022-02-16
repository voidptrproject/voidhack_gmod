#pragma once

#include <string>
#include <string_view>
#include <any>
#include <algorithm>
#include <map>

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