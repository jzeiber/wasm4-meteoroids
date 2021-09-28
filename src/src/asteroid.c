#include "asteroid.h"
#include "wasmmath.h"
#include "wasmrand.h"
#include "wasm4.h"

void Asteroid_Initialize(struct Asteroid *a)
{
	GameObject_Initialize(&a->m_obj);
	a->m_obj.m_type=OBJECT_ASTEROID;
	a->m_onscreen=false;
}

void Asteroid_Initialize_Asteroid(struct Asteroid *a, const struct Asteroid *rhs)
{
	GameObject_Initialize_GameObject(&a->m_obj,&rhs->m_obj);
	a->m_onscreen=rhs->m_onscreen;
}

void Asteroid_Update(struct Asteroid *a, const int ticks)
{
	GameObject_Update(&a->m_obj,ticks);
}

void Asteroid_Randomize(struct Asteroid *a, bool wholescreen)
{	
	struct GameObject *o=&a->m_obj;
	double rad=0.0;
	o->m_coordcount=0;
	o->m_alive=true;
	o->m_scale=(_drand()*5.0)+3.0;

	o->m_rotrad=0;
	o->m_rotspeed=(_drand()*0.08)-0.04;
	
	if(wholescreen==true)	// spawn anywhere on screen
	{
		a->m_onscreen=true;
		FPoint2D_SetPos(&o->m_pos,_drand()*(double)SCREEN_SIZE,_drand()*(double)SCREEN_SIZE);
		o->m_movrad=_drand()*2.0*M_PI;
		o->m_movspeed=(_drand()*0.5)+0.1;
	}
	else	// spawn at edge of screen
	{
		a->m_onscreen=false;
		int pos=_drand()*2.0;
		
		double x=_drand()*SCREEN_SIZE;
		double y=_drand()*SCREEN_SIZE;
		
		if(pos==0)
		{
			x=(_drand()*10.0)+10.0;
			x=(_drand()<0.5 ? x : SCREEN_SIZE-x);
		}
		else
		{
			y=(_drand()*10.0)+10.0;
			y=(_drand()<0.5 ? y : SCREEN_SIZE-y);
		}
		

		FPoint2D_SetPos(&o->m_pos,x,y);
		o->m_movspeed=(_drand()*0.5)+0.1;
		o->m_movrad=_atan2(-((_drand()*(double)SCREEN_SIZE)-y),(_drand()*(double)SCREEN_SIZE)-x);
	}
	
	for(int i=0; i<10; i++)
	{
		if(rad<=2.0*M_PI)
		{
			double dist=(_drand()*0.4)+0.5;

			FPoint2D_SetPos(&o->m_modelcoords[i],dist*_cos(rad),dist*_sin(rad));
			o->m_coordcount++;
			
			rad+=(_drand()/3.0)+0.45;
		}
	}
}

bool Asteroid_CheckOnScreen(struct Asteroid *a)
{
	if(a->m_obj.m_pos.m_x>=0 && a->m_obj.m_pos.m_x<SCREEN_SIZE && a->m_obj.m_pos.m_y>=0 && a->m_obj.m_pos.m_y<SCREEN_SIZE)
	{
		return true;
	}
	
	for(int i=0; i<a->m_obj.m_coordcount; i++)
	{
		if(a->m_obj.m_worldcoords[i].m_x>=0 && a->m_obj.m_worldcoords[i].m_x<SCREEN_SIZE && a->m_obj.m_worldcoords[i].m_y>=0 && a->m_obj.m_worldcoords[i].m_y<SCREEN_SIZE)
		{
			return true;
		}
	}
	
	return false;
}
