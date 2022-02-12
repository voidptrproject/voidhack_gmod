#pragma once

#include "c_base_entity.hpp"

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
};

__forceinline c_base_player* get_local_player()
{
	return reinterpret_cast<c_base_player*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
}


__forceinline c_base_player* get_player_by_index(const uint32_t i)
{
	return reinterpret_cast<c_base_player*>(interfaces::entity_list->get_client_entity(i));
}