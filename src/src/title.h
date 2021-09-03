#pragma once

#include "game.h"

struct Title
{
	int64_t m_blinkcount;
	bool m_blinkon;
};

struct Game;

void Title_Initialize(struct Title *t, struct Game *g);
void Title_Update(struct Title *t, struct Game *g, const int ticks);
void Title_Draw(struct Title *t, struct Game *g);
