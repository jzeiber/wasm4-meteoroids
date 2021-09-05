#include "title.h"
#include "wasmrand.h"
#include "wasm4.h"
#include "printf.h"
#include "global.h"

void Title_Initialize(struct Title *t, struct Game *g)
{
	t->m_blinkcount=0;
	t->m_blinkon=true;

	_srand(g->m_ticks ^ *MOUSE_X ^ *MOUSE_Y);
	for(int i=0; i<50 && i<MAX_ASTEROIDS; i++)
	{
		Asteroid_Initialize(&g->m_asteroids[i]);
		Asteroid_Randomize(&g->m_asteroids[i],true);		
	}
	for(int i=50; i<MAX_ASTEROIDS; i++)
	{
		g->m_asteroids[i].m_obj.m_alive=false;
	}
}

void Title_Update(struct Title *t, struct Game *g, const int ticks)
{
	t->m_blinkcount++;
	if(t->m_blinkcount>19)
	{
		t->m_blinkcount=0;
		t->m_blinkon=!t->m_blinkon;
	}
	
	for(int i=0; i<MAX_ASTEROIDS; i++)
	{
		if(g->m_asteroids[i].m_obj.m_alive)
		{
			Asteroid_Update(&g->m_asteroids[i],ticks);
			GameObject_CalculateWorldCoords(&g->m_asteroids[i].m_obj);
			GameObject_WrapPos(&g->m_asteroids[i].m_obj,SCREEN_SIZE,SCREEN_SIZE);
		}
	}
}

void Title_Draw(struct Title *t, struct Game *g)
{
	*DRAW_COLORS=0x02;
	for(int i=0; i<MAX_ASTEROIDS; i++)
	{
		if(g->m_asteroids[i].m_obj.m_alive)
		{
			GameObject_DrawWrapped(&g->m_asteroids[i].m_obj,SCREEN_SIZE,SCREEN_SIZE);
		}
	}
	
	*DRAW_COLORS=0x04;
	text("Meteoroids",40,30);

	if(t->m_blinkon==true)
	{
		text("click mouse",36,60);
		text("or press X",40,70);
		text("to start",48,80);
	}
	
	text("High Score",40,100);
	snprintf(buff,64,"%d",g->m_highscore);
	text(buff,40,110);
	
	text("v0.2",SCREEN_SIZE-(4*8)-2,SCREEN_SIZE-10);
}
