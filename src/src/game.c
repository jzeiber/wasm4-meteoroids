#include "game.h"
#include "wasm4.h"
#include "wasmrand.h"
#include "wasmmath.h"
#include "sound.h"
#include "global.h"
#include "printf.h"

#include <stddef.h>
#include <float.h>

void Game_Initialize(struct Game *g)
{
	g->m_state=STATE_STARTUP;
	g->m_playstate=STATE_WAIT;
	g->m_waitticks=0;
	g->m_ticks=0;
	g->m_score=0;
	g->m_highscore=0;
	g->m_tickssinceshot=0;
	g->m_tickssincelastasteroid=0;
	g->m_tickssinceufo=0;
	g->m_lives=0;
	
	// read high score from file
	{
		if(diskr(buff,12)==12)
		{
			// magic bytes
			if(buff[0]=='M' && buff[1]=='S' && buff[2]=='G' && buff[3]=='F')
			{
				for(int b=0; b<8; b++)
				{
					g->m_highscore|=(((int64_t)buff[4+b]) << ((8-b)*8));
				}
			}
		}
	}
	
	Input_Initialize(&g->m_input);
	Spaceship_Initialize(&g->m_player);
	UFO_Initialize(&g->m_ufo);
	
	for(int i=0; i<MAX_ASTEROIDS; i++)
	{
		Asteroid_Initialize(&g->m_asteroids[i]);
	}
	
	for(int i=0; i<MAX_PROJECTILES; i++)
	{
		Projectile_Initialize(&g->m_projectiles[i]);
	}
}

