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
    This header contains algorithms to handle navigation
    and proper implementation of the collision algorithms
*/


#ifndef _LD_NAVI_2D_H
#define _LD_NAVI_2D_H

#include <stdio.h>
#include <stdlib.h>


#include "LD_Collision_2D.h"
#include "LD_Geometry.h"

/************
 * Structs
 * ************/

typedef struct
{
    int Edges_Count;
    Line_Segment_2D_Struct * EdgesBuffer;
}Edges_Map_2D_struct;

struct Navi_Map_2D_struct
{
    int Node_Count;
    struct _NavNode_2D_
    {
	float x;
	float y;

	int Connections_Count;
	struct _NavNode_2D_Con_
	{
	    float Common_Edge_Length;
	    struct _NavNode_2D_ * Node;
	} * Connections_Array;
    } * Node_Array;

};

typedef struct Navi_Map_2D_struct Navi_Map_2D_struct;
typedef struct _NavNode_2D_ _NavNode_2D_;

/************
 * DECL
 * ************/

/********************************************************************************
//This function recives an array of line segments, a Swept struct
//and the functions to do the calculations.
//It also recives the parameter for the direction of the segments perpendiculars
*********************************************************************************/
int Edges_Map_2D_vs_Swept(Edges_Map_2D_struct * Edges_Map,void * Swept_Object, 
int (*Swept_Algorithm) (void*,Line_Segment_2D_Struct*,Collision_2D_Struct*,float),
void (*Response_Algorithm) (void*,Collision_2D_Struct*),
float ClockWise_Multiplier); //-1.f

void Edges_Map_2D_Clear(Edges_Map_2D_struct * Edges_Map);

/******************************************
 *This function recieves a RAW buffer of 2D Triangles and a number of triangles
 *This function generates a graph where each node is the baricenter of each triangle
 *This node can be used for algoritms such as the A* algorithm
 * *****************************************/
int Navi_Map_2D_Generate(float * Triangle_2D_RAW_Compatible_Buffer, int Triangles_in_Buffer,
Navi_Map_2D_struct * Navi_Map);

void Navi_Map_2D_clear(Navi_Map_2D_struct * Navi_Map);

char Navi_Map_2D_FindPath(Navi_Map_2D_struct * Navi_Map,_NavNode_2D_  * Origin,_NavNode_2D_ * Destiny);

#endif
