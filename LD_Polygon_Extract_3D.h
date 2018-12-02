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
	This is a collection of algorithms to extract data (like submeshes) from 3D meshes
*/

#ifndef _LD_POLYGON_EXTRACT_3D_H
#define _LD_POLYGON_EXTRACT_3D_H

#include "LD_Geometry.h"

typedef struct
{
    float x;
    float y;
    float z;
}Vertex_3D_Struct;

typedef struct
{
    float x; float y;
}Vertex_2D_Struct;

typedef struct
{
    float Point_A[3];
    float Point_B[3];
}Edge_3D_Struct;

typedef struct
{
    float Point_A[2];
    float Point_B[2];
}Edge_2D_Struct;

typedef struct
{
    Vertex_3D_Struct Point_A;
    Vertex_3D_Struct Point_B;
    Vertex_3D_Struct Point_C;
}Triangle_3D_struct;

typedef struct
{
    Vertex_2D_Struct Point_A;
    Vertex_2D_Struct Point_B;
    Vertex_2D_Struct Point_C;
}Triangle_2D_struct;

typedef struct Edge_2D_Linked_List_Struct
{
    Edge_2D_Struct Data;
    struct Edge_2D_Linked_List_Struct * Next;
}Edge_2D_Linked_List_Struct;



/*****************************************************/
/*  DECL
/*****************************************************/

/***************************************************
	Given a mesh made out of triangles with three components,
	This algorithm extracts only the border edges.
	This algorithm ignores the Z component.
	The result is a 2D Polygon.
***************************************************/
Edge_2D_Struct * Triangles_3D_Extract_Border_Edges_to_2D_no_Z
(float * Compatible_RAW_3D_Triangles_Buffer,int Number_of_Triangles_in_Buffer,
int * Edges_Count_Return);

/**************************************************
	Given a mesh made out of triangles with three components,
	this algoritms eliminates one component.
	The result is an array of triangles with two componenets.
**************************************************/
float * Triangle_3D_to_2D 
(float * Compatible_RAW_3D_Triangle_Buffer, int Number_of_Triangles_in_Buffer,
char Component_to_eliminate);//Component 0 = X, Component 1 = Y, Component 2 = Z

#endif
