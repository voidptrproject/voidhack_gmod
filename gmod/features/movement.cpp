#include "features.hpp"

#include "../hooks.hpp"
#include "../globals.hpp"

#include "../game/entities/c_base_player.hpp"

static bool movement_create_move(float frametime, c_user_cmd* cmd) {
	if (settings::GetVariable<bool>("BunnyHop")) {
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
	return false;
}

static inline features::feature bunny_hop([](){
	settings::CreateVariable("BunnyHop", false);

	menu::AddElementToCategory(menu::EMenuCategory_Misc, std::make_shared<menu::ToggleButtonElement>("BunnyHop", "BunnyHop"));

	hooks::add_listener(hooks::e_hook_type::create_move, movement_create_move);
});