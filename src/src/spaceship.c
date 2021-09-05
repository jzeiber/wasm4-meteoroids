#include "spaceship.h"
#include "point.h"

void Spaceship_Initialize(struct Spaceship *s)
{
	struct GameObject *o=&s->m_obj;
	GameObject_Initialize(o);
	o->m_type=OBJECT_PLAYER;
	
	o->m_coordcount=3;
	FPoint2D_Initialize_XY(&o->m_modelcoords[0],3.5,0.0);
	FPoint2D_Initialize_XY(&o->m_modelcoords[1],-3.5,2.5);
	FPoint2D_Initialize_XY(&o->m_modelcoords[2],-3.5,-2.5);
	
	FPoint2D_Initialize_XY(&o->m_pos,0,0);
}
