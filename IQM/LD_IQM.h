#ifndef _LD_IQM_H
#define _LD_IQM_H

#include <stdlib.h>
#include "../Extern/LD_Math.h"
#include "../libLDExtra/LD_Tricks.h"

#define FROMBLENDER

typedef struct IQM_RAW_Struct
{
    unsigned int Filesize;
	unsigned int ModelFlags;
    unsigned int VertexCount;
	unsigned int TrianglesCount;
	unsigned int Sizeof_Vec3Buffers;
    unsigned int Sizeof_Vec2Buffers;
    unsigned int IndicesCount;
    unsigned int JointsCount;

    struct _IQM_Vec3_Struct
    {
        float x,y,z;
    } * Vertices;

    struct _IQM_Vec3_Struct * Normals;

    struct _IQM_UV_Struct
    {
        float u,v;
    } * UV;

    unsigned char * BlendingIndices;
    unsigned char * BlendingWeights;

    struct _IQM_Joint_Struct
    {
        unsigned int name;
        int parent;
        float translate [3];
        float rotate [4];
        float scale [3];
    } * Joints;

    int * Indices;

    unsigned int PosesCount;
    unsigned int AnimationsCount;
    unsigned int FramesCount;

    struct _IQM_Joint_Struct * Poses;

    struct _IQM_Anim_Struct
    {
        unsigned int name;
        unsigned int first_frame;
        unsigned int num_frames;
        float framerate;
        unsigned int flags;
    } * Animations;

} IQM_RAW_Struct;

struct _IQM_Pose_Struct 
 {
    int parent;
    unsigned int mask;
    float translate [3];
    float rotate [4];
    float scale [3];
    float channelscale[10];
};


static void __IQM_Joint_SWAP_Y_Z__( struct _IQM_Joint_Struct * Poses,unsigned int JointsCount)
{
    struct _IQM_Joint_Struct* END = Poses+JointsCount;
    int Quaternion[4]={0x3f3504f3,0x80000000,0x80000000,0xbf3504f3}; //<- It solves the angle of the root
    //x = sqr(2)/2, y=0, z=0, w = -sqr(2)/2
    int Quaternion_2[4]={0xbf3504f3,0x80000000,0x80000000,0xbf3504f3}; //<- It solves the angle of the rest
    //x = -sqr(2)/2, y=0, z=0, w = -sqr(2)/2
    
    for(Poses;Poses<END;Poses++)
    {
	if(Poses->parent == -1)
	{QuaternionMUL((float*)Quaternion,Poses->rotate,Poses->rotate);}
	else
	{

	    QuaternionRotateV3((float*)Quaternion_2,(float*)Poses->translate,(float*)Poses->translate);
 
	    _swap(*(int*)(Poses->rotate+1),*(int*)(Poses->rotate+2));
	    Poses->rotate[1]*=-1;
	}
    }

}

