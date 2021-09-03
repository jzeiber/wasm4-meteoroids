#pragma once

#include <stdint.h>
#include <stdbool.h>

struct Point2D
{
	int32_t m_x;
	int32_t m_y;
};

struct DPoint2D
{
	double m_x;
	double m_y;
};

void Point2D_Initialize(struct Point2D *p);
void Point2D_Initialize_XY(struct Point2D *p, const int32_t x, const int32_t y);
void Point2D_Initialize_DPoint2D(struct Point2D *p, const struct DPoint2D *rhs);
void Point2D_OffsetPos(struct Point2D *p, const int32_t dx, const int32_t dy);
void Point2D_SetPos(struct Point2D *p, const int32_t x, const int32_t y);
bool Point2D_WithinTriangle(struct Point2D *p, const struct Point2D *p1, const struct Point2D *p2, const struct Point2D *p3);
struct Point2D *Point2D_Assign(struct Point2D *p, const struct DPoint2D *rhs);


void DPoint2D_Initialize(struct DPoint2D *p);
void DPoint2D_Initialize_XY(struct DPoint2D *p, const double x, const double y);
void DPoint2D_Initialize_Point2D(struct DPoint2D *p, const struct Point2D *rhs);
void DPoint2D_Initialize_DPoint2D(struct DPoint2D *p, const struct DPoint2D *rhs);
void DPoint2D_OffsetPos(struct DPoint2D *p, const double dx, const double dy);
void DPoint2D_SetPos(struct DPoint2D *p, const double x, const double y);
double DPoint2D_Sign(const struct DPoint2D *p1, const struct DPoint2D *p2, const struct DPoint2D *p3);
// is this point contained within triangle formed by p1,p2,p3
bool DPoint2D_WithinTriangle(const struct DPoint2D *p, const struct DPoint2D *p1, const struct DPoint2D *p2, const struct DPoint2D *p3);
struct DPoint2D *DPoint2D_Assign(struct DPoint2D *p, const struct Point2D *rhs);