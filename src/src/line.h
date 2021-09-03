#pragma once

#include "point.h"

struct Line
{
	struct DPoint2D m_p1;
	struct DPoint2D m_p2;
};

void Line_Line(struct Line *l);
void Line_Initialize_XY(struct Line *l, const double x1, const double y1, const double x2, const double y2);
void Line_Initialize_DPoint2D(struct Line *l, const struct DPoint2D *p1, const struct DPoint2D *p2);
bool Line_Intersects_DPoint2D(struct Line *l, const struct DPoint2D *p1, const struct DPoint2D *p2);
bool Line_Intersects_Line(struct Line *l, const struct Line *rhs);
bool Line_OnSegment(const struct DPoint2D *p1, const struct DPoint2D *p2, const struct DPoint2D *p3);
int Line_Orientation(const struct DPoint2D *p1, const struct DPoint2D *p2, const struct DPoint2D *p3);
