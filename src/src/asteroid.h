#pragma once

#include "gameobject.h"

struct Asteroid
{
	struct GameObject m_obj;
	bool m_onscreen;
};

void Asteroid_Initialize(struct Asteroid *a);
void Asteroid_Initialize_Asteroid(struct Asteroid *a, const struct Asteroid *rhs);
void Asteroid_Randomize(struct Asteroid *a, bool wholescreen);
void Asteroid_Update(struct Asteroid *a, const int ticks);
bool Asteroid_CheckOnScreen(struct Asteroid *a);	// check if any of the world coords are on the screen
