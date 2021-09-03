#pragma once

#include "gameobject.h"

struct UFO
{
	struct GameObject m_obj;
	int64_t m_ticksalive;
	int64_t m_tickssinceshot;
	int64_t m_tickssincechangedir;
};

struct Game;

void UFO_Initialize(struct UFO *u);
void UFO_Randomize(struct UFO *u);
void UFO_Update(struct Game *g, struct UFO *u, const int ticks);
void UFO_Draw(struct UFO *u);
