/* This header is for STL files processing */
/* Luis Delgado. 2018 */


#ifndef _LD_STL_H
#define _LD_STL_H

#include <stdlib.h>


/************************
 * Structs
 * **********************/

typedef struct _STL_Triangle_Struct_
{
    float Point_A[3];
    float Point_C[3];
    float Point_B[3];
}  _STL_Triangle_Struct_;

 typedef struct STL_Mesh_NoNormals_Struct
{
    int Triangles_Count;
    _STL_Triangle_Struct_ * Triangle_Array;

}STL_Mesh_NoNormals_Struct;



/************************
 * DECL
 * **********************/

/** Given a binary STL file path, the function return an array of triangles.
 *  This function ignores Normals.**/
int STL_BIN_Load_NoNormals(const char * File_path,STL_Mesh_NoNormals_Struct * STL_MESH );

#endif