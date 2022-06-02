#pragma once

#include <functional>
#include "interfaces.hpp"

namespace hooks {
	using create_move_listener = bool(*)(float, c_user_cmd*);
	using frame_stage_notify_listener = void(*)(int);
	using cl_move_listener = void(*)(float, bool);

	enum class e_hook_type {
		create_move,
		frame_stage_notify,
		cl_move
	};

	namespace internal {
		void hook(LPVOID address, LPVOID new_function, LPVOID* original);
		void unhook(LPVOID address);
	}

	template <typename func> struct hook_t {
		hook_t() {}
		hook_t(func new_function) : hook_fn(new_function) {}
		~hook_t() {}

		func original;
		func hook_fn;

		void hook(uintptr_t address) { internal::hook((LPVOID)address, (LPVOID)hook_fn, (LPVOID*)&original); }
		void unhook() { internal::unhook((LPVOID)original); }
	};

	void add_listener(e_hook_type hook, void* listener);

	void initialize_hooks();
	void shutdown_hooks();
}

namespace utils {
	namespace details {
		extern bool current_send_packets;
		void apply_send_packets();
	}
	inline bool send_packets(bool val) {
		details::current_send_packets = val;
	}
}