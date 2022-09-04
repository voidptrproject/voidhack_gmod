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

#define create_variable(name, type) settings::variable<type> name(#name, type{});

namespace observer {
	struct subject {
		
	};
}

namespace internal {
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
}

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

	struct base_variable {
		virtual void save(nlohmann::json& j) = 0;
		virtual void load(const nlohmann::json& j) = 0;
	};

	template <typename t>
	struct variable : public base_variable {
		t data;
		std::string name;

		variable(std::string_view name, t&& data) : name(name), data(data) {}

		virtual void save(nlohmann::json& j) override {
			j[name] = data; 
		}

		virtual void load(const nlohmann::json& j) override { 
			if(j.is_object() && !j[name].is_null())
				data = j[name].get<t>(); 
		}

		operator t& () { return data; }
		variable<t>& operator=(const t& d) { data = d; return *this; }
		bool operator==(const t& d) { return data == d; }

		t& get() { return data; }
		t* ptr() { return &data; }
	};

	std::string save_settings_to_string();
	void save_settings_to_stream(std::ostream& stream);

	void load_settings_from_string(std::string_view data);
	void load_settings_from_stream(std::istream& stream);

	std::vector<base_variable*>& get_vars();

	template<typename ...vars>
	constexpr inline void register_variables(vars&... v) {
		get_vars().insert(get_vars().end(), {(&v)...});
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