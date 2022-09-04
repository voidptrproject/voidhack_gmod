#include "features.hpp"

#include "../hooks.hpp"
#include "../globals.hpp"

#include "../game/entities/c_base_player.hpp"

create_variable(bunny_hop_enabled, bool);
create_variable(auto_strafe, bool);

inline auto bhop(c_user_cmd* cmd) {
	if (bunny_hop_enabled) {
		auto local_player = get_local_player();
		static bool should_fake = false;
		if (static bool last_jumped = false; !last_jumped && should_fake) {
			should_fake = false;
			cmd->buttons |= IN_JUMP;
		} else if (cmd->buttons & IN_JUMP) {
			if (local_player->get_flags() & (1 << 0)) {
				last_jumped = true;
				should_fake = true;
			} else {
				cmd->buttons &= ~IN_JUMP;
				last_jumped = false;
			}
		} else {
			last_jumped = false;
			should_fake = false;
		}
	}
}
auto autostrafe(c_user_cmd* cmd) {
	auto local_player = get_local_player();

	if (auto_strafe && cmd->buttons & IN_JUMP) {
		cmd->forwardmove += 1000.f;

		if (cmd->mousedx < 0) {
			cmd->sidemove -= 1000.f;
		} else if (cmd->mousedx > 0) {
			cmd->sidemove += 1000.f;
		}
	}
}

bool movement_create_move(float frametime, c_user_cmd* cmd) {
	bhop(cmd);
	autostrafe(cmd);
	return false;
}

static inline features::feature bunny_hop([](){
	settings::register_variables(bunny_hop_enabled, auto_strafe);

	menu::AddElementToCategory(menu::EMenuCategory_Misc, std::make_shared<menu::ToggleButtonElement>("BunnyHop", bunny_hop_enabled.ptr()));
	menu::AddElementToCategory(menu::EMenuCategory_Misc, std::make_shared<menu::ToggleButtonElement>("AutoStrafe", auto_strafe.ptr()));

	hooks::add_listener(hooks::e_hook_type::create_move, movement_create_move);
});

