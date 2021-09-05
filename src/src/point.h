#pragma once

#include <stdint.h>
#include <stdbool.h>

struct Point2D
{
	int32_t m_x;
	int32_t m_y;
};

struct FPoint2D
{
	float m_x;
	float m_y;
};

void Point2D_Initialize(struct Point2D *p);
void Point2D_Initialize_XY(struct Point2D *p, const int32_t x, const int32_t y);
void Point2D_Initialize_FPoint2D(struct Point2D *p, const struct FPoint2D *rhs);
void Point2D_OffsetPos(struct Point2D *p, const int32_t dx, const int32_t dy);
void Point2D_SetPos(struct Point2D *p, const int32_t x, const int32_t y);
bool Point2D_WithinTriangle(struct Point2D *p, const struct Point2D *p1, const struct Point2D *p2, const struct Point2D *p3);
struct Point2D *Point2D_Assign(struct Point2D *p, const struct FPoint2D *rhs);


void FPoint2D_Initialize(struct FPoint2D *p);
void FPoint2D_Initialize_XY(struct FPoint2D *p, const double x, const double y);
void FPoint2D_Initialize_Point2D(struct FPoint2D *p, const struct Point2D *rhs);
void FPoint2D_Initialize_FPoint2D(struct FPoint2D *p, const struct FPoint2D *rhs);
void FPoint2D_OffsetPos(struct FPoint2D *p, const double dx, const double dy);
void FPoint2D_SetPos(struct FPoint2D *p, const double x, const double y);
double FPoint2D_Sign(const struct FPoint2D *p1, const struct FPoint2D *p2, const struct FPoint2D *p3);
// is this point contained within triangle formed by p1,p2,p3
bool FPoint2D_WithinTriangle(const struct FPoint2D *p, const struct FPoint2D *p1, const struct FPoint2D *p2, const struct FPoint2D *p3);
struct FPoint2D *FPoint2D_Assign(struct FPoint2D *p, const struct Point2D *rhs);