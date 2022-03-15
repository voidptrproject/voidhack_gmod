#pragma once

#include "c_base_entity.hpp"
#include "../../memory.hpp"

#include <algorithm>

class c_base_combat_weapon;

enum class e_move_type
{
	none = 0,
	isometric,
	walk,
	step,
	fly,
	flygravity,
	vphysics,
	push,
	noclip,
	ladder,
	observer,
	custom,
	last = custom,
	max_bits = 4,
	movetype
};

class c_base_player : public c_base_entity {
public:
	netvar("DT_BasePlayer", "m_fFlags", get_flags, int);
	netvar("DT_BasePlayer", "m_vecViewOffset[0]", get_view_offset, c_vector);
	netvar("DT_BasePlayer", "m_hActiveWeapon", active_weapon_handle, uintptr_t);
	netvar("DT_BasePlayer", "m_vecVelocity[0]", get_velocity, c_vector);
	netvar("DT_BasePlayer", "hMyWeapons", get_weapons, uintptr_t);
	netvar("DT_BasePlayer", "m_bloodColor", get_blood_color, c_vector);
	netvar("DT_BasePlayer", "m_Local", get_local, uintptr_t);
	netvar("DT_BasePlayer", "m_iAmmo", get_ammo, int);
	netvar("DT_BasePlayer", "m_fOnTarget", get_on_target, int);
	netvar("DT_BasePlayer", "m_nTickBase", get_tick_base, int);
	netvar("DT_BasePlayer", "m_nNextThinkTick", get_next_think_tick, float);
	netvar("DT_BasePlayer", "m_flFriction", get_friction, float);
	netvar("DT_BasePlayer", "flDeathTime", get_death_time, float);
	netvar("DT_BasePlayer", "deadflag", get_dead_flag, int);
	netvar("DT_BasePlayer", "m_iDefaultFOV", get_default_fov, int);
	netvar("DT_BasePlayer", "m_hVehicle", get_vehicle_handle, uintptr_t);
	netvar("DT_BasePlayer", "m_iObserverMode", get_observer_mode, int);
	netvar("DT_BasePlayer", "m_hObserverTarget", get_observer_target_handle, uintptr_t);
	netvar("DT_PlayerResource", "m_iPing", get_ping, int);

	c_color get_team_color() {
		c_color color;
		gmod_lua_interface glua(interfaces::lua_shared->get_lua_interface((int)e_special::glob));

		glua->push_special((int)e_special::glob);

		glua->push_special((int)e_special::glob);
		glua->get_field(-1, "team");
		glua->get_field(-1, "GetColor");
		glua->push_number(this->get_team_num());
		glua->call(1, 1);

		auto color_object = glua->get_object(-1);
		auto r = color_object->get_member_int("r");
		auto g = color_object->get_member_int("g");
		auto b = color_object->get_member_int("b");

		color.init(r, g, b);
		return color;
	}

	std::string get_team_name() {
		gmod_lua_interface glua(interfaces::lua_shared->get_lua_interface((int)e_special::glob));
		glua->push_special((int)e_special::glob);
		glua->get_field(-1, "team");
		glua->get_field(-1, "GetName");
		glua->push_number(this->get_team_num());
		glua->call(1, 1);
		return glua->get_string();
	}

	std::string get_user_group() {
		gmod_lua_interface lua(interfaces::lua_shared->get_lua_interface((int)e_special::glob));

		push_entity();
		lua->get_field(-1, "GetUserGroup");
		lua->push(-2);
		lua->call(1, 1);

		return lua->get_string();
	}

	bool is_admin() {
		auto string_contains = [](const std::string& str, const std::string& substr) {
			return str.find(substr) != std::string::npos;
		};
		auto user_group = get_user_group();
		std::transform(user_group.begin(), user_group.end(), user_group.begin(), [](char c) { return std::tolower(c); });
		return string_contains(user_group, "admin") || string_contains(user_group, "moder") ||
			string_contains(user_group, "root") || string_contains(user_group, "king") || string_contains(user_group, "owner");
	}

	std::string get_name() const {
		player_info_s info;
		interfaces::engine->get_player_info(get_index(), &info);
		return info.name;
	}

	c_base_player* get_observer_target() {
		static auto function = memory::symbol_t<void* (__fastcall*)(void*)>({ { "40 53 48 83 EC 20 48 39 0D ? ? ? ? 48 8B D9 75 26" }, memory::client_module });
		return (c_base_player*)function.ptr(this);
	}
};

__forceinline c_base_player* get_local_player()
{
	return reinterpret_cast<c_base_player*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
}


__forceinline c_base_player* get_player_by_index(const uint32_t i)
{
	return reinterpret_cast<c_base_player*>(interfaces::entity_list->get_client_entity(i));
}