#include "features.hpp"

class fire_bullets_info_t {
public:
	int32_t shoots_num; //0x0000
	c_vector src; //0x0004
	c_vector direction; //0x0010
	c_vector spread; //0x001C
	float distance; //0x0028
	int32_t ammo_type; //0x002C
	int32_t trace_freq; //0x0030
	float damage; //0x0034
	int32_t player_damage; //0x0038
	int32_t flags; //0x003C
	float damage_scale; //0x0040
	c_base_entity* attacker; //0x0044
	c_base_entity* ignore; //0x004C
	bool primaryattack; //0x0054
	bool userserverrandomseed; //0x0055
}; //Size: 0x0056

struct shoot_render_data {
	c_vector position;
	c_vector screen_position;
	c_vector spread;
	std::string weapon;
};

std::vector<shoot_render_data> shoots_data;
std::mutex rendermutex;

c_vector apply_spread(c_user_cmd* cmd) {

	if (get_local_player()->get_active_weapon()->is_use_lua()) {
		gmod_lua_interface lua(interfaces::lua_shared->get_lua_interface((int)e_special::glob));
		get_local_player()->get_active_weapon()->push_entity();
		lua->get_field(-1, "Primary");
		auto object = lua->get_object(-1);
		auto cone = object->get_member_float("Spread");

		auto seed = md5::md5_pseudo_random(cmd->command_number) & 0xFF;
		interfaces::random->set_seed(seed);

		auto spread = cone;

		float x, y;
		x = interfaces::random->random_float(-0.5f, 0.5) + interfaces::random->random_float(-0.5f, 0.5);
		y = interfaces::random->random_float(-0.5f, 0.5) + interfaces::random->random_float(-0.5f, 0.5);

		c_vector forward, right, up;
		math::angle_to_vectors(cmd->viewangles, forward, right, up);

		return forward + (right * c_vector(spread) * c_vector(x)) + (up * c_vector(spread) * c_vector(y));
	}
	return { 0.f };
}

bool create_move(float f, c_user_cmd* cmd) {
	if (input::get_key_state('F') && cmd->is_button_set(IN_ATTACK) && get_local_player()->get_active_weapon()) {
		auto dir = apply_spread(cmd);
		
		auto angle = math::get_angle({0.f}, dir);
		angle = math::fix_angles(angle);

		auto delta = cmd->viewangles - angle;
		cmd->viewangles += delta;

		//interfaces::engine->set_view_angles(cmd->viewangles);
	}
	return false;
}

void renderf(render::render_data_t& r) {
}

features::feature nospread([]() {
	hooks::add_listener(hooks::e_hook_type::create_move, create_move);
});