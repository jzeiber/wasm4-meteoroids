#pragma once

#include "point.h"
#include <stdbool.h>

enum ObjectType
{
OBJECT_NONE=0,
OBJECT_PLAYER=1,
OBJECT_ASTEROID=2,
OBJECT_ALIEN=3,
OBJECT_PROJECTILE=4
};

struct GameObject
{
	uint8_t m_type;
	bool m_alive;
	double m_scale;
	struct DPoint2D m_lastpos;
	struct DPoint2D m_pos;
	struct DPoint2D m_modelcoords[10];
	struct DPoint2D m_worldcoords[10];
	int m_coordcount;
	double m_rotrad;
	double m_rotspeed;
	double m_movrad;
	double m_movspeed;
};

void GameObject_Initialize(struct GameObject *o);
void GameObject_Initialize_GameObject(struct GameObject *o, const struct GameObject *rhs);
void GameObject_Update(struct GameObject *o, const int ticks);
void GameObject_WrapPos(struct GameObject *o, const int w, const int h);
bool GameObject_Collision(struct GameObject *o, const struct GameObject *rhs);
bool GameObject_CollisionWrapped(struct GameObject *o, const struct GameObject *rhs, const int w, const int h);
bool GameObject_CollisionSprite(struct GameObject *o, const unsigned char *sprite, const double spritex, const double spritey, const int spritew, const int spriteh);
void GameObject_CalculateWorldCoords(struct GameObject *o);
void GameObject_Draw(struct GameObject *o);
void GameObject_DrawWrapped(struct GameObject *o, const int w, const int h);
