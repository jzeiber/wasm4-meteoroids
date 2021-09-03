#include "gameobject.h"
#include "line.h"
#include "wasmmath.h"
#include "miscfuncs.h"
#include "wasm4.h"

void GameObject_Initialize(struct GameObject *o)
{
	o->m_type=OBJECT_NONE;
	o->m_alive=false;
	o->m_scale=1.0;
	DPoint2D_Initialize(&o->m_lastpos);
	DPoint2D_Initialize(&o->m_pos);
	o->m_coordcount=0;
	o->m_rotrad=0;
	o->m_rotspeed=0;
	o->m_movrad=0;
	o->m_movspeed=0;
	for(int i=0; i<10; i++)
	{
		DPoint2D_Initialize(&o->m_modelcoords[i]);
		DPoint2D_Initialize(&o->m_worldcoords[i]);
	}
}

void GameObject_Initialize_GameObject(struct GameObject *o, const struct GameObject *rhs)
{
	o->m_type=rhs->m_type;
	o->m_alive=rhs->m_alive;
	o->m_scale=rhs->m_scale;
	DPoint2D_Initialize_DPoint2D(&o->m_lastpos,&rhs->m_lastpos);
	DPoint2D_Initialize_DPoint2D(&o->m_pos,&rhs->m_pos);
	o->m_coordcount=rhs->m_coordcount;
	o->m_rotrad=rhs->m_rotrad;
	o->m_rotspeed=rhs->m_rotspeed;
	o->m_movrad=rhs->m_movrad;
	o->m_movspeed=rhs->m_movspeed;
	for(int i=0; i<10; i++)
	{
		DPoint2D_Initialize_DPoint2D(&o->m_modelcoords[i],&rhs->m_modelcoords[i]);
		DPoint2D_Initialize_DPoint2D(&o->m_worldcoords[i],&rhs->m_worldcoords[i]);
	}
}

void GameObject_Update(struct GameObject *o, const int ticks)
{
	if(o->m_alive==false)
	{
		return;
	}
	
	DPoint2D_Initialize_DPoint2D(&o->m_lastpos,&o->m_pos);
	
	double dx=_cos(o->m_movrad)*o->m_movspeed*(double)ticks;
	double dy=-(_sin(o->m_movrad)*o->m_movspeed*(double)ticks);	// y reversed with respect to screen drawing	

	DPoint2D_OffsetPos(&o->m_pos,dx,dy);
	
	//rotate and normalize
	o->m_rotrad+=(o->m_rotspeed*(double)ticks);
	while(o->m_rotrad<0)
	{
		o->m_rotrad+=(2*M_PI);
	}
	while(o->m_rotrad>(2*M_PI))
	{
		o->m_rotrad-=(2*M_PI);
	}
	
}

void GameObject_WrapPos(struct GameObject *o, const int w, const int h)
{
	while(o->m_pos.m_x<0)
	{
		DPoint2D_OffsetPos(&o->m_pos,w,0);
		DPoint2D_OffsetPos(&o->m_lastpos,w,0);
	}
	while(o->m_pos.m_x>=w)
	{
		DPoint2D_OffsetPos(&o->m_pos,-w,0);
		DPoint2D_OffsetPos(&o->m_lastpos,-w,0);
	}
	while(o->m_pos.m_y<0)
	{
		DPoint2D_OffsetPos(&o->m_pos,0,h);
		DPoint2D_OffsetPos(&o->m_lastpos,0,h);
	}
	while(o->m_pos.m_y>=h)
	{
		DPoint2D_OffsetPos(&o->m_pos,0,-h);
		DPoint2D_OffsetPos(&o->m_lastpos,0,-h);
	}
}

