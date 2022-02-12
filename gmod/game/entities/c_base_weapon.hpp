#pragma once

#include "c_base_player.hpp"

class c_base_combat_weapon : public c_base_entity
{
public:
	netvar("DT_BaseCombatWeapon", "m_nSequence", get_sequence, int);
	netvar("DT_BaseCombatWeapon", "m_nForceBone", get_froce_bone, int);
	netvar("DT_BaseCombatWeapon", "m_iClip1", get_clip1, int);
	netvar("DT_BaseCombatWeapon", "m_iClip2", get_clip2, int);
	netvar("DT_BaseCombatWeapon", "m_iPrimaryAmmoType", get_primary_ammo_type, int);
	netvar("DT_BaseCombatWeapon", "m_iSecondaryAmmoType", get_secoundary_ammo_type, int);
	netvar("DT_BaseCombatWeapon", "m_flNextPrimaryAttack", get_next_primary_attack, float);
	netvar("DT_BaseCombatWeapon", "m_flNextSecondaryAttack", get_next_secondary_attack, float);
	netvar("DT_BaseCombatWeapon", "m_nNextThinkTick", get_next_think_tick, int);
	netvar("DT_BaseCombatWeapon", "m_iState", get_state, int);
	netvar("DT_BaseCombatWeapon", "m_hOwner", get_owner_handle, uintptr_t);

	const c_vector& get_bullet_spread()
	{
		using fn = const c_vector & (__thiscall*)(void*);
		return (*(fn**)this)[326](this);
	}
};