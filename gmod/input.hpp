#pragma once

#include <string_view>
#include <functional>
#include <map>
#include <vector>
#include <algorithm>

namespace input {
	enum class EKeyState {
		Pressed,
		Released
	};
	using KeyType = std::uint32_t;
	using HandleFunction_t = std::function<void(EKeyState state)>;

	struct KeyHandler;
	void AddHandler(KeyHandler* handler);
	KeyHandler* GetHandler(std::string_view name);
	
	struct KeyHandler {
		std::string name;
		KeyType key;
		HandleFunction_t function;

		KeyHandler(std::string_view handler_name, KeyType handler_key, HandleFunction_t handler_callback, bool add_to_list = true) {
			name = handler_name;
			key = handler_key;
			function = handler_callback;

			if (add_to_list && GetHandler(name) == nullptr)
				AddHandler(this);
		}

		inline void operator()(EKeyState key_state) const { return function(key_state); }
	};

	namespace internal {
		using handlers_storage_t = std::vector<KeyHandler*>;

		struct key_handlers_t {
			handlers_storage_t handlers_storage;

			inline auto get_handler(std::string_view name) {
				return std::find_if(handlers_storage.begin(), handlers_storage.end(), 
					[&](decltype(handlers_storage)::value_type v) { return v->name == name; });
			}
			inline auto begin() const { return handlers_storage.begin(); }
			inline auto end() const { return handlers_storage.end(); }
			inline auto add(KeyHandler* t) { 
				return handlers_storage.emplace_back(t);
			}
		};

		key_handlers_t& get_handlers();
		
		inline void notify_handlers(KeyType key, EKeyState state) {
			for (auto handler : get_handlers().handlers_storage)
				if (handler->key == key)
					handler->function(state);
		}
	}

	inline void AddHandler(KeyHandler* handler) {
		internal::get_handlers().add(handler);
	}

	inline KeyHandler* GetHandler(std::string_view name) {
		auto handler = internal::get_handlers().get_handler(name);
		return handler == internal::get_handlers().end() ? nullptr : *handler;
	}
}