bool GameObject_Collision(struct GameObject *o, const struct GameObject *rhs)
{
	if(o->m_alive==false || rhs->m_alive==false || o->m_coordcount<=0 || rhs->m_coordcount<=0)
	{
		return false;
	}
	
	// point and point - check if equal
	if(o->m_coordcount==1 && rhs->m_coordcount==1)
	{
		if(_dabs(o->m_worldcoords[0].m_x-rhs->m_worldcoords[0].m_x)<=0.5 && _dabs(o->m_worldcoords[0].m_y-rhs->m_worldcoords[0].m_y)<=0.5)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	// point and line
	if((o->m_coordcount==1 && rhs->m_coordcount==2) || (o->m_coordcount==2 && rhs->m_coordcount==1))
	{
		const struct DPoint2D *p1, *p2, *p3;
		if(o->m_coordcount==1)
		{
			p1=&rhs->m_worldcoords[0];
			p2=&o->m_worldcoords[0];
			p3=&rhs->m_worldcoords[1];
		}
		else
		{
			p1=&o->m_worldcoords[0];
			p2=&rhs->m_worldcoords[0];
			p3=&o->m_worldcoords[1];
		}
		return Line_OnSegment(p1,p2,p3);
	}
	
	// point and polygon
	if((o->m_coordcount==1 && rhs->m_coordcount>2) || (o->m_coordcount>2 && rhs->m_coordcount==1))
	{
		const struct DPoint2D *p1;
		const struct GameObject *obj;
		
		if(o->m_coordcount==1)
		{
			p1=&o->m_worldcoords[0];
			obj=rhs;
		}
		else
		{
			p1=&rhs->m_worldcoords[0];
			obj=o;
		}
		
		for(int i=0; i<obj->m_coordcount-1; i++)
		{
			// triangle formed by outer line segment and internal position
			if(DPoint2D_WithinTriangle(p1,&obj->m_worldcoords[i],&obj->m_worldcoords[i+1],&obj->m_pos))
			{
				return true;
			}
		}
		if(DPoint2D_WithinTriangle(p1,&obj->m_worldcoords[obj->m_coordcount-1],&obj->m_worldcoords[0],&obj->m_pos))
		{
			return true;
		}
		return false;
	}
	
	// find bounding box min/max x and y for each object
	double minx1=o->m_worldcoords[0].m_x;
	double miny1=o->m_worldcoords[0].m_y;
	double maxx1=o->m_worldcoords[0].m_x;
	double maxy1=o->m_worldcoords[0].m_y;
	double minx2=rhs->m_worldcoords[0].m_x;
	double miny2=rhs->m_worldcoords[0].m_y;
	double maxx2=rhs->m_worldcoords[0].m_x;
	double maxy2=rhs->m_worldcoords[0].m_y;
	for(int i=1; i<10; i++)
	{
		if(i<o->m_coordcount)
		{
			minx1=_min(minx1,o->m_worldcoords[i].m_x);
			miny1=_min(miny1,o->m_worldcoords[i].m_y);
			maxx1=_max(maxx1,o->m_worldcoords[i].m_x);
			maxy1=_max(maxy1,o->m_worldcoords[i].m_y);
		}
		if(i<rhs->m_coordcount)
		{
			minx2=_min(minx2,rhs->m_worldcoords[i].m_x);
			miny2=_min(miny2,rhs->m_worldcoords[i].m_y);
			maxx2=_max(maxx2,rhs->m_worldcoords[i].m_x);
			maxy2=_max(maxy2,rhs->m_worldcoords[i].m_y);
		}
	}
	
	// check if bounding box coords don't intersect
	if(maxx1 < minx2 || miny1 > maxy2 || minx1 > maxx2 || maxy1 < miny2)
	{
		return false;
	}
	
	// polygon and polygon
	for(int i=0; i<o->m_coordcount; i++)
	{
		for(int j=0; j<rhs->m_coordcount-1; j++)
		{
			if(DPoint2D_WithinTriangle(&o->m_worldcoords[i],&rhs->m_worldcoords[j],&rhs->m_worldcoords[j+1],&rhs->m_pos))
			{
				return true;
			}
		}
		if(DPoint2D_WithinTriangle(&o->m_worldcoords[i],&rhs->m_worldcoords[rhs->m_coordcount-1],&rhs->m_worldcoords[0],&rhs->m_pos))
		{
			return true;
		}
	}
	for(int i=0; i<rhs->m_coordcount; i++)
	{
		for(int j=0; j<o->m_coordcount-1; j++)
		{
			if(DPoint2D_WithinTriangle(&rhs->m_worldcoords[i],&o->m_worldcoords[j],&o->m_worldcoords[j+1],&o->m_pos))
			{
				return true;
			}
		}
		if(DPoint2D_WithinTriangle(&rhs->m_worldcoords[i],&o->m_worldcoords[o->m_coordcount-1],&o->m_worldcoords[0],&o->m_pos))
		{
			return true;
		}
	}
	
	return false;
}

bool GameObject_CollisionWrapped(struct GameObject *o, const struct GameObject *rhs, const int w, const int h)
{
	struct GameObject wo;
	if(GameObject_Collision(o,rhs)==true)
	{
		return true;
	}
	
	GameObject_Initialize_GameObject(&wo,o);
	
	for(int wy=-h; wy<=h; wy+=h)
	{
		for(int wx=-w; wx<=w; wx+=w)
		{
			if(wy!=0 && wx!=0)
			{
				for(int i=0; i<wo.m_coordcount; i++)
				{
					wo.m_worldcoords[i].m_x=o->m_worldcoords[i].m_x+(double)wx;
					wo.m_worldcoords[i].m_y=o->m_worldcoords[i].m_y+(double)wy;
				}
				if(GameObject_Collision(&wo,rhs)==true)
				{
					return true;
				}
			}
		}
	}
	
	return false;

}

bool GameObject_CollisionSprite(struct GameObject *o, const unsigned char *sprite, const double spritex, const double spritey, const int spritew, const int spriteh)
{
	int minx[16];
	int maxx[16];
	int miny[16];
	int maxy[16];
	
	for(int i=0; i<16; i++)
	{
		minx[i]=-1;
		maxx[i]=-1;
		miny[i]=-1;
		maxy[i]=-1;
	}
	
	struct GameObject go;
	int byte=0;
	int8_t bit=7;
	for(int y=0; y<spriteh; y++)
	{
		for(int x=0; x<spritew; x++, bit--)
		{
			if(bit<0)
			{
				byte++;
				bit=7;
			}
			if((sprite[byte] >> bit) & 0x01)
			{
				if(minx[y]==-1)
				{
					minx[y]=x;
				}
				maxx[y]=x;
				if(miny[x]==-1)
				{
					miny[x]=y;
				}
				maxy[x]=y;
			}
		}
	}

	for(int y=0; y<spriteh; y++)
	{
		for(int x=0; x<spritew; x++)
		{
			if(x>=minx[y] && x<=maxx[y] && y>=miny[x] && y<=maxy[x])
			{
				go.m_alive=true;
				go.m_coordcount=1;
				go.m_worldcoords[0].m_x=spritex+(double)x;
				go.m_worldcoords[0].m_y=spritey+(double)y;
				if(GameObject_Collision(o,&go)==true)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void GameObject_CalculateWorldCoords(struct GameObject *o)
{
	for(int i=0; i<o->m_coordcount; i++)
	{
		double x=o->m_pos.m_x+(o->m_scale*((o->m_modelcoords[i].m_x*_cos(o->m_rotrad))-(o->m_modelcoords[i].m_y*_sin(o->m_rotrad))));
		double y=o->m_pos.m_y-(o->m_scale*((o->m_modelcoords[i].m_x*_sin(o->m_rotrad))+(o->m_modelcoords[i].m_y*_cos(o->m_rotrad))));
		DPoint2D_Initialize_XY(&o->m_worldcoords[i],x,y);
	}
}

void GameObject_Draw(struct GameObject *o)
{
	if(o->m_coordcount>0)
	{
		for(int i=0; i<o->m_coordcount-1; i++)
		{
			line(o->m_worldcoords[i].m_x,o->m_worldcoords[i].m_y,o->m_worldcoords[i+1].m_x,o->m_worldcoords[i+1].m_y);
		}
		line(o->m_worldcoords[o->m_coordcount-1].m_x,o->m_worldcoords[o->m_coordcount-1].m_y,o->m_worldcoords[0].m_x,o->m_worldcoords[0].m_y);
	}
}

void GameObject_DrawWrapped(struct GameObject *o, const int w, const int h)
{
	if(o->m_coordcount>0)
	{
		double minx=o->m_worldcoords[0].m_x;
		double miny=o->m_worldcoords[0].m_y;
		double maxx=o->m_worldcoords[0].m_x;
		double maxy=o->m_worldcoords[0].m_y;
		
		for(int i=1; i<o->m_coordcount; i++)
		{
			minx=_min(minx,o->m_worldcoords[i].m_x);
			miny=_min(miny,o->m_worldcoords[i].m_y);
			maxx=_max(maxx,o->m_worldcoords[i].m_x);
			maxy=_max(maxy,o->m_worldcoords[i].m_y);
		}
		
		GameObject_Draw(o);
		
		bool xoob=(minx<0 || maxx>=w ? true : false);
		bool yoob=(miny<0 || maxy>=h ? true : false);
		
		// if min or max x,y outside bounds, just wrap coords and draw again
		// wrap left/right
		if(xoob==true)
		{
			struct GameObject wrapo;
			GameObject_Initialize_GameObject(&wrapo,o);
			for(int i=0; i<wrapo.m_coordcount; i++)
			{
				if(minx<0)
				{
					wrapo.m_worldcoords[i].m_x+=w;
				}
				else
				{
					wrapo.m_worldcoords[i].m_x-=w;
				}
			}
			GameObject_Draw(&wrapo);
		}
		// wrap top/bottom
		if(yoob==true)
		{
			struct GameObject wrapo;
			GameObject_Initialize_GameObject(&wrapo,o);
			for(int i=0; i<wrapo.m_coordcount; i++)
			{
				if(miny<0)
				{
					wrapo.m_worldcoords[i].m_y+=h;
				}
				else
				{
					wrapo.m_worldcoords[i].m_y-=h;
				}
			}
			GameObject_Draw(&wrapo);
		}
		// wrap both left/right top/bottom
		if(xoob==true && yoob==true)
		{
			struct GameObject wrapo;
			GameObject_Initialize_GameObject(&wrapo,o);
			for(int i=0; i<wrapo.m_coordcount; i++)
			{
				if(minx<0)
				{
					wrapo.m_worldcoords[i].m_x+=w;
				}
				else
				{
					wrapo.m_worldcoords[i].m_x-=w;
				}
				if(miny<0)
				{
					wrapo.m_worldcoords[i].m_y+=h;
				}
				else
				{
					wrapo.m_worldcoords[i].m_y-=h;
				}
			}
			GameObject_Draw(&wrapo);
		}
		
	}
}
