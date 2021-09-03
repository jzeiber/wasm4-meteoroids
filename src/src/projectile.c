#include "projectile.h"

void Projectile_Initialize(struct Projectile *p)
{
	GameObject_Initialize(&p->m_obj);
	p->m_obj.m_type=OBJECT_PROJECTILE;
	p->m_owner=0;
	p->m_ticks=0;
	p->m_maxticks=0;
	
	p->m_obj.m_coordcount=1;
	DPoint2D_Initialize_XY(&p->m_obj.m_modelcoords[0],0,0);
}

void Projectile_Update(struct Projectile *p, const int ticks)
{
	GameObject_Update(&p->m_obj,ticks);
	p->m_ticks++;
	if(p->m_ticks>p->m_maxticks)
	{
		p->m_obj.m_alive=false;
	}
}
