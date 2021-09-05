#pragma once

#define MAX_ASTEROIDS 100
#define MAX_PROJECTILES 20
#define MAX_EXPLOSIONS 30

#include "input.h"
#include "asteroid.h"
#include "spaceship.h"
#include "projectile.h"
#include "explosion.h"
#include "ufo.h"
#include "title.h"

enum GameState
{
STATE_STARTUP=0,
STATE_TITLE=1,
STATE_GAME=2,
STATE_GAMEOVER=3
};

enum PlayState
{
STATE_WAIT=0,
STATE_PLAY=1
};

struct Game
{
	int64_t m_ticks;
	int8_t m_state;
	int64_t m_score;
	int64_t m_highscore;
	int64_t m_tickssinceshot;
	int8_t m_playstate;
	int64_t m_waitticks;
	int64_t m_tickssincelastasteroid;
	int64_t m_tickssinceufo;
	int8_t m_lives;
	
	struct Input m_input;
	
	struct Spaceship m_player;
	struct UFO m_ufo;
	struct Asteroid m_asteroids[MAX_ASTEROIDS];
	struct Projectile m_projectiles[MAX_PROJECTILES];
	struct Explosion m_explosions[MAX_EXPLOSIONS];
	
};

void Game_Initialize(struct Game *g);
void Game_Update(struct Game *g, const int ticks);
void Game_Draw(struct Game *g);
void Game_NewGame(struct Game *g);
void Game_ResetStateAndWait(struct Game *g);
void Game_HandlePlayerCollision(struct Game *g, struct GameObject *o);
struct Projectile *Game_NextFreeProjectile(struct Game *g);	// find next free projectile, returns oldest if all are alive
struct Asteroid *Game_NextFreeAsteroid(struct Game *g);		// find next dead asteroid, returns NULL if all are alive
struct Explosion *Game_NextFreeExplosion(struct Game *g);	// find next dead explosion, returns NULL if all are alive