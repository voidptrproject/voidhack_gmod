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
		static inline ::internal::variables_storage_t settings;
	}

	template<typename t> inline t& get(std::string_view name, const t& new_val = t()) {
		if (!internal::settings.exists(name))
			internal::settings[name] = new_val;
		return *std::any_cast<t>(&internal::settings[name]);
	}
	template<typename t> inline void set(std::string_view name, const t& val) {
		internal::settings[name] = val;
	}
}