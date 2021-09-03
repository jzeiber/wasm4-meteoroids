#include "point.h"
#include <stdint.h>
#include <stdbool.h>

void Point2D_Initialize(struct Point2D *p)
{
	p->m_x=0;
	p->m_y=0;
}

void Point2D_Initialize_XY(struct Point2D *p, const int32_t x, const int32_t y)
{
	p->m_x=x;
	p->m_y=y;
}

void Point2D_Initialize_DPoint2D(struct Point2D *p, const struct DPoint2D *rhs)
{
	p->m_x=rhs->m_x;
	p->m_y=rhs->m_y;
}

void Point2D_OffsetPos(struct Point2D *p, const int32_t dx, const int32_t dy)
{
	p->m_x+=dx;
	p->m_y+=dy;
}

void Point2D_SetPos(struct Point2D *p, const int32_t x, const int32_t y)
{
	p->m_x=x;
	p->m_y=y;
}

bool Point2D_WithinTriangle(struct Point2D *p, const struct Point2D *p1, const struct Point2D *p2, const struct Point2D *p3)
{
	struct DPoint2D dp;
	struct DPoint2D dp1;
	struct DPoint2D dp2;
	struct DPoint2D dp3;
	
	DPoint2D_Assign(&dp,p);
	DPoint2D_Assign(&dp1,p1);
	DPoint2D_Assign(&dp2,p2);
	DPoint2D_Assign(&dp3,p3);
	
	return DPoint2D_WithinTriangle(&dp,&dp1,&dp2,&dp3);
}

struct Point2D *Point2D_Assign(struct Point2D *p, const struct DPoint2D *rhs)
{
	p->m_x=rhs->m_x;
	p->m_y=rhs->m_y;
	return p;
}




void DPoint2D_Initialize(struct DPoint2D *p)
{
	p->m_x=0;
	p->m_y=0;
}

void DPoint2D_Initialize_XY(struct DPoint2D *p, const double x, const double y)
{
	p->m_x=x;
	p->m_y=y;
}

void DPoint2D_Initialize_Point2D(struct DPoint2D *p, const struct Point2D *rhs)
{
	p->m_x=rhs->m_x;
	p->m_y=rhs->m_y;
}

void DPoint2D_Initialize_DPoint2D(struct DPoint2D *p, const struct DPoint2D *rhs)
{
	p->m_x=rhs->m_x;
	p->m_y=rhs->m_y;
}

void DPoint2D_OffsetPos(struct DPoint2D *p, const double dx, const double dy)
{
	p->m_x+=dx;
	p->m_y+=dy;
}

void DPoint2D_SetPos(struct DPoint2D *p, const double x, const double y)
{
	p->m_x=x;
	p->m_y=y;
}

double DPoint2D_Sign(const struct DPoint2D *p1, const struct DPoint2D *p2, const struct DPoint2D *p3)
{
	return (p1->m_x - p3->m_x) * (p2->m_y - p3->m_y) - (p2->m_x - p3->m_x) * (p1->m_y - p3->m_y);
}
	
// is this point contained within triangle formed by p1,p2,p3
bool DPoint2D_WithinTriangle(const struct DPoint2D *p, const struct DPoint2D *p1, const struct DPoint2D *p2, const struct DPoint2D *p3)
{
	double d1, d2, d3;
	bool has_neg, has_pos;
	
	d1=DPoint2D_Sign(p,p1,p2);
	d2=DPoint2D_Sign(p,p2,p3);
	d3=DPoint2D_Sign(p,p3,p1);
	
	has_neg=(d1<0) || (d2<0) || (d3<0);
	has_pos=(d1>0) || (d2>0) || (d3>0);
	
	return !(has_neg && has_pos);
}
	
struct DPoint2D *DPoint2D_Assign(struct DPoint2D *p, const struct Point2D *rhs)
{
	p->m_x=rhs->m_x;
	p->m_y=rhs->m_y;
	return p;
}
