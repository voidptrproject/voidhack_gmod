#pragma once

#include <functional>
#include "interfaces.hpp"

namespace hooks {
	using create_move_listener = bool(*)(float, c_user_cmd*);
	using frame_stage_notify_listener = void(*)(int);
	using lock_cursor_listener = bool(*)();

	enum class e_hook_type {
		create_move,
		frame_stage_notify,
		lock_cursor
	};

	void add_listener(e_hook_type hook, void* listener);

	void initialize_hooks();
	void shutdown_hooks();
}