#pragma once

#include "gameobject.h"

struct Spaceship
{
	struct GameObject m_obj;
};

void Spaceship_Initialize(struct Spaceship *s);
