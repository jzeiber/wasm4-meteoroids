#pragma once

#include "gameobject.h"

struct Explosion
{
	struct GameObject m_obj;
	int16_t m_ticksleft;
};

struct Game;

void Explosion_Initialize(struct Explosion *e);
void Explosion_Update(struct Explosion *e, const int ticks);
void Explosion_Create(struct Game *g, struct GameObject *o);
