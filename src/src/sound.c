#include "sound.h"
#include "wasm4.h"

void Sound_DestroyAsteroid()
{
	tone(100 | (70 << 16),16 | (6 << 8),30,TONE_NOISE);
}

void Sound_PlayerFireProjectile()
{
	tone(1000 | (600 << 16),8 | (4 << 8),30,TONE_PULSE1);
}

void Sound_UFOFireProjectile()
{
	tone(900 | (500 << 16),8 | (4 << 8),30,TONE_PULSE2);
}

void Sound_PlayerCollide()
{
	tone(350 | (210 << 16),32 | (50 << 8),30,TONE_NOISE);
}