void Game_Update(struct Game *g, const int ticks)
{
	g->m_ticks++;
	g->m_tickssinceshot++;
	g->m_tickssincelastasteroid++;
	g->m_tickssinceufo++;
	if(g->m_playstate==STATE_WAIT)
	{
		g->m_waitticks++;
	}
	
	Input_Update(&g->m_input);
	
	// update state
	switch(g->m_state)
	{
	case STATE_STARTUP:
		Title_Initialize(&title,g);
		g->m_state=STATE_TITLE;
	break;
	case STATE_TITLE:
		if(Input_MouseButtonClick(&g->m_input,1)==true || Input_GamepadButtonPress(&g->m_input,1))
		{
			// TODO - initialize new game
			Game_NewGame(g);
			g->m_state=STATE_GAME;
			g->m_playstate=STATE_WAIT;
			g->m_waitticks=0;
			// reset last buttons so we don't fire right away
			g->m_input.m_lastmousebuttons=g->m_input.m_mousebuttons;
			g->m_input.m_lastgamepad=g->m_input.m_gamepad;
			g->m_tickssinceshot=30;
		}
		else
		{
			Title_Update(&title,g,1);
		}
	break;
	case STATE_GAME:
		// TODO - handle input, update objects
		
		if(g->m_playstate==STATE_PLAY && (Input_MouseButtonDown(&g->m_input,1)==true || Input_GamepadButtonDown(&g->m_input,1)==true) && g->m_tickssinceshot>=30)
		{
			// button pressed - create projectile
			struct Projectile *p=Game_NextFreeProjectile(g);
			p->m_obj.m_alive=true;
			p->m_ticks=0;
			p->m_maxticks=80;
			DPoint2D_Initialize_DPoint2D(&p->m_obj.m_pos,&g->m_player.m_obj.m_pos);
			// move points to front of player (model coord 0 is front of ship)
			double dx=_cos(g->m_player.m_obj.m_rotrad)*g->m_player.m_obj.m_modelcoords[0].m_x;
			double dy=-(_sin(g->m_player.m_obj.m_rotrad)*g->m_player.m_obj.m_modelcoords[0].m_x);
			p->m_obj.m_pos.m_x+=dx;
			p->m_obj.m_pos.m_y+=dy;
			// set last pos to current pos
			DPoint2D_Initialize_DPoint2D(&p->m_obj.m_lastpos,&p->m_obj.m_pos);
			
			p->m_obj.m_movrad=g->m_player.m_obj.m_rotrad;
			p->m_obj.m_movspeed=1.8;
			p->m_owner=OWNER_PLAYER;
			g->m_tickssinceshot=0;
			
			// TODO - play sound
			Sound_PlayerFireProjectile();
		}
		else if(g->m_playstate==STATE_WAIT && (g->m_waitticks>120 || (Input_MouseButtonClick(&g->m_input,1)==true || Input_GamepadButtonPress(&g->m_input,1)==true)))
		{
			g->m_playstate=STATE_PLAY;
		}
		
		if(g->m_playstate==STATE_PLAY)
		{
			// rotate ship
			if(g->m_input.m_inputtype==INPUT_GAMEPAD)
			{
				if(Input_GamepadLeftDown(&g->m_input)==true)
				{
					g->m_player.m_obj.m_rotrad+=0.1;
				}
				else if(Input_GamepadRightDown(&g->m_input)==true)
				{
					g->m_player.m_obj.m_rotrad-=0.1;
				}
				// normalize rotation angle 0 to 2PI
				while(g->m_player.m_obj.m_rotrad<0)
				{
					g->m_player.m_obj.m_rotrad+=(2.0*M_PI);
				}
				while(g->m_player.m_obj.m_rotrad>=(2.0*M_PI))
				{
					g->m_player.m_obj.m_rotrad-=(2.0*M_PI);
				}
			}
			else
			{
				double rad=_atan2(-(*MOUSE_Y-g->m_player.m_obj.m_pos.m_y),*MOUSE_X-g->m_player.m_obj.m_pos.m_x);
				if(rad<DBL_MAX && rad>-DBL_MAX)
				{
					g->m_player.m_obj.m_rotrad=rad;
				}
			}
			
			// thrust ship
			if(Input_GamepadUpDown(&g->m_input)==true || Input_MouseButtonDown(&g->m_input,2)==true)
			{
				double cx=g->m_player.m_obj.m_movspeed*_cos(g->m_player.m_obj.m_movrad);
				double cy=g->m_player.m_obj.m_movspeed*_sin(g->m_player.m_obj.m_movrad);
				double nx=0.02*_cos(g->m_player.m_obj.m_rotrad);
				double ny=0.02*_sin(g->m_player.m_obj.m_rotrad);
				double fx=cx+nx;
				double fy=cy+ny;
				double speed=_sqrt((fx*fx)+(fy*fy));
				if(speed>1.0)
				{
					speed=1.0;
				}
				double rad=_atan2(fy,fx);
				g->m_player.m_obj.m_movrad=rad;
				g->m_player.m_obj.m_movspeed=speed;
			}
			else if(g->m_player.m_obj.m_movspeed>0.0)
			{
				g->m_player.m_obj.m_movspeed=(g->m_player.m_obj.m_movspeed*0.99)-0.01;
				if(g->m_player.m_obj.m_movspeed<0)
				{
					g->m_player.m_obj.m_movspeed=0;
				}
			}

			// update objects
			GameObject_Update(&g->m_player.m_obj,ticks);
			GameObject_CalculateWorldCoords(&g->m_player.m_obj);
			GameObject_WrapPos(&g->m_player.m_obj,SCREEN_SIZE,SCREEN_SIZE);
			
			int asteroidsalive=0;
			for(int i=0; i<MAX_ASTEROIDS; i++)
			{
				if(g->m_asteroids[i].m_obj.m_alive==true)
				{
					asteroidsalive++;
				}
			}

			// check ticks since last asteroid and spawn a new one (check +/- 20 pixels around ship) and regen until outside range
			// TODO - check colliding projectiles as well
			if(g->m_tickssincelastasteroid>10 && (asteroidsalive<10 || (g->m_tickssincelastasteroid>(300-(g->m_score/1000)))))
			{
				struct Asteroid *a=Game_NextFreeAsteroid(g);
				if(a!=NULL)
				{
					bool collided=false;
					int tries=0;
					do
					{
						Asteroid_Randomize(a,false);
						if((_dabs(a->m_obj.m_pos.m_x - g->m_player.m_obj.m_pos.m_x) < 20) && (_dabs(a->m_obj.m_pos.m_y - g->m_player.m_obj.m_pos.m_y) < 20))
						{
							collided=true;
						}
						else
						{
							for(int i=0; i<MAX_PROJECTILES; i++)
							{
								if(g->m_projectiles[i].m_obj.m_alive==true && GameObject_Collision(&g->m_projectiles[i].m_obj,&a->m_obj)==true)
								{
									collided=true;
								}
							}
						}

					}while(collided==true && tries++<100);
				}
				g->m_tickssincelastasteroid=0;
			}
			
			//if(g->m_tickssinceufo>=600 && g->m_ufo.m_obj.m_alive==false && _drand()<0.001)
			if(g->m_tickssinceufo>=1200 && g->m_ufo.m_obj.m_alive==false && _drand()<0.005)
			{
				UFO_Randomize(&g->m_ufo);
				g->m_tickssinceufo=0;
			}
			
			for(int i=0; i<MAX_ASTEROIDS; i++)
			{
				if(g->m_asteroids[i].m_obj.m_alive)
				{
					Asteroid_Update(&g->m_asteroids[i],ticks);
					GameObject_CalculateWorldCoords(&g->m_asteroids[i].m_obj);
					if(g->m_asteroids[i].m_onscreen==false)
					{
						if(Asteroid_CheckOnScreen(&g->m_asteroids[i])==true)
						{
							g->m_asteroids[i].m_onscreen=true;
						}
					}
					if(g->m_asteroids[i].m_onscreen==true)
					{
						GameObject_WrapPos(&g->m_asteroids[i].m_obj,SCREEN_SIZE,SCREEN_SIZE);
					}
				}
			}
			
			for(int i=0; i<MAX_PROJECTILES; i++)
			{
				if(g->m_projectiles[i].m_obj.m_alive)
				{
					Projectile_Update(&g->m_projectiles[i],ticks);
					GameObject_CalculateWorldCoords(&g->m_projectiles[i].m_obj);
					GameObject_WrapPos(&g->m_projectiles[i].m_obj,SCREEN_SIZE,SCREEN_SIZE);
				}
			}
			
			// update ufo
			
			UFO_Update(g,&g->m_ufo,ticks);
			GameObject_CalculateWorldCoords(&g->m_ufo.m_obj);
			
			// detect collisions
			
			for(int i=0; i<MAX_PROJECTILES; i++)
			{
				if(g->m_projectiles[i].m_obj.m_alive==true)
				{	
					for(int j=0; j<MAX_ASTEROIDS && g->m_projectiles[i].m_obj.m_alive==true; j++)
					{
						if(g->m_asteroids[j].m_obj.m_alive==true)
						{
							bool collide=false;
							if(GameObject_CollisionWrapped(&g->m_projectiles[i].m_obj,&g->m_asteroids[j].m_obj,SCREEN_SIZE,SCREEN_SIZE)==true)
							{
								collide=true;
							}
							else
							{
								// check line collision - last projectile pos and this pos against asteroid pos
								struct GameObject go;
								go.m_alive=true;
								go.m_coordcount=2;
								DPoint2D_Initialize_DPoint2D(&go.m_worldcoords[0],&g->m_projectiles[i].m_obj.m_lastpos);
								DPoint2D_Initialize_DPoint2D(&go.m_worldcoords[1],&g->m_projectiles[i].m_obj.m_pos);
								if(GameObject_CollisionWrapped(&go,&g->m_asteroids[j].m_obj,SCREEN_SIZE,SCREEN_SIZE)==true)
								{
									collide=true;
								}
							}
							if(collide==true)
							{
								// break apart large asteroid or destroy smaller one and add to score

								Sound_DestroyAsteroid();
								
								g->m_asteroids[j].m_obj.m_alive=false;
								g->m_projectiles[i].m_obj.m_alive=false;
								if(g->m_projectiles[i].m_owner==OWNER_PLAYER)
								{
									g->m_score+=100;
								}
								if(g->m_asteroids[j].m_obj.m_scale>=5.0)
								{
									double origrad=g->m_asteroids[j].m_obj.m_movrad;
									g->m_asteroids[j].m_obj.m_alive=true;
									g->m_asteroids[j].m_obj.m_scale/=2.0;
									g->m_asteroids[j].m_obj.m_movspeed*=1.2;
									g->m_asteroids[j].m_obj.m_movrad=origrad+((_drand()*M_PI_4)+(M_PI_4));	// 45 to 90 degrees right
									while(g->m_asteroids[j].m_obj.m_movrad<0)
									{
										g->m_asteroids[j].m_obj.m_movrad+=(2.0*M_PI);
									}
									GameObject_CalculateWorldCoords(&g->m_asteroids[j].m_obj);
									
									struct Asteroid *a=Game_NextFreeAsteroid(g);
									if(a)
									{
										Asteroid_Initialize_Asteroid(a,&g->m_asteroids[j]);
										a->m_obj.m_movrad=origrad-((_drand()*M_PI_4)+(M_PI_4));	// 45 to 90 degrees left
										while(a->m_obj.m_movrad>=(2.0*M_PI))
										{
											a->m_obj.m_movrad-=(2.0*M_PI);
										}
										GameObject_CalculateWorldCoords(&a->m_obj);
									}
								}
							}

						}
					}
					
					// check projectile collision with player
					// TODO - ignore own projectiles??
					if(g->m_projectiles[i].m_obj.m_alive==true && GameObject_CollisionWrapped(&g->m_projectiles[i].m_obj,&g->m_player.m_obj,SCREEN_SIZE,SCREEN_SIZE)==true)
					{
						Game_HandlePlayerCollision(g,&g->m_projectiles[i].m_obj);
						return;
					}
					
					// check projectile collision with ufo
					if(g->m_projectiles[i].m_obj.m_alive==true && g->m_projectiles[i].m_owner!=OWNER_UFO && GameObject_Collision(&g->m_projectiles[i].m_obj,&g->m_ufo.m_obj)==true)
					{
						// TODO - play sound
						g->m_projectiles[i].m_obj.m_alive=false;
						g->m_score+=1000;
						g->m_ufo.m_obj.m_alive=false;
						g->m_tickssinceufo=0;
					}
					
				}
			}
			
			// check collision - player and asteroids
			for(int i=0; i<MAX_ASTEROIDS; i++)
			{
				if(g->m_asteroids[i].m_obj.m_alive==true)
				{
					if(GameObject_CollisionWrapped(&g->m_asteroids[i].m_obj,&g->m_player.m_obj,SCREEN_SIZE,SCREEN_SIZE)==true)
					{
						Game_HandlePlayerCollision(g,&g->m_asteroids[i].m_obj);
						return;
					}
				}
			}
			// check collision - player and ufo
			if(GameObject_Collision(&g->m_player.m_obj,&g->m_ufo.m_obj)==true)
			{
				Game_HandlePlayerCollision(g,&g->m_ufo.m_obj);
				return;
			}
			
		}
	break;
	case STATE_GAMEOVER:
		
	break;
	}
	
}

