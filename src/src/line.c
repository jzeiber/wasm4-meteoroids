#include "line.h"
#include "miscfuncs.h"
#include <stdbool.h>

void Line_Line(struct Line *l)
{
	DPoint2D_Initialize(&l->m_p1);
	DPoint2D_Initialize(&l->m_p2);
}

void Line_Initialize_XY(struct Line *l, const double x1, const double y1, const double x2, const double y2)
{
	DPoint2D_Initialize_XY(&l->m_p1,x1,y1);
	DPoint2D_Initialize_XY(&l->m_p2,x2,y2);
}

void Line_Initialize_DPoint2D(struct Line *l, const struct DPoint2D *p1, const struct DPoint2D *p2)
{
	DPoint2D_Initialize_DPoint2D(&l->m_p1,p1);
	DPoint2D_Initialize_DPoint2D(&l->m_p2,p2);
}


bool Line_Intersects_DPoint2D(struct Line *l, const struct DPoint2D *p1, const struct DPoint2D *p2)
{
	struct Line l2;
	Line_Initialize_DPoint2D(&l2,p1,p2);
	return Line_Intersects_Line(l,&l2);
}
	
bool Line_Intersects_Line(struct Line *l, const struct Line *rhs)
{
	int o1=Line_Orientation(&l->m_p1,&l->m_p2,&rhs->m_p1);
	int o2=Line_Orientation(&l->m_p1,&l->m_p2,&rhs->m_p2);
	int o3=Line_Orientation(&rhs->m_p1,&rhs->m_p2,&l->m_p1);
	int o4=Line_Orientation(&rhs->m_p1,&rhs->m_p2,&l->m_p2);
	
	if(o1!=o2 && o3!=o4)
	{
		return true;
	}
	
	if(o1==0 && Line_OnSegment(&l->m_p1,&rhs->m_p1,&l->m_p2))
	{
		return true;
	}
	
	if(o2==0 && Line_OnSegment(&l->m_p1,&rhs->m_p2,&l->m_p2))
	{
		return true;
	}
	
	if(o3==0 && Line_OnSegment(&rhs->m_p1,&l->m_p1,&rhs->m_p2))
	{
		return true;
	}
	
	if(o4==0 && Line_OnSegment(&rhs->m_p1,&l->m_p2,&rhs->m_p2))
	{
		return true;
	}
	
	return false;
}

bool Line_OnSegment(const struct DPoint2D *p1, const struct DPoint2D *p2, const struct DPoint2D *p3)
{
	if(p2->m_x <= _max(p1->m_x,p3->m_x) && p2->m_x >= _min(p1->m_x,p3->m_x) &&
	   p2->m_y <= _max(p1->m_y,p3->m_y) && p2->m_y >= _min(p1->m_y,p3->m_y))
	{
		return true;
	}
	return false;
}

int Line_Orientation(const struct DPoint2D *p1, const struct DPoint2D *p2, const struct DPoint2D *p3)
{
	double val=(p2->m_y-p1->m_y) * (p3->m_x-p2->m_x) - (p2->m_x-p1->m_x) * (p3->m_y-p2->m_y);
	if(val==0)
	{
		return 0;	// colinear
	}
	return (val>0) ? 1 : 2;	// clockwise or counterclockwise
}

