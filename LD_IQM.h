#ifndef _LD_IQM_H
#define _LD_IQM_H

#include <stdlib.h>

typedef struct IQM_RAW_Struct
{
    int Filesize;
	int ModelFlags;
    int VertexCount;
	int TrianglesCount;
	int Sizeof_Vec3Buffers;
    int Sizeof_Vec2Buffers;
    int IndicesCount;
    int JointsCount;

    struct _IQM_Vec3_Struct
    {
        float x,y,z;
    } * Vertices;

    struct _IQM_Vec3_Struct * Normals;

    struct _IQM_UV_Struct
    {
        float u,v;
    } * UV;

    int * Indices;

    struct _IQM_Joint_Struct
    {
        unsigned int name;
        int parent;
        float translate [3];
        float rotate [4];
        float scale [3];
    } * Joints;

    struct _IQM_Blend_Struct
    {
        unsigned char Indices[4];
        unsigned char Weights[4];
    }  * Blendings;
    
    struct 
    {
        int parent;
        float translate [3];
        float rotate [4];
        float scale [3];
    } * Poses;

} IQM_RAW_Struct;


int IQM_Load_File(IQM_RAW_Struct * IQM_Model,const char * file_Path)
{
    FILE * file = fopen(file_Path, "rb");
    if (file == NULL){ return -1;}

    typedef struct 
    {
        char magic[16];
        unsigned int version;
        unsigned int filesize;
        unsigned int flags;
        unsigned int num_text, ofs_text;
        unsigned int num_meshes, ofs_meshes;
        unsigned int num_vertexarrays, num_vertexes, ofs_vertexarrays;
        unsigned int num_triangles, ofs_triangles, ofs_adjacency;
        unsigned int num_joints, ofs_joints;
        unsigned int num_poses, ofs_poses;
        unsigned int num_anims, ofs_anims;
        unsigned int num_frames, num_framechannels, ofs_frames, ofs_bounds;
        unsigned int num_comment, ofs_comment;
        unsigned int num_extensions, ofs_extensions;
    }iqmheader; 

    iqmheader Header;

    fread(&Header,sizeof(iqmheader),1,file);

    int * VertexCount = &IQM_Model->VertexCount;
    int * TrianglesCount = &IQM_Model->TrianglesCount;
    int * JointsCount = &IQM_Model->JointsCount;
    int * Filesize = &IQM_Model->Filesize;
    int * ModelFlags = &IQM_Model->ModelFlags;

    *JointsCount = Header.num_joints;


    fseek(file,Header.ofs_meshes+(12), SEEK_SET);

    fread(VertexCount,4,1,file);
    fseek(file,4, SEEK_CUR);
    fread(TrianglesCount,4,1,file);

    int tmpcalc = *VertexCount*12;
    int tmpcalc3 = *VertexCount*8;
    int tmpcalc2 = *TrianglesCount*3;
    int tmpcalc4 =*JointsCount * sizeof(struct _IQM_Joint_Struct);

    IQM_Model->IndicesCount=tmpcalc2;

    tmpcalc+=4; //<- It's 4 instead of 3 to avoid memory reading errors.
    //It have to do with memory alignment.
    //An XMM register can load 1, 2 or 4 IEEE-756 floats, but not 3.

    IQM_Model->Sizeof_Vec2Buffers=tmpcalc3;
    IQM_Model->Sizeof_Vec3Buffers=tmpcalc;

    uint8_t * TmpPos = (uint8_t*) malloc(tmpcalc);
    uint8_t * TmpNormal = (uint8_t*) malloc(tmpcalc);
    uint8_t * TmpUV = (uint8_t*) malloc(tmpcalc3);
    uint8_t * TmpIndices = (uint8_t*) malloc(tmpcalc2*4);
    uint8_t * TmpJoints = (uint8_t*) malloc(tmpcalc4);

    fseek(file,Header.ofs_vertexarrays+16, SEEK_SET);
    fread(&tmpcalc,4,1,file);
    
    fseek(file,tmpcalc, SEEK_SET);

    fread(TmpPos,4,*VertexCount*3,file);
    fread(TmpUV,4,*VertexCount*2,file);
    fread(TmpNormal,4,*VertexCount*3,file);
    //printf("%i\n",VertexCount);
    fseek(file,Header.ofs_triangles, SEEK_SET);
    
//printf("%d | %d | %d\n",ofs_triangles,num_triangles,*TrianglesCount);

    fread(TmpIndices,4,tmpcalc2,file);

    fseek(file,Header.ofs_joints, SEEK_SET);
    fread(TmpJoints,tmpcalc4,1,file);


    IQM_Model->Vertices=(struct _IQM_Vec3_Struct*)TmpPos;
    IQM_Model->UV =(struct _IQM_UV_Struct*) TmpUV;
    IQM_Model->Normals=(struct _IQM_Vec3_Struct*)TmpNormal;
    IQM_Model->Indices=(int*)TmpIndices;
    IQM_Model->Joints = (struct _IQM_Joint_Struct*)TmpJoints;

    fclose(file);

    return 0;
}

void IQM_Clear(IQM_RAW_Struct * IQM)
{   if(IQM->Vertices!=NULL) {free(IQM->Vertices);}
    if(IQM->Normals!=NULL)  {free(IQM->Normals);}
    if(IQM->UV!=NULL)       {free(IQM->UV);}
    if(IQM->Indices!=NULL)  {free(IQM->Indices);}
    if(IQM->Joints!=NULL)   {free(IQM->Joints);}
    if(IQM->Blendings!=NULL){free(IQM->Blendings);}
    if(IQM->Poses!=NULL)    {free(IQM->Poses);}
}

void IQM_Destroy(IQM_RAW_Struct * IQM)
{
    IQM_Clear(IQM);
    free(IQM);
}

#endif