void Game_Draw(struct Game *g)
{
	// draw state
	switch(g->m_state)
	{
	case STATE_TITLE:
		Title_Draw(&title,g);
		break;
	case STATE_GAME:
		*DRAW_COLORS=0x02;
		for(int i=0; i<MAX_ASTEROIDS; i++)
		{
			if(g->m_asteroids[i].m_obj.m_alive==true)
			{
				GameObject_Draw(&g->m_asteroids[i].m_obj);
			}
		}
		*DRAW_COLORS=0x03;
		for(int i=0; i<MAX_PROJECTILES; i++)
		{
			if(g->m_projectiles[i].m_obj.m_alive==true)
			{
				GameObject_Draw(&g->m_projectiles[i].m_obj);
				line(g->m_projectiles[i].m_obj.m_lastpos.m_x,g->m_projectiles[i].m_obj.m_lastpos.m_y,g->m_projectiles[i].m_obj.m_pos.m_x,g->m_projectiles[i].m_obj.m_pos.m_y);
			}
		}
		*DRAW_COLORS=0x03;
		GameObject_Draw(&g->m_player.m_obj);
		
		if(g->m_ufo.m_obj.m_alive==true)
		{
			UFO_Draw(&g->m_ufo);
		}
		
		snprintf(buff,64,"%d",g->m_score);
		text(buff,5,5);
		
		*DRAW_COLORS=0x30;
		for(int i=1; i<=g->m_lives; i++)
		{
			blit((char *)shipsprite,(SCREEN_SIZE-(i*10))-5,5,8,8,BLIT_1BPP);
		}
		
		break;
	case STATE_GAMEOVER:
		
		break;
	}
}

