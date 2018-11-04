#include "LD_STL.h"
#include <stdio.h>
#include <string.h>

int STL_BIN_Load_NoNormals(const char * File_path,STL_Mesh_NoNormals_Struct * STL_MESH )
{
    FILE * file = fopen(File_path,"rb");
    if(file == NULL){return -1;}

    fseek(file,80,SEEK_SET);
 
    if(fread(&STL_MESH->Triangles_Count,4,1,file)<0){return -1;}

    STL_MESH->Triangle_Array = (struct _STL_Triangle_Struct_ *) 
    malloc (sizeof(struct _STL_Triangle_Struct_ )*STL_MESH->Triangles_Count);

    for (struct _STL_Triangle_Struct_ * exp = STL_MESH->Triangle_Array;exp < STL_MESH->Triangle_Array+STL_MESH->Triangles_Count;exp++)
    {
        fseek(file,4*3,SEEK_CUR);
        fread(exp,4,3*3,file);
        fseek(file,2,SEEK_CUR);
    }

    fclose (file);
    return 1;
}
