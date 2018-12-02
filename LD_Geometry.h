

#ifndef _LD_GEOMETRY_H
#define _LD_GEOMETRY_H

#include "LD_Math.h"

#ifdef __cplusplus
extern "C"{
#endif
    void Triangle_3D_Baricenter(float * Triangle, float * Result);
    void Triangle_2D_Baricenter(float * Triangle, float * Result);
    char Check_Point_in_Triangle_2D(float * Triangle, float * Point);
#ifdef __cplusplus
}
#endif

#endif