void Game_NewGame(struct Game *g)
{
	_srand(g->m_ticks);
	g->m_score=0;
	g->m_tickssinceufo=0;
	g->m_player.m_obj.m_alive=true;
	DPoint2D_Initialize_XY(&g->m_player.m_obj.m_pos,SCREEN_SIZE/2.0,SCREEN_SIZE/2.0);
	g->m_player.m_obj.m_rotrad=_drand()*2.0*M_PI;
	g->m_player.m_obj.m_rotspeed=0;
	g->m_player.m_obj.m_movrad=0;
	g->m_player.m_obj.m_movspeed=0;
	g->m_lives=3;
	
	GameObject_CalculateWorldCoords(&g->m_player.m_obj);
	
	// reset projectiles
	for(int i=0; i<MAX_PROJECTILES; i++)
	{
		g->m_projectiles[i].m_obj.m_alive=false;
	}
	
	// reset asteroids
	for(int i=0; i<10 && i<MAX_ASTEROIDS; i++)
	{
		Asteroid_Randomize(&g->m_asteroids[i],false);
		GameObject_CalculateWorldCoords(&g->m_asteroids[i].m_obj);
		GameObject_WrapPos(&g->m_asteroids[i].m_obj,SCREEN_SIZE,SCREEN_SIZE);
	}
	for(int i=10; i<MAX_ASTEROIDS; i++)
	{
		g->m_asteroids[i].m_obj.m_alive=false;
	}
	
	// reset ufo
	g->m_ufo.m_obj.m_alive=false;
	
}

