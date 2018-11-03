/***********************************************************************
MIT License

Copyright (c) 2018 Luis Delgado

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/

/*
This library contains algorithms to handle 2D collision detection and response
*/

#ifndef _LD_COLLISION_H 
#define _LD_COLLISION_H

#include "LD_Math.h"
#include <stdlib.h>
#include <string.h>
#include <math.h> // <- needs libm to be linked. -lm


typedef struct AABB_2D_Struct
{
    float Center_Position[2];
    float Half_Extent[2];   
}AABB_2D_Struct;

typedef struct Swept_AABB_2D_Struct
{
    float Center_Position[2];
    float Half_Extent[2];
    float Direction[2];
} Swept_AABB_2D_Struct;

typedef struct Line_Segment_2D_Struct
{
    float Point_A[2];
    float Point_B[2];
} Line_Segment_2D_Struct;

typedef struct Collision_2D_Struct
{
    float Time;
    float Point[2];
} Collision_2D_Struct;

/****************************************
 * DECL
/****************************************/

void Swept_AABB_2D_Create(Swept_AABB_2D_Struct * Swept_AABB_2D, 
float Center_Position_X, float Center_Position_Y,
float Half_Extent_X,float Half_Extent_Y);

int Swept_AABB_vs_Line_Segment_2D_Check
(void * _Swept_AABB_,
Line_Segment_2D_Struct * Line_Segment, 
Collision_2D_Struct * Return_Collision,
float ClockWise_Multiplier);

int Swept_AABB_2D_vs_Static_AABB_2D_Check(void * _Swept_AABB_, 
AABB_2D_Struct * Static_AABB,Collision_2D_Struct * Collision_Data);

int Swept_AABB_2D_vs_Swept_AABB_2D_Check(Swept_AABB_2D_Struct * Swept_AABB_A,
Swept_AABB_2D_Struct * Swept_AABB_B,Collision_2D_Struct * Collision_Data);

void Swept_AABB_2D_Response_Slide(void * _Swept_AABB_, 
Collision_2D_Struct * Collision_Data);

#endif