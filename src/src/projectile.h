#pragma once

#include "gameobject.h"

enum Owner
{
OWNER_PLAYER=1,
OWNER_UFO=2
};

struct Projectile
{
	struct GameObject m_obj;
	int8_t m_owner;
	int64_t m_ticks;
	int64_t m_maxticks;
};

void Projectile_Initialize(struct Projectile *p);
void Projectile_Update(struct Projectile *p, const int ticks);