struct Projectile *Game_NextFreeProjectile(struct Game *g)
{
	for(int i=0; i<MAX_PROJECTILES; i++)
	{
		if(g->m_projectiles[i].m_obj.m_alive==false)
		{
			return &g->m_projectiles[i];
		}
	}
	if(MAX_PROJECTILES>0)
	{
		// no dead projectiles, return oldest projectile
		struct Projectile *p=&g->m_projectiles[0];
		for(int i=1; i<MAX_PROJECTILES; i++)
		{
			if(g->m_projectiles[i].m_ticks>p->m_ticks)
			{
				p=&g->m_projectiles[i];
			}
		}
		return p;
	}
	return NULL;
}

struct Asteroid *Game_NextFreeAsteroid(struct Game *g)
{
	for(int i=0; i<MAX_ASTEROIDS; i++)
	{
		if(g->m_asteroids[i].m_obj.m_alive==false)
		{
			return &g->m_asteroids[i];
		}
	}
	return NULL;
}

void Game_ResetStateAndWait(struct Game *g)
{
	g->m_playstate=STATE_WAIT;
	g->m_waitticks=0;
	g->m_input.m_lastmousebuttons=g->m_input.m_mousebuttons;
	g->m_input.m_lastgamepad=g->m_input.m_gamepad;

	for(int i=0; i<MAX_PROJECTILES; i++)
	{
		g->m_projectiles[i].m_obj.m_alive=false;
	}
	
	bool collide=false;
	int tries=0;
	int radius=10;
	do
	{
		// try to place close to center of screen, growing radius for each iteration until it fits without collision
		collide=false;
		g->m_player.m_obj.m_pos.m_x=((double)SCREEN_SIZE/2.0)+(_drand()*(double)radius*2.0)-(double)radius;
		g->m_player.m_obj.m_pos.m_y=((double)SCREEN_SIZE/2.0)+(_drand()*(double)radius*2.0)-(double)radius;
		g->m_player.m_obj.m_rotrad=_drand()*2.0*M_PI;
		g->m_player.m_obj.m_movspeed=0;
		GameObject_CalculateWorldCoords(&g->m_player.m_obj);
		for(int i=0; i<MAX_ASTEROIDS && collide==false; i++)
		{
			if(g->m_asteroids[i].m_obj.m_alive==true)
			{
				collide=GameObject_CollisionWrapped(&g->m_player.m_obj,&g->m_asteroids[i].m_obj,SCREEN_SIZE,SCREEN_SIZE);
			}
			// also check radius - keep outside 10 units of center of asteroid
			if(_dabs(g->m_asteroids[i].m_obj.m_pos.m_x-g->m_player.m_obj.m_pos.m_x)<10 && _dabs(g->m_asteroids[i].m_obj.m_pos.m_y-g->m_player.m_obj.m_pos.m_y)<10)
			{
				collide=true;
			}
		}
		if(radius<(SCREEN_SIZE/2.0))
		{
			radius+=10;
		}
	}while(collide==true && tries++<200);
	
	// if we exhausted placement of ship tries - move all asteroids to sides and place player in center of screen
	if(tries>=200)
	{
		g->m_player.m_obj.m_pos.m_x=((double)SCREEN_SIZE/2.0);
		g->m_player.m_obj.m_pos.m_y=((double)SCREEN_SIZE/2.0);
		GameObject_CalculateWorldCoords(&g->m_player.m_obj);
		for(int i=0; i<MAX_ASTEROIDS; i++)
		{
			if(g->m_asteroids[i].m_obj.m_alive==true)
			{
				Asteroid_Randomize(&g->m_asteroids[i],false);
			}
		}
	}

	g->m_tickssinceufo=0;
	g->m_ufo.m_obj.m_alive=false;
}

void Game_HandlePlayerCollision(struct Game *g, struct GameObject *o)
{
	o;	// unused for now;

	Sound_PlayerCollide();
	
	g->m_lives--;
	if(g->m_lives>=0)
	{
		// reset state and place ship
		Game_ResetStateAndWait(g);
	}
	else
	{
		// save high score
		if(g->m_score>g->m_highscore)
		{
			g->m_highscore=g->m_score;

			buff[0]='M';
			buff[1]='S';
			buff[2]='G';
			buff[3]='F';
			for(int b=0; b<8; b++)
			{
				buff[4+b]=(g->m_highscore >> ((8-b)*8)) & 0xff;
			}
			diskw(buff,12);
		}
		Title_Initialize(&title,g);
		g->m_state=STATE_TITLE;
	}
}