static void __IQM_Poses_into_Joints(struct _IQM_Pose_Struct * Poses, struct _IQM_Joint_Struct * Return,
    unsigned int PosesCount, unsigned short* Frames, unsigned int FramesCount)
{
    struct _IQM_Pose_Struct * END = Poses+PosesCount;
    struct _IQM_Pose_Struct * START = Poses;
    struct _IQM_Pose_Struct * INIT = Poses;

/*
    for(;Poses<END;Poses++,Return++)
    {	
	struct _IQM_Pose_Struct * MJoint = Poses;
	printf("Mask: %x\n",MJoint->mask);
	printf("T: (%f,%f,%f), R: (%f,%f,%f,%f), S: (%f,%f,%f)",MJoint->translate[0],MJoint->translate[1],MJoint->translate[2],MJoint->rotate[0],MJoint->rotate[1],MJoint->rotate[2],MJoint->rotate[3],MJoint->scale[0],MJoint->scale[1],MJoint->scale[2]);printf("\n"); 
	printf(" t: (%f,%f,%f), r: (%f,%f,%f,%f), s: (%f,%f,%f)",MJoint->channelscale[0],MJoint->channelscale[1],MJoint->channelscale[2],MJoint->channelscale[3],MJoint->channelscale[4],MJoint->channelscale[5],MJoint->channelscale[6],MJoint->channelscale[7],MJoint->channelscale[8],MJoint->channelscale[9]);printf("\n"); 
    }
    printf("FC: %d\n",FramesCount);
*/

    for(int i = FramesCount;i--;)
    {
//	    printf("PosesBuff:%lx \n",Return);
	INIT = START;
	for(;INIT<END;INIT++,Return++)
	{
	    Return->parent = INIT->parent;	    
	    struct _IQM_Joint_Struct* Joint = Return;
	    char Maskcond;unsigned int cond = 1;
	    float * channelscale = INIT->channelscale;	   
	     
	    #define MSKCOND Maskcond = ((INIT->mask&cond) == cond)

	    MSKCOND;
	    //printf("%d (%d) | %x&%x = %x \n",*Frames,Maskcond,INIT->mask,cond,INIT->mask&cond);
	    Joint->translate[0] = INIT->translate[0] +(*Frames*Maskcond*(*channelscale));
		channelscale++;Frames+=Maskcond;cond<<=1;
	    Maskcond = (INIT->mask&cond==cond);
	    MSKCOND;
	    //printf("%d (%d) | %x&%x = %x \n",*Frames,Maskcond,INIT->mask,cond,INIT->mask&cond);
	    Joint->translate[1] = INIT->translate[1] +(*Frames*Maskcond*(*channelscale));
		channelscale++;Frames+=Maskcond;cond<<=1;
	    Maskcond = (INIT->mask&cond==cond);
	    MSKCOND;
	    //printf("%d (%d) | %x&%x = %x \n",*Frames,Maskcond,INIT->mask,cond,INIT->mask&cond);
	    Joint->translate[2] = INIT->translate[2] +(*Frames*Maskcond*(*channelscale));
		channelscale++;Frames+=Maskcond;cond<<=1;

	    MSKCOND;
	    //printf("%d (%d) | %x&%x = %x \n",*Frames,Maskcond,INIT->mask,cond,INIT->mask&cond);
	    Joint->rotate[0] = INIT->rotate[0] +(*Frames*Maskcond*(*channelscale));
		channelscale++;Frames+=Maskcond;cond<<=1;
	    MSKCOND;
	    //printf("%d (%d) | %x&%x = %x \n",*Frames,Maskcond,INIT->mask,cond,INIT->mask&cond);
	    Joint->rotate[1] = INIT->rotate[1] +(*Frames*Maskcond*(*channelscale));
		channelscale++;Frames+=Maskcond;cond<<=1;
	    MSKCOND;
	    //printf("%d (%d) | %x&%x = %x \n",*Frames,Maskcond,INIT->mask,cond,INIT->mask&cond);
	    Joint->rotate[2] = INIT->rotate[2] +(*Frames*Maskcond*(*channelscale));
		channelscale++;Frames+=Maskcond;cond<<=1;	
	    MSKCOND;
	    //printf("%d (%d) | %x&%x = %x \n",*Frames,Maskcond,INIT->mask,cond,INIT->mask&cond);
	    Joint->rotate[3] = INIT->rotate[3] +(*Frames*Maskcond*(*channelscale));
		channelscale++;Frames+=Maskcond;cond<<=1;
	    
	    MSKCOND;
	    Joint->scale[0] = INIT->scale[0] +(*Frames*Maskcond*(*channelscale));
		channelscale++;Frames+=Maskcond;cond<<=1;
	    MSKCOND;
	    Joint->scale[1] = INIT->scale[1] +(*Frames*Maskcond*(*channelscale));
		channelscale++;Frames+=Maskcond;cond<<=1;
	    MSKCOND;
	    Joint->scale[2] = INIT->scale[2] +(*Frames*Maskcond*(*channelscale));
		channelscale++;Frames+=Maskcond;cond<<=1;


	struct _IQM_Joint_Struct * MJoint = Joint;
//	printf("T: (%f,%f,%f), R: (%f,%f,%f,%f), S: (%f,%f,%f)",MJoint->translate[0],MJoint->translate[1],MJoint->translate[2],MJoint->rotate[0],MJoint->rotate[1],MJoint->rotate[2],MJoint->rotate[3],MJoint->scale[0],MJoint->scale[1],MJoint->scale[2]);printf("\n"); 
 

	//The joints are somewhat compressed, so, after decipher, the results are stored in the buffer

	

	}
//	printf("\n");
//	getchar();	
    }
}

