#include "explosion.h"
#include "game.h"
#include "wasmmath.h"
#include "wasmrand.h"

void Explosion_Initialize(struct Explosion *e)
{
	GameObject_Initialize(&e->m_obj);
	e->m_obj.m_type=OBJECT_EXPLOSION;
}

void Explosion_Update(struct Explosion *e, const int ticks)
{
	if(e->m_obj.m_alive==true)
	{
		GameObject_Update(&e->m_obj,ticks);
		e->m_ticksleft-=ticks;
		for(int i=0; i<ticks; i++)
		{
			e->m_obj.m_scale*=0.95;
		}
		if(e->m_ticksleft<=0)
		{
			e->m_obj.m_alive=false;
		}
	}
}

void Explosion_Create(struct Game *g, struct GameObject *o)
{
	for(int i=0; i<o->m_coordcount; i++)
	{
		const int ni=(i<(o->m_coordcount-1) ? i+1 : 0);
		struct Explosion *e=Game_NextFreeExplosion(g);
		if(e)
		{
			e->m_ticksleft=o->m_type==OBJECT_ASTEROID ? 20 : 60;
			e->m_obj.m_scale=1;
			e->m_obj.m_alive=true;
			e->m_obj.m_movspeed=o->m_movspeed*.5;
			e->m_obj.m_movrad=o->m_movrad;
			// explosion origin is line segment midpoint of object
			e->m_obj.m_pos.m_x=(o->m_worldcoords[i].m_x+o->m_worldcoords[ni].m_x)/2.0;
			e->m_obj.m_pos.m_y=(o->m_worldcoords[i].m_y+o->m_worldcoords[ni].m_y)/2.0;
			// add a little bit of outward movement from the center of the explosion
			const double outrad=_atan2(-(e->m_obj.m_pos.m_y-o->m_pos.m_y),(e->m_obj.m_pos.m_x-o->m_pos.m_x));
			const double dx=(_cos(outrad)*.1)+(_cos(e->m_obj.m_movrad)*e->m_obj.m_movspeed);
			const double dy=-((_sin(outrad)*.1)+(_sin(e->m_obj.m_movrad)*e->m_obj.m_movspeed));
			e->m_obj.m_movrad=_atan2(-dy,dx);
			e->m_obj.m_movspeed=_sqrt((dx*dx)+(dy*dy));
			const double len=_sqrt((o->m_worldcoords[i].m_x-o->m_worldcoords[ni].m_x)*(o->m_worldcoords[i].m_x-o->m_worldcoords[ni].m_x)+(o->m_worldcoords[i].m_y-o->m_worldcoords[ni].m_y)*(o->m_worldcoords[i].m_y-o->m_worldcoords[ni].m_y));
			double rad=_atan2(-(o->m_worldcoords[i].m_y-o->m_worldcoords[ni].m_y),(o->m_worldcoords[i].m_x-o->m_worldcoords[ni].m_x));
			e->m_obj.m_rotrad=rad;
			e->m_obj.m_rotspeed=(_drand()*0.1)-0.5;
			e->m_obj.m_coordcount=2;
			e->m_obj.m_modelcoords[0].m_x=-(len/2.0);
			e->m_obj.m_modelcoords[0].m_y=0;
			e->m_obj.m_modelcoords[1].m_x=(len/2.0);
			e->m_obj.m_modelcoords[1].m_y=0;
		}
	}
}
