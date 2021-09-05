#include "ufo.h"
#include "wasm4.h"
#include "game.h"
#include "gameobject.h"
#include "wasmrand.h"
#include "wasmmath.h"
#include "sound.h"

#include <stdbool.h>

void UFO_Initialize(struct UFO *a)
{
	GameObject_Initialize(&a->m_obj);
	
	a->m_ticksalive=0;
	a->m_tickssinceshot=0;
	a->m_tickssincechangedir=0;
	
	a->m_obj.m_scale=1.0;
	a->m_obj.m_coordcount=10;
	FPoint2D_Initialize_XY(&a->m_obj.m_modelcoords[0], 1.5, 2.0);
	FPoint2D_Initialize_XY(&a->m_obj.m_modelcoords[1], 3.5, 0.0);
	FPoint2D_Initialize_XY(&a->m_obj.m_modelcoords[2], 5.5, 0.0);
	FPoint2D_Initialize_XY(&a->m_obj.m_modelcoords[3], 6.5,-1.0);
	FPoint2D_Initialize_XY(&a->m_obj.m_modelcoords[4], 3.5,-4.0);
	FPoint2D_Initialize_XY(&a->m_obj.m_modelcoords[5],-3.5,-4.0);
	FPoint2D_Initialize_XY(&a->m_obj.m_modelcoords[6],-6.5,-1.0);
	FPoint2D_Initialize_XY(&a->m_obj.m_modelcoords[7],-5.5, 0.0);
	FPoint2D_Initialize_XY(&a->m_obj.m_modelcoords[8],-3.5, 0.0);
	FPoint2D_Initialize_XY(&a->m_obj.m_modelcoords[9],-1.5, 2.0);
}

void UFO_Randomize(struct UFO *u)
{
	u->m_obj.m_alive=true;
	u->m_tickssinceshot=0;
	u->m_ticksalive=0;
	u->m_tickssincechangedir=0;
	
	int pos=_drand()*2.0;
	
	double x=_drand()*SCREEN_SIZE;
	double y=_drand()*SCREEN_SIZE;
	
	if(pos==0)
	{
		x=(_drand()*10.0)+10.0;
		x=(_drand()<0.5 ? -x : SCREEN_SIZE+x);
	}
	else
	{
		y=(_drand()*10.0)+10.0;
		y=(_drand()<0.5 ? -y : SCREEN_SIZE+y);
	}
	
	FPoint2D_SetPos(&u->m_obj.m_pos,x,y);
	FPoint2D_SetPos(&u->m_obj.m_lastpos,x,y);
	
	u->m_obj.m_movrad=_atan2(-((_drand()*(double)SCREEN_SIZE)-y),(_drand()*(double)SCREEN_SIZE)-x);
	u->m_obj.m_movspeed=(_drand()*0.4)+0.3;

}

void UFO_Update(struct Game *g, struct UFO *a, const int ticks)
{
	if(a->m_obj.m_alive==true)
	{
		GameObject_Update(&a->m_obj,ticks);
		a->m_tickssinceshot++;
		a->m_ticksalive++;
		a->m_tickssincechangedir++;
		
		bool onscreen=false;
		if(a->m_obj.m_pos.m_x>=0 && a->m_obj.m_pos.m_x<SCREEN_SIZE && a->m_obj.m_pos.m_y>=0 && a->m_obj.m_pos.m_y<SCREEN_SIZE)
		{
			onscreen=true;
		}
		
		if(a->m_tickssinceshot>=40 && onscreen==true && _drand()<0.02)
		{
			a->m_tickssinceshot=0;
			struct Projectile *p=Game_NextFreeProjectile(g);
			FPoint2D_Initialize_FPoint2D(&p->m_obj.m_pos,&a->m_obj.m_pos);
			FPoint2D_Initialize_FPoint2D(&p->m_obj.m_lastpos,&a->m_obj.m_pos);
			p->m_obj.m_alive=true;
			p->m_ticks=0;
			p->m_maxticks=60;
			p->m_owner=OWNER_UFO;
			// 10% chance to fire directly at player
			if(_drand()<0.1)
			{
				p->m_obj.m_movrad=_atan2(-(g->m_player.m_obj.m_pos.m_y-p->m_obj.m_pos.m_y),g->m_player.m_obj.m_pos.m_x-p->m_obj.m_pos.m_x);
			}
			else
			{
				p->m_obj.m_movrad=_drand()*2.0*M_PI;
			}
			p->m_obj.m_movspeed=1.8;
			GameObject_CalculateWorldCoords(&p->m_obj);
			Sound_UFOFireProjectile();
		}
		
		// chance to change dir
		if(a->m_tickssincechangedir>=40 && _drand()<0.01)
		{
			a->m_obj.m_movrad=_atan2(-((_drand()*(double)SCREEN_SIZE)-a->m_obj.m_pos.m_y),(_drand()*(double)SCREEN_SIZE)-a->m_obj.m_pos.m_x);
			a->m_obj.m_movspeed=(_drand()*0.4)+0.3;
			a->m_tickssincechangedir=0;
		}
		
		// off screen and enough time has passed - turn off
		if(a->m_ticksalive>600 && onscreen==false)
		{
			a->m_obj.m_alive=false;
			a->m_ticksalive=0;
			g->m_tickssinceufo=0;
		}
	}
	
}

void UFO_Draw(struct UFO *a)
{
	GameObject_Draw(&a->m_obj);
	line(a->m_obj.m_worldcoords[1].m_x-1,a->m_obj.m_worldcoords[1].m_y+1,a->m_obj.m_worldcoords[8].m_x+1,a->m_obj.m_worldcoords[1].m_y+1);
}
