#ifndef _LD_GEOMETRY_H
#define _LD_GEOMETRY_H

#include "LD_Math.h"

#ifdef __cplusplus
extern "C"{
#endif
    void Triangle_3D_Barycenter(float * Triangle, float * Result);
    void Triangle_2D_Barycenter(float * Triangle, float * Result);
    char Check_Point_in_Triangle_2D(float * Triangle, float * Point,int Byte_offset);
    char Check_Point_in_Segment_2D(float * Segment, float * Point);
    char Check_Segment_vs_Segment_2D(float * Segment_A, float * Segment_B,float * Return_A_T);
    char Check_V3V3_vs_Plane(float *Time,void *V3V3,void *Plane,char Mode);
#define Check_Line_vs_Plane(Time_Return,V3V3_Line,V3V3_Plane) \
	Check_V3V3_vs_Plane(Time_Return,V3V3_Line,V3V3_Plane,0)
#define Check_Ray_vs_Plane(Time_Return,V3V3_Ray,V3V3_Plane) \
	Check_V3V3_vs_Plane(Time_Return,V3V3_Ray,V3V3_Plane,1)
#define Check_Segment_vs_Plane(Time_Return,V3V3_Seg,V3V3_Plane) \
	Check_V3V3_vs_Plane(Time_Return,V3V3_Seg,V3V3_Plane,2)

#ifdef __cplusplus
}
#endif

#endif
