#pragma once

#include <string_view>
#include <functional>
#include <map>
#include <vector>
#include <algorithm>

namespace input {
	enum class e_key_state {
		pressed,
		released
	};
	using key_t = std::uint32_t;
	using handle_function_t = std::function<void(e_key_state state)>;

	struct key_handler_t;
	void add_handler(key_handler_t* handler);
	key_handler_t* get_handler(std::string_view name);
	
	struct key_handler_t {
		std::string name;
		key_t key;
		handle_function_t function;

		key_handler_t(std::string_view handler_name, key_t handler_key, handle_function_t handler_callback, bool add_to_list = true) {
			name = handler_name;
			key = handler_key;
			function = handler_callback;

			if (add_to_list && get_handler(name) == nullptr)
				add_handler(this);
		}

		inline void operator()(e_key_state key_state) const { return function(key_state); }
	};

	namespace internal {
		using handlers_storage_t = std::vector<key_handler_t*>;

		struct key_handlers_t {
			handlers_storage_t handlers_storage;

			inline auto get_handler(std::string_view name) {
				return std::find_if(handlers_storage.begin(), handlers_storage.end(), [&](decltype(handlers_storage)::value_type v) { return v->name == name; });
			}
			inline auto begin() const { return handlers_storage.begin(); }
			inline auto end() const { return handlers_storage.end(); }
			inline auto add(key_handler_t* t) { 
				return handlers_storage.emplace_back(t);
			}
		};

		key_handlers_t& get_handlers();
		
		inline void notify_handlers(key_t key, e_key_state state) {
			for (auto handler : get_handlers().handlers_storage)
				if (handler->key == key)
					handler->function(state);
		}
	}

	inline void add_handler(key_handler_t* handler) {
		internal::get_handlers().add(handler);
	}

	inline key_handler_t* get_handler(std::string_view name) {
		auto handler = internal::get_handlers().get_handler(name);
		return handler == internal::get_handlers().end() ? nullptr : *handler;
	}
}