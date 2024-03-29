#pragma once
#include "../math/math.hpp"

#define IN_ATTACK		(1 << 0)
#define IN_JUMP			(1 << 1)
#define IN_DUCK			(1 << 2)
#define IN_FORWARD		(1 << 3)
#define IN_BACK			(1 << 4)
#define IN_USE			(1 << 5)
#define IN_CANCEL		(1 << 6)
#define IN_LEFT			(1 << 7)
#define IN_RIGHT		(1 << 8)
#define IN_MOVELEFT		(1 << 9)
#define IN_MOVERIGHT	(1 << 10)
#define IN_ATTACK2		(1 << 11)
#define IN_RUN			(1 << 12)
#define IN_RELOAD		(1 << 13)
#define IN_ALT1			(1 << 14)
#define IN_ALT2			(1 << 15)
#define IN_SCORE		(1 << 16)   // Used by client.dll for when scoreboard is held down
#define IN_SPEED		(1 << 17)	// Player is holding the speed key
#define IN_WALK			(1 << 18)	// Player holding walk key
#define IN_ZOOM			(1 << 19)	// Zoom key for HUD zoom
#define IN_WEAPON1		(1 << 20)	// weapon defines these bits
#define IN_WEAPON2		(1 << 21)	// weapon defines these bits
#define IN_BULLRUSH		(1 << 22)
#define IN_GRENADE1		(1 << 23)
#define IN_GRENADE2		(1 << 24)
#define	IN_ATTACK3		(1 << 25)

class c_user_cmd
{
public:
	int		command_number;
	int		tick_count;
	q_angle viewangles;
	float	forwardmove;
	float	sidemove;
	float	upmove;
	int		buttons;
	char impulse;
	int		weaponselect;
	int		weaponsubtype;
	int		random_seed;	// For shared random functions
	short	mousedx;		// mouse accum in x from create move
	short	mousedy;		// mouse accum in y from create move
	bool	hasbeenpredicted;

	bool is_button_set(int btn) const {
		return buttons & btn;
	}

	bool is_in_fire() const {
		return buttons & IN_ATTACK;
	}
	
};