void __IQM_Joints_Concatenate(struct _IQM_Joint_Struct * JointsBuffer,unsigned int JointsCount,
				unsigned int Poses_Per_Frame)
{

    struct _IQM_Joint_Struct * Joint = JointsBuffer;
    struct _IQM_Joint_Struct * END = Joint+JointsCount;
    
    unsigned int PosesCounter = 0;
    unsigned int FrameCounter = 0;

    for(;Joint<END;Joint++)
    {
	PosesCounter++;
	//printf("PC:  %d vs PpF: %d\n",PosesCounter,Poses_Per_Frame);

	if(PosesCounter > Poses_Per_Frame){FrameCounter++,PosesCounter =1;}
	if(Joint->parent ==-1){continue;}

	struct _IQM_Joint_Struct * Parent = JointsBuffer+(Poses_Per_Frame*FrameCounter)+Joint->parent;

//	printf("S: (%f,%f,%f)\n",Parent->scale[0],Parent->scale[1],Parent->scale[2]);
	V3V3MUL(Joint->translate,Parent->scale,Joint->translate);
	//printf("Frm: %d\n",FrameCounter);
	//printf("INI: (%f,%f,%f), (%f,%f,%f,%f)\n",Joint->translate[0],Joint->translate[1],Joint->translate[2],Joint->rotate[0],Joint->rotate[1],Joint->rotate[2],Joint->rotate[3]);
	QuaternionRotateV3(Parent->rotate,Joint->translate,Joint->translate);
	//printf("ROT: (%f,%f,%f) by (%f,%f,%f,%f)\n",Joint->translate[0],Joint->translate[1],Joint->translate[2],Parent->rotate[0],Parent->rotate[1],Parent->rotate[2],Parent->rotate[3]);
        QuaternionMUL(Joint->rotate,Parent->rotate,Joint->rotate);
        V3V3ADD(Joint->translate,Parent->translate,Joint->translate);
	//printf("TLT: (%f,%f,%f) by (%f,%f,%f)\n",Joint->translate[0],Joint->translate[1],Joint->translate[2],Parent->translate[0],Parent->translate[1],Parent->translate[2]);
	//printf("FIN: (%f,%f,%f), (%f,%f,%f,%f)\n",Joint->translate[0],Joint->translate[1],Joint->translate[2],Joint->rotate[0],Joint->rotate[1],Joint->rotate[2],Joint->rotate[3]);
	//printf("\n");

    }
    //printf("\n"); 
    //getchar();
}

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

    unsigned int * VertexCount = &IQM_Model->VertexCount;
    unsigned int * TrianglesCount = &IQM_Model->TrianglesCount;
    unsigned int * JointsCount = &IQM_Model->JointsCount;
    unsigned int * Filesize = &IQM_Model->Filesize;
    unsigned int * ModelFlags = &IQM_Model->ModelFlags;

    *JointsCount = Header.num_joints;


    fseek(file,Header.ofs_meshes+(12), SEEK_SET);

    fread(VertexCount,4,1,file);
    fseek(file,4, SEEK_CUR);
    fread(TrianglesCount,4,1,file);

    unsigned int tmpcalc = *VertexCount*12;
    unsigned int tmpcalc3 = *VertexCount*8;
    unsigned int tmpcalc2 = *TrianglesCount*3;
    unsigned int tmpcalc5 = *VertexCount*16;
    unsigned int tmpcalc4 =*JointsCount * sizeof(struct _IQM_Joint_Struct);
    unsigned int tmpcalc6 = *VertexCount*4;

    IQM_Model->IndicesCount=tmpcalc2;
    //printf("%d\n",tmpcalc2);

    tmpcalc+=4; //<- It's 4 instead of 3 to avoid memory reading errors.
    //It have to do with memory alignment.
    //An XMM register can load 1, 2 or 4 IEEE-756 floats, but not 3.

    IQM_Model->Sizeof_Vec2Buffers=tmpcalc3;
    IQM_Model->Sizeof_Vec3Buffers=tmpcalc;

    void * TmpPos = malloc(tmpcalc);
    void * TmpNormal = malloc(tmpcalc);
    void * TmpUV =  malloc(tmpcalc3);
    void * TmpTangent = malloc(tmpcalc5);
    void * TmpBlendingIndices = malloc(tmpcalc6);
    void * TmpBlendingWeights = malloc(tmpcalc6);
    void * TmpIndices = malloc(tmpcalc2*4);
    void * TmpJoints = malloc(tmpcalc4);

    fseek(file,Header.ofs_vertexarrays+16, SEEK_SET);
    fread(&tmpcalc,4,1,file);
    
    fseek(file,tmpcalc, SEEK_SET);
    //printf("Offset: %d (%x)\n",tmpcalc,tmpcalc);

    fread(TmpPos,4,*VertexCount*3,file);
    fread(TmpUV,4,*VertexCount*2,file);
    fread(TmpNormal,4,*VertexCount*3,file);
    fread(TmpTangent,4,*VertexCount*4,file);
    fread(TmpBlendingIndices,1,*VertexCount*4,file);
    fread(TmpBlendingWeights,1,*VertexCount*4,file);
    //printf("%i\n",VertexCount);

    fseek(file,Header.ofs_triangles, SEEK_SET);
    fread(TmpIndices,4,tmpcalc2,file);

    fseek(file,Header.ofs_joints, SEEK_SET);
    fread(TmpJoints,tmpcalc4,1,file);

    IQM_Model->Vertices=(struct _IQM_Vec3_Struct*)TmpPos;
    IQM_Model->UV =(struct _IQM_UV_Struct*) TmpUV;
    IQM_Model->Normals=(struct _IQM_Vec3_Struct*)TmpNormal;
    IQM_Model->Indices=(int*)TmpIndices;
    IQM_Model->Joints = (struct _IQM_Joint_Struct*)TmpJoints;
    IQM_Model->BlendingIndices = TmpBlendingIndices;
    IQM_Model->BlendingWeights = TmpBlendingWeights;


    //**Load Animations **//

    IQM_Model->PosesCount = Header.num_poses;
    IQM_Model->AnimationsCount =Header.num_anims;
    IQM_Model->FramesCount = Header.num_frames;

    tmpcalc5 = Header.num_poses;
    tmpcalc6 = Header.num_frames*Header.num_framechannels;
    tmpcalc = tmpcalc5*Header.num_frames*sizeof(struct _IQM_Joint_Struct);
    tmpcalc2 = Header.num_anims*sizeof(struct _IQM_Anim_Struct);
    tmpcalc3 = tmpcalc6*sizeof(unsigned short);
    tmpcalc4 = tmpcalc5*sizeof(struct _IQM_Pose_Struct);

    void * tmpPoses = malloc(tmpcalc);
    void * tmpAnims = malloc(tmpcalc2);
    void * tmpFrames = malloc(tmpcalc3);

    struct _IQM_Pose_Struct * Poses = malloc(tmpcalc4);
   
    fseek(file,Header.ofs_poses, SEEK_SET);
    fread(Poses,sizeof(struct _IQM_Pose_Struct),tmpcalc5,file);
    fseek(file,Header.ofs_anims, SEEK_SET);
    fread(tmpAnims,tmpcalc2,1,file);
    fseek(file,Header.ofs_frames, SEEK_SET);
    fread(tmpFrames,tmpcalc3,1,file);

	
	//** Process RAW poses into joints **//
    //printf("Frames:%d | FrameChannels:%d | FramePoses: %d\n",Header.num_frames,Header.num_framechannels,Header.num_poses); 
    __IQM_Poses_into_Joints(Poses,tmpPoses,tmpcalc5,(unsigned short*) tmpFrames,Header.num_frames);	

    free(Poses); //<-- RAW frame poses are deleted. 
    free(tmpFrames);
	//** ***************************** **//
    
 
    IQM_Model->Poses = tmpPoses;//<-These poses are the animations frames poses. PosesCount*FramesCount
    IQM_Model->Animations = tmpAnims;

    
    #ifdef FROMBLENDER

    //*** Process Joints ***//
    //Swap Y and Z and correct root angles//
    __IQM_Joint_SWAP_Y_Z__(IQM_Model->Joints,IQM_Model->JointsCount);
    __IQM_Joint_SWAP_Y_Z__(IQM_Model->Poses,IQM_Model->FramesCount*IQM_Model->PosesCount);
    #endif

	//** Proccess RAW Joints to concatenate then **//
    __IQM_Joints_Concatenate(IQM_Model->Joints,IQM_Model->JointsCount,IQM_Model->PosesCount);
    __IQM_Joints_Concatenate(IQM_Model->Poses,IQM_Model->FramesCount*IQM_Model->PosesCount,IQM_Model->PosesCount);
	//** *************************************** **//
    

    fclose(file);

    free(TmpTangent); //<- Not implemented

    return 0;
}

void IQM_Clear(IQM_RAW_Struct * IQM)
{  
    if(IQM->Vertices!=NULL) {free(IQM->Vertices);}
    if(IQM->Normals!=NULL)  {free(IQM->Normals);}
    if(IQM->UV!=NULL)       {free(IQM->UV);}
    if(IQM->Indices!=NULL)  {free(IQM->Indices);}
    if(IQM->Joints!=NULL)   {free(IQM->Joints);}
    if(IQM->BlendingIndices!=NULL){free(IQM->BlendingIndices);}
    if(IQM->BlendingWeights!=NULL){free(IQM->BlendingWeights);}
    if(IQM->Poses!=NULL)    {free(IQM->Poses);}
    if(IQM->Animations!=NULL)    {free(IQM->Animations);}
}

void IQM_Destroy(IQM_RAW_Struct * IQM)
{
    IQM_Clear(IQM);
    free(IQM);
}

#endif
