
#ifndef _LD_GEOMETRY_H
#define _LD_GEOMETRY_H

#include "LD_Math.h"

extern
{
    void Triangle_3D_Baricenter(float * Triangle, float * Result);
    char Check_2D_Point_in_3D_Triangle_no_Z(float * Triangle, float * Point);
}

#endif
