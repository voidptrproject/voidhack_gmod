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
	struct keystate { 
		EKeyState current = EKeyState::Released; 
		EKeyState prev = EKeyState::Pressed;

		operator bool() { return current == EKeyState::Pressed; }
		bool pressed() { return current == EKeyState::Pressed; }
		bool released() { return current == EKeyState::Released; }
	};

	using key_type_t = int;
	using handle_function_t = std::function<void(EKeyState state)>;

	struct key_handler;
	void add_handler(const key_handler& handler);
	key_handler* get_handler(std::string_view name);
	
	struct key_handler {
		std::string name;
		key_type_t* key;
		handle_function_t function;

		key_handler(std::string_view handler_name, key_type_t* handler_key, handle_function_t handler_callback) {
			name = handler_name;
			key = handler_key;
			function = handler_callback;
		}

		inline void operator()(EKeyState key_state) const { return function(key_state); }
	};

	namespace internal {
		using handlers_storage_t = std::vector<key_handler>;

		struct key_handlers_t {
			handlers_storage_t handlers_storage;

			inline auto get_handler(std::string_view name) {
				return std::find_if(handlers_storage.begin(), handlers_storage.end(), 
					[&](decltype(handlers_storage)::value_type v) { return v.name == name; });
			}
			inline auto begin() const { return handlers_storage.begin(); }
			inline auto end() const { return handlers_storage.end(); }
			inline auto add(const key_handler& t) { 
				return handlers_storage.emplace_back(t);
			}
		};

		key_handlers_t& get_handlers();
		inline std::map<uint32_t, keystate>& get_keys_states() {
			static std::map<uint32_t, keystate> states;
			return states;
		}

		inline void notify_handlers(key_type_t key, EKeyState state) {
			for (auto& handler : get_handlers().handlers_storage) {
				if (handler.key && (*handler.key == key)) {
					handler.function(state);
				}
			}
			get_keys_states()[key].prev = get_keys_states()[key].current;
			get_keys_states()[key].current = state;
		}
	}

	inline keystate get_key_state(uint32_t key) {
		return internal::get_keys_states()[key];
	}

	inline void add_handler(const key_handler& handler) {
		internal::get_handlers().add(handler);
	}

	inline key_handler* get_handler(std::string_view name) {
		auto handler = internal::get_handlers().get_handler(name);
		return handler == internal::get_handlers().end() ? nullptr : &*handler;
	}

	struct just_pressed_flag {
		EKeyState prevstate = EKeyState::Released;

		just_pressed_flag() {}

		inline auto after_handle(EKeyState state) {
			prevstate = state;
		}

		inline auto is_just_pressed(EKeyState state) {
			return (state == EKeyState::Released && prevstate == EKeyState::Released);
		}
	};
}