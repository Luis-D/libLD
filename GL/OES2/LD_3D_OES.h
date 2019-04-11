#ifndef LD_3D_OES_H
#define LD_3D_OES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../libLDExtra/LD_Stack_Queue.h"
#include "../../libLDExtra/LD_Tricks.h"

#include "../GL_UTILS.h"

    /** Mathematical operations **/
    /** By default it uses libLDM **/
#include "../../LDM/LD_Math.h"
#define __M4x4V4_PseudoV3_W1_MUL_ M4MULV3
#define __M4x4_PseudoM3x3_V4_PseudoV3_W1_MUL_ AM4MULV3
#define __QuatInterpolation V4LERP

/*
void QuaternionUnit_Inverse(void * Quaternion, void * Result) //Needs ASM implementation
{
    ((float*)Result)[0] =((float*)Quaternion)[0] *-1.f;
    ((float*)Result)[1] =((float*)Quaternion)[1] *-1.f;
    ((float*)Result)[2] =((float*)Quaternion)[2] *-1.f;
    ((float*)Result)[3] =((float*)Quaternion)[3] ;
}

void Quaternion_Diff_Unit(void * Q1, void * Q2, void * Result) //Needs ASM implementation
{
    QuaternionUnit_Inverse(Q1,Result);
    QuaternionMUL(Result,Q2,Result);
}
*/

typedef struct LD_Model_Object_Struct 
{
    unsigned int VertexCount;
	unsigned int TrianglesCount;
	unsigned int Sizeof_Vec3Buffers;
    unsigned int Sizeof_Vec2Buffers;
    unsigned int Sizeof_Vec4Buffers;
    unsigned int JointsCount;
    unsigned int IndicesCount;

    struct __Vec3_Struct{float x,y,z;}  * Vertices;
    struct __Vec3_Struct                * Normals;
    struct __UV_Struct{float u,v;}      * UV;
    struct __Vec4_Struct{float x,y,z;}  * Tangents;
    unsigned char * BlendingIndices;
    unsigned char * BlendingWeights;
    struct __Joint_Struct
    {
        unsigned int name;
        int parent;
        float translate [3];
        float rotate [4];
        float scale [3];
    } * Joints;
    
    int * Indices;

}LD_Model_Object_Struct;

//One animation for many instances
//Its equal to the IQM_RAW_Struct animation section.
//This struct shall be used to access to a pointer to that section
typedef struct LD_Animation_Object_Struct
{ 
    unsigned int PosesCount;
    unsigned int AnimationsCount;
    unsigned int FramesCount;

    struct __Joint_Struct * Poses;

    struct ___Anim_Struct
    {   
        unsigned int name;
        unsigned int first_frame;
        unsigned int num_frames;
        float framerate;
        unsigned int flags;
    } * Animations;

}LD_Animation_Object_Struct;//

void LD_Model_Object_Clear(void * LD_Model_)
{
    LD_Model_Object_Struct * LD_Model = (LD_Model_Object_Struct*) LD_Model_;
    if(LD_Model->Vertices!=NULL) {free(LD_Model->Vertices);}
    if(LD_Model->Normals!=NULL)  {free(LD_Model->Normals);}
    if(LD_Model->UV!=NULL)       {free(LD_Model->UV);}
    if(LD_Model->Indices!=NULL)  {free(LD_Model->Indices);}
    if(LD_Model->Joints!=NULL)   {free(LD_Model->Joints);}
    if(LD_Model->BlendingIndices!=NULL){free(LD_Model->BlendingIndices);}
    if(LD_Model->BlendingWeights!=NULL){free(LD_Model->BlendingWeights);}
}

void LD_Animation_Object_Clear(void * LD_Model_)
{
    LD_Animation_Object_Struct * LD_Anim = (LD_Animation_Object_Struct*) LD_Model_;
    free(LD_Anim->Poses);
    free(LD_Anim->Animations);
}


struct LD_3D_Struct
{
    unsigned int Models_Count;
    DoublyLinkedList(LD_Model_Object_Struct) * Models_LL;

    unsigned int Animations_Count;
    DoublyLinkedList(LD_Animation_Object_Struct*) * Animations_LL;

    unsigned int Sizeof_InstancesBuffer;
    struct LD_Instance_Struct
    {
        struct LD_Instance_Model{
            float Matrix[16];   //<- Model Matrix
             LD_Model_Object_Struct * Data; //<- Its a pointer to its unique model
        }Model;
	
	    struct LD_Instance_Animation{
            unsigned int Animations_Array_Size; //<- Multiple animations can be played.
            struct _LD_Instance_Animation_Descriptor{
                LD_Animation_Object_Struct * Data; //<- a pointer to the animation data.
                char FLAG;  //<-0: LSB is set for playing, when clear, it means freeze.
                // 1: Looping frames
                // 2: Reverse
                // 3: Frames Interpolation

                // 8: Ignore if set
                struct __Joint_Struct * Current_Pose;
                struct __Joint_Struct * Next_Pose; //<- Useful if Interpolated
    
                unsigned int CurrentAnimation;
                float CurrentFrame;
                float NextFrame;
                float Factor;
                float Speed;
            } * Animations_Array;
            //If more than one animation is being played, the data is added.

        }Animation;
	 
    } * InstancesBuffer;

    unsigned int Sizeof_VRAMBuffer;
    struct LD_VRAMBuffer_struct
    {
        char FLAG; //LSB is for Staticity, MSB is for draw Exclusion
        struct LD_Instance_Struct * First_instance;
        struct LD_Instance_Struct * Last_instance;
        GLenum Usage;
        unsigned int Sizeof_Vec3Buffers;
        unsigned int Sizeof_Vec2Buffers;
        unsigned int IndicesCount;
        unsigned int TexturesCount;
        GLuint VBO_Position;
        GLuint VBO_UV;
        GLuint VBO_Normals;
        GLuint VAO_PositionNormal;
        GLuint VAO_PositionUV;
        GLuint EBO;
        GLuint * Texture;
        struct Configuration_Struct
        {
            char Culling;
        } Configuration;
    } * VRAMBuffer;

} LD_3D;

typedef struct LD_VRAMBuffer_struct VRAMBufferStructdef;
typedef struct LD_Instance_Struct   InstanceStructdef;

void LD_3D_init()
{
        //GL_Utils_init();

    float Cero [3] = {0,0,0}; //<- Check for a XOR solution.

    _zero(*(int*)&LD_3D.Models_Count);
    _zero(*(int*)&LD_3D.Animations_Count);
    _zero(*(int*)&LD_3D.Sizeof_InstancesBuffer);
    _zero(*(int*)&LD_3D.Sizeof_VRAMBuffer);
    //memcpy(LD_3D.Light_System.Ambient_Color,Cero,sizeof(float)*3);

    LD_3D.Models_LL = SinglyLinkedList_Create();
    LD_3D.Animations_LL = DoublyLinkedList_Create();
    _NULL(*(int*)&LD_3D.InstancesBuffer);
    _NULL(*(int*)&LD_3D.VRAMBuffer);
}

LD_Model_Object_Struct * LD_3D_ModelsLinkedList_Append(void * Data_pointer)
{
     //The RAW model is memcpied
    void * Return = D_LinkedList_add(LD_3D.Models_LL,Data_pointer,sizeof(LD_Model_Object_Struct));
    LD_3D.Models_Count+=(Return != NULL);
    return (LD_Model_Object_Struct *)Return;
}

void LD_3D_ModelsLinkedList_Remove(LD_Model_Object_Struct * Model)
{
    DoublyLinkedList_remove(LD_3D.Models_LL,Model, LD_Model_Object_Clear);
    LD_3D.Models_Count-=(LD_3D.Models_Count>0);
}

void LD_3D_ModelsLinkedList_System_Delete()
{
    DoublyLinkedList_clear(LD_3D.Models_LL,LD_Model_Object_Clear);
    _zero(LD_3D.Models_Count);
}

LD_Animation_Object_Struct * LD_3D_AnimationsLinkedList_Append(void * Data_pointer)
{
    //Only the pointer is memcpied, not the entire struct
    void * Return = D_LinkedList_add(LD_3D.Animations_LL,&Data_pointer,sizeof(void*));
    LD_3D.Animations_Count+=(Return != NULL);
    return (LD_Animation_Object_Struct *)*(uintptr_t*)Return;
}

void LD_3D_AnimationsLinkedList_Remove(LD_Animation_Object_Struct * Model)
{
    DoublyLinkedList_remove(LD_3D.Animations_LL,Model, LD_Animation_Object_Clear);
    LD_3D.Animations_Count-=(LD_3D.Models_Count>0);
}

void LD_3D_AnimationsLinkedList_System_Delete()
{
    DoublyLinkedList_clear(LD_3D.Animations_LL,LD_Animation_Object_Clear);
    _zero(LD_3D.Animations_Count);
}

unsigned int LD_3D_InstanceBuffer_Set_capacity(unsigned int New_Instance_Capacity)
{
    LD_3D.InstancesBuffer = _resize(InstanceStructdef,LD_3D.InstancesBuffer,New_Instance_Capacity);
    if(LD_3D.InstancesBuffer == NULL){return 0;}

    LD_3D.Sizeof_InstancesBuffer=New_Instance_Capacity;

    return New_Instance_Capacity;
}

void LD_3D_Fill_Instance(InstanceStructdef * Instance_Ptr, LD_Model_Object_Struct * Model_Ptr)
{
    //memcpy(Instance_Ptr->Model.Matrix,Identity_Matrix_4x4,4*16);
    //printf("Inicio\n");
    M4IDENTITY(Instance_Ptr->Model.Matrix);
    //printM4(Instance_Ptr->Model.Matrix);
    /*
    memset(Instance_Ptr->Model.Matrix,0,16*4);
    Instance_Ptr->Model.Matrix[0]=1.f;
    Instance_Ptr->Model.Matrix[5]=1.f;
    Instance_Ptr->Model.Matrix[10]=1.f;
    Instance_Ptr->Model.Matrix[15]=1.f;
*/
    //printf("\n");
    //printM4(Instance_Ptr->Model.Matrix);

    

    //printf("Matrix: %p\n",Instance_Ptr->Model.Matrix);
    Instance_Ptr->Model.Data = Model_Ptr;
    Instance_Ptr->Animation.Animations_Array_Size = 0;
    Instance_Ptr->Animation.Animations_Array = NULL;
    //printf("Termino\n");
}

void LD_3D_Instance_Identity(InstanceStructdef * Instance_Ptr)
{M4IDENTITY(Instance_Ptr->Model.Matrix);}

void LD_3D_Instance_Scale(InstanceStructdef * Instance_Ptr,float * ScaleVector)
{
    Instance_Ptr->Model.Matrix[0]*=ScaleVector[0];
    Instance_Ptr->Model.Matrix[5]*=ScaleVector[1];
    Instance_Ptr->Model.Matrix[10]*=ScaleVector[2];
}

void LD_3D_Instance_Translate(InstanceStructdef * Instance_Ptr,float * Vector)
{
    float * ptr = &Instance_Ptr->Model.Matrix[12];
    V3ADD(ptr,ptr,Vector);
}

void LD_3D_Instance_Rotate_Euler(InstanceStructdef * Instance_Ptr,float * Axis,float Radians)
{
    float tmp[4], tmpM[16];
    EULERTOQUAT(tmp,Axis,Radians);
    float * ptr = Instance_Ptr->Model.Matrix;
    QUATTOM4(tmpM,tmp);
    //printf("Inicio\n");
    //printM4(tmpM);
   // printf("\n");
    //printM4(ptr);
    M4MUL(ptr,tmpM,ptr);
    //printf("\n");
    //printM4(ptr);
    //getchar();
}

char LD_3D_Instance_Set_Animations_Array(InstanceStructdef * Instance, unsigned int Capacity)
{
    struct _LD_Instance_Animation_Descriptor * tmp = 
    _new(struct _LD_Instance_Animation_Descriptor,Capacity);
    if(tmp==NULL){return 0;}
    memset(tmp,0,sizeof(struct _LD_Instance_Animation_Descriptor)*Capacity);
    Instance->Animation.Animations_Array=tmp;
    Instance->Animation.Animations_Array_Size=Capacity;
    return 1;
}
 
void LD_3D_Instance_Set_Animation(InstanceStructdef * Instance, unsigned int Index,
    LD_Animation_Object_Struct * Animation_Ptr,unsigned int Animation_Number, float Speed_Multiplier)
{
    struct _LD_Instance_Animation_Descriptor * Curr = Instance->Animation.Animations_Array+Index;
    Curr->FLAG = 0;
    Curr->Data = Animation_Ptr;
    Curr->CurrentAnimation=Animation_Number;
    Curr->CurrentFrame = 0;
    Curr->NextFrame = 0;
    Curr->Speed = Speed_Multiplier;
    Curr->Current_Pose=NULL;
    Curr->Next_Pose=NULL;
}

void LD_3D_Instance_Animation_Set_Flag(InstanceStructdef * Instance,unsigned int Index,char Flag)
{
    (Instance->Animation.Animations_Array+Index)->FLAG = Flag;
}

char * LD_3D_Instance_Animation_Flag(InstanceStructdef * Instance,unsigned int Index)
{
    return &(Instance->Animation.Animations_Array+Index)->FLAG;
}
 
void LD_3D_Instance_Clear(InstanceStructdef * Instance_Ptr)
{
    free(Instance_Ptr->Animation.Animations_Array);
    
}

void LD_3D_Instance_System_Delete()
{
    int * i = &LD_3D.Sizeof_InstancesBuffer;
    for(;i--;)
    {
        InstanceStructdef * tmp = LD_3D.InstancesBuffer+*i;
        LD_3D_Instance_Clear(tmp);
        free(tmp);
    }

    free(LD_3D.InstancesBuffer);
    _NULL(*(int*)&LD_3D.InstancesBuffer);
}

unsigned int LD_3D_VRAMBuffer_allocate(unsigned int New_number_of_buffers)
{
    LD_3D.VRAMBuffer =_resize(VRAMBufferStructdef,LD_3D.VRAMBuffer,New_number_of_buffers);
    if(LD_3D.VRAMBuffer == NULL){return 0;}

    array_foreach(VRAMBufferStructdef,ex,LD_3D.VRAMBuffer,New_number_of_buffers)
    {
	    ex->TexturesCount=0;
        {ex->FLAG=128;}
        ex->Configuration.Culling=1;
    }

    LD_3D.Sizeof_VRAMBuffer=New_number_of_buffers;
    return New_number_of_buffers;
}

unsigned int LD_3D_VRAMBuffer_Set_Texture_Capacity(VRAMBufferStructdef* VRAMBuffer, unsigned int Cap)
{
    GLuint * tmp = (GLuint*)malloc(sizeof(GLuint)*Cap);
    if(tmp==NULL){return 0;}
    VRAMBuffer->Texture = tmp;
    VRAMBuffer->TexturesCount=Cap;
    return Cap;
}

void LD_3D_VRAMBuffer_Set_Texture(VRAMBufferStructdef* VRAMBuffer, unsigned int Index,GLuint  Texture)
{
    VRAMBuffer->Texture[Index] = Texture;
}

void LD_3D_VRAMBuffer_Clear(VRAMBufferStructdef* VRAMBuffer)
{
    if(VRAMBuffer->TexturesCount > 0)
    {free(VRAMBuffer->Texture);}
    VRAMBuffer->FLAG |= 0x80;
    

    if(VRAMBuffer->IndicesCount > 0)
    {
        glDeleteBuffers(1,&VRAMBuffer->VBO_Position);
        glDeleteBuffers(1,&VRAMBuffer->VBO_UV);
        glDeleteBuffers(1,&VRAMBuffer->VBO_Normals);
        glDeleteBuffers(1,&VRAMBuffer->EBO);
        glDeleteVertexArrays(1,&VRAMBuffer->VAO_PositionNormal);
        glDeleteVertexArrays(1,&VRAMBuffer->VAO_PositionUV);
    }
    

    _zero(VRAMBuffer->Sizeof_Vec3Buffers);
    _zero(VRAMBuffer->Sizeof_Vec2Buffers);
    _zero(VRAMBuffer->IndicesCount);
    _zero(VRAMBuffer->TexturesCount);
}

void LD_3D_VRAMBuffer_System_Clear()
{
    int  i = LD_3D.Sizeof_VRAMBuffer;
    while( i --> 0)
    {
        //LD_3D_VRAMBuffer_Clear(LD_3D.VRAMBuffer+*i,1);
        LD_3D_VRAMBuffer_Clear(LD_3D.VRAMBuffer+i);
        
    }
}

void LD_3D_VRAMBuffer_System_Delete()
{
    LD_3D_VRAMBuffer_System_Clear();

    free(LD_3D.VRAMBuffer);
    _NULL(*(int*)&LD_3D.VRAMBuffer);
}


void LD_3D_Fill_VRAMBuffer(VRAMBufferStructdef * VRAMPtr, InstanceStructdef * InstancesBuffer_First, InstanceStructdef * InstancesBuffer_Last,char FLAG)
{
    VRAMPtr->FLAG=FLAG;
    VRAMPtr->First_instance = InstancesBuffer_First;
    VRAMPtr->Last_instance = InstancesBuffer_Last;

    int * so = &VRAMPtr->Sizeof_Vec3Buffers; _zero(*so);
    int * si = &VRAMPtr->Sizeof_Vec2Buffers; _zero(*si);
    int * vc = &VRAMPtr->IndicesCount; _zero(*vc);

    for (InstanceStructdef * ex = InstancesBuffer_First;ex<=InstancesBuffer_Last;ex++ )
    {
        *so += ex->Model.Data->Sizeof_Vec3Buffers;
        *si += ex->Model.Data->Sizeof_Vec2Buffers;
        *vc += ex->Model.Data->IndicesCount;
    }

    GLuint * VBO_Position = &VRAMPtr->VBO_Position;
    GLuint * VBO_UV = &VRAMPtr->VBO_UV;
    GLuint * VBO_Normals = &VRAMPtr->VBO_Normals;
    GLuint * VAO_PositionNormal = &VRAMPtr->VAO_PositionNormal;
    GLuint * VAO_PositionUV = &VRAMPtr->VAO_PositionUV;
    GLuint * EBO =&VRAMPtr->EBO;

    GLenum * Usage = &VRAMPtr->Usage;

    if((FLAG & 1) == 1){*Usage=GL_STATIC_DRAW;}
    else{*Usage=GL_DYNAMIC_DRAW;;}

    glGenBuffers(1,VBO_Position);
    glGenBuffers(1,VBO_UV);
    glGenBuffers(1,VBO_Normals);
    glGenBuffers(1,EBO);

    glBindBuffer(GL_ARRAY_BUFFER,*VBO_Position);
    glBufferData(GL_ARRAY_BUFFER,*so,NULL,*Usage);  
    uint8_t * tptr = (uint8_t*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    int sb;
    for (InstanceStructdef * iptr = InstancesBuffer_First;iptr<= InstancesBuffer_Last ;iptr++)
    {
        sb = iptr->Model.Data->Sizeof_Vec3Buffers;
        memcpy(tptr,iptr->Model.Data->Vertices,sb);
        tptr+=sb;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER,*VBO_Normals);
    glBufferData(GL_ARRAY_BUFFER,*so,NULL,*Usage);  
    tptr = (uint8_t*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    for (InstanceStructdef * iptr = InstancesBuffer_First;iptr<= InstancesBuffer_Last ;iptr++)
    {
        sb = iptr->Model.Data->Sizeof_Vec3Buffers;
        memcpy(tptr,iptr->Model.Data->Normals,sb);
        tptr+=sb;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER,*VBO_UV);
    glBufferData(GL_ARRAY_BUFFER,*si,NULL,*Usage);  
    tptr = (uint8_t*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    for (InstanceStructdef * iptr = InstancesBuffer_First;iptr<= InstancesBuffer_Last ;iptr++)
    {
        sb = iptr->Model.Data->Sizeof_Vec2Buffers;
        memcpy(tptr,iptr->Model.Data->UV,sb);
        tptr+=sb;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);


    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenVertexArrays(1,VAO_PositionNormal);
    glGenVertexArrays(1,VAO_PositionUV);

   // printf("-------\n");
    glBindVertexArray(*VAO_PositionNormal);
        glBindBuffer(GL_ARRAY_BUFFER,*VBO_Position);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4*3, (GLvoid*)(0));
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER,*VBO_Normals);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4*3, (GLvoid*)(0));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,*EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,*vc * 4,NULL,*Usage);  

        int * indptr = (int *) glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_WRITE_ONLY);
	//printf("indptr = %lx\n",indptr);
        int IndexIndex=0,IndexAcc=0;
        for (InstanceStructdef * iptr = InstancesBuffer_First;iptr<= InstancesBuffer_Last ;iptr++)
        {
            sb = iptr->Model.Data->IndicesCount;
	    //printf("IC: %d @ %lx \n",sb, iptr->Model.Data);
            for(int iii=0;iii<sb;iii++)
            {
                indptr[IndexIndex] = iptr->Model.Data->Indices[iii] + IndexAcc ;
                IndexIndex++;
		//printf("->%d\n",indptr[IndexIndex]);
            }
            IndexAcc+=iptr->Model.Data->VertexCount;
        }

    //printf("-------\n");
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    glBindVertexArray(0);

    glBindVertexArray(*VAO_PositionUV);

        glBindBuffer(GL_ARRAY_BUFFER,*VBO_Position);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4*3, (GLvoid*)(0));
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER,*VBO_UV);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*2, (GLvoid*)(0));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,*EBO);

    glBindVertexArray(0);

}

void __LD_3D_Update_Get_Frames(struct _LD_Instance_Animation_Descriptor * Animation,
float Delta,float BASE_FPS)
{
    char FLAG = Animation->FLAG;
    //char IS_IGNORED = _checkbits(FLAG,1<7);
    struct LD_Animation_Object_Struct * Data = Animation->Data;
    if(Data==NULL){return;}
    struct ___Anim_Struct * ActualAnimation = Data->Animations+Animation->CurrentAnimation;
    

    char IS_ANIMATED = _checkbits(FLAG,1);

    char IS_REVERSED = _checkbits(FLAG,4);
    #define IS_NOT_REVERSED !IS_REVERSED

    char IS_LOOPING = _checkbits(FLAG,2);
    #define IS_NOT_LOOPING !IS_LOOPING

     Animation->CurrentFrame=Animation->NextFrame;

    unsigned int MaxFrame = ActualAnimation->num_frames-1;
    unsigned int CurrFrame = (int) Animation->CurrentFrame;
    #define IS_IN_BOUNDS (CurrFrame>=0 && CurrFrame<=MaxFrame)
    CurrFrame=(CurrFrame*IS_IN_BOUNDS)+(!IS_IN_BOUNDS*MaxFrame );

    unsigned int ATEND = CurrFrame == MaxFrame;
    unsigned int ATBEG = CurrFrame == 0;

   

    unsigned int NextFrame = (CurrFrame+(!ATEND))*(!ATEND);



    float FrameRate = ActualAnimation->framerate;
    float FollFrame =Animation->CurrentFrame+
    (
        (1.f+(-2.f * (IS_REVERSED*1.f)))*
        (FrameRate/BASE_FPS * Delta)*Animation->Speed
    ) * (IS_ANIMATED*1.f);
    
    float fCurrFrame = Animation->CurrentFrame;
    float Factor = (fCurrFrame - (float) (int) fCurrFrame);
    Factor = (Factor * (Factor!=1.f)*1.f);

    
    float MaxFramef = (float) MaxFrame+1;
    char ENDED = (FollFrame >= MaxFramef);
    char ENDED_REVERSED = (IS_REVERSED && FollFrame<=0);
    if(IS_LOOPING && ( ENDED || ENDED_REVERSED ))
    {
        FollFrame+=MaxFramef*((-1.f*(ENDED*1.f)) + ((ENDED_REVERSED*1.f)));
        if(FollFrame >= MaxFramef){FollFrame=0;}
    }

    if(IS_NOT_LOOPING)
    {
        if((FollFrame >= MaxFramef-1))
        {
            FollFrame =MaxFramef-1;
        }
        if((FollFrame<=0))
        {
            FollFrame=0;
        }
    }
    

   
   

    

    Animation->Factor= Factor;
    Animation->NextFrame= FollFrame;

/*
    printf("(%x -%d-)=(%x)(%x) %d -> %d | ",Animation->FLAG,Animation->FLAG,IS_REVERSED,IS_LOOPING,CurrFrame,NextFrame);
    printf("%f -> %f | ",Animation->CurrentFrame,FollFrame);
    printf("f: %f\n",Factor);
     */

    Animation->Current_Pose = Data->Poses+(Data->PosesCount*CurrFrame);
    Animation->Next_Pose  = Data->Poses+(Data->PosesCount*NextFrame);



    #undef IS_IN_BOUNDS
    #undef IS_REVERSED
    #undef IS_NOT_LOOPING
}

void __LD_3D_Update_Calculate_Pose_Position(struct __Joint_Struct * MJoint,
    void * QNew, void * TNew,void * Original_Vector, float  Weight, void *Result)
{
    float QDiff[4]; 
    //Quaternion_Diff_Unit(MJoint->rotate,QNew,QDiff);
    UQUATDIFF(QDiff,MJoint->rotate,QNew);
    float _temp_temp[3]; //<-Temporal transform for this cycle.
    V3SUB(_temp_temp,(float*)Original_Vector,(float*)MJoint->translate); 
    //QuaternionRotateV3(QDiff,_temp_temp,_temp_temp);

//printV3(((float*)_temp_temp));printf("\n");

    QUATROTV3(_temp_temp,QDiff,_temp_temp);

   // printV3(((float*)_temp_temp));printf("\n");

    V3ADD(_temp_temp,TNew,_temp_temp);
    V3MULSS(_temp_temp,_temp_temp,Weight);
    V3ADD(Result,_temp_temp,Result);
/*
    printV4(((float*)MJoint->rotate));printf("\n");
    printV4(((float*)QNew));printf("\n");
    printV4(((float*)QDiff));printf("\n");
    printV3(((float*)Original_Vector)); printf("\n");
    printV3(((float*)MJoint->translate)); printf("\n");
    printV3(((float*)TNew)); printf("\n");
    printV3(((float*)Result));printf("\n");
  getchar();
     */
}

void __LD_3D_Update_Calculate_Pose_Normal(struct __Joint_Struct * MJoint,
    void * QNew, void * TNew,void * Original_Vector, float  Weight, void *Result)
{
    float _temp_temp[3]; //<-Temporal transform for this cycle.
    float QDiff[4]; 
    //Quaternion_Diff_Unit(MJoint->rotate,QNew,QDiff);
    UQUATDIFF(QDiff,MJoint->rotate,QNew);
    //QuaternionRotateV3(QDiff,Original_Vector,_temp_temp);
    QUATROTV3(_temp_temp,QDiff,Original_Vector);
    V3MULSS(_temp_temp,_temp_temp,Weight);
    V3ADD((float*)Result,(float*)_temp_temp,(float*)Result);
}

void __LD_3D_Update_Calculate_Pose(float * Original_Vector,
unsigned char * BWeight,unsigned char * BIndex,struct __Joint_Struct * MJoints,
struct LD_Instance_Animation * Anim, void * Result_Vector,
void (*Processing_Function) (struct __Joint_Struct *,void*,void*,void*,float,void*))
{
    float Pivot[3]; memcpy(Pivot,Original_Vector,sizeof(float)*3);
    Original_Vector = Pivot;

    

    array_foreach(struct _LD_Instance_Animation_Descriptor,CurrAnim,
            Anim->Animations_Array,Anim->Animations_Array_Size)
    {
        //printf("->IGN: %x\n",CurrAnim->FLAG);
        if(!(_checkbits(CurrAnim->FLAG,128)) && CurrAnim->Data!=NULL)
        {
        struct __Joint_Struct * AJoints=CurrAnim->Current_Pose;
        struct __Joint_Struct * NJoints=CurrAnim->Next_Pose;
        float Frame = CurrAnim->CurrentFrame;

        char INTERPOLATION = _checkbits(CurrAnim->FLAG,8);
        float Result[3]={0,0,0};
        float CurW = 0;
        float Factor = CurrAnim->Factor;

        //printf("V: (%f,%f,%f)\n",Vrexp->x,Vrexp->y,Vrexp->z);
        for(char _w_ = 0;_w_<4&&CurW<1.f ;_w_++)
        {
            float WW = BWeight[_w_] / 255.f;CurW+=WW;
            unsigned char BB = BIndex[_w_];
            struct __Joint_Struct * AJoint = AJoints+BB; //<- Current Pose
            struct __Joint_Struct * NJoint = NJoints+BB; //<- Next Pose
            struct __Joint_Struct * MJoint = MJoints+BB; //<- Base Pose

            float QNew[4]; memcpy(QNew,AJoint->rotate,sizeof(float)*4);          
            float TNew[3]; memcpy(TNew,AJoint->translate,sizeof(float)*3); 
            
            if(INTERPOLATION)
            {
                V3LERP(TNew,TNew,NJoint->translate,Factor);
                __QuatInterpolation(QNew,QNew,NJoint->rotate,Factor);
            }

           Processing_Function(MJoint,QNew,TNew,Original_Vector,WW,Result);
        }
        memcpy(Original_Vector,Result,sizeof(float)*3);
        }
    }


    memcpy(Result_Vector,Pivot,sizeof(float)*3);
}

void LD_3D_Update(float Delta, float BASE_FPS)
{
    array_foreach(VRAMBufferStructdef,Vex,LD_3D.VRAMBuffer,LD_3D.Sizeof_VRAMBuffer)
    {
        if((Vex->FLAG & 1) == 0 & (Vex->FLAG & 0x80) == 0) //If the buffer is non-static and drawable
        {      
            
                /* Update animations, update poses pointers*/
            array_range(InstanceStructdef,Iexp,Vex->First_instance,Vex->Last_instance)
            {         
                struct LD_Instance_Animation * Anim = &Iexp->Animation;
                
                struct _LD_Instance_Animation_Descriptor * Anim_Arr = Anim->Animations_Array;
                            
                /*If the array is null, then its size must be 0*/
                array_foreach(struct _LD_Instance_Animation_Descriptor,Aexp,
                                Anim_Arr,Anim->Animations_Array_Size)
                {        
                    __LD_3D_Update_Get_Frames(Aexp,Delta,BASEFPS);
                }
            }


		    glBindBuffer(GL_ARRAY_BUFFER,Vex->VBO_Position);
            glBufferData(GL_ARRAY_BUFFER,Vex->Sizeof_Vec3Buffers,NULL,Vex->Usage);
            float * ptr =(float*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
            InstanceStructdef * Iend =  Vex->Last_instance; //Check this line for a better solution
            for(InstanceStructdef * Iexp = Vex->First_instance; Iexp <= Iend; Iexp++)
            {
                struct LD_Model_Object_Struct * Model_Data = Iexp->Model.Data;
                struct LD_Instance_Animation * Anim = &Iexp->Animation;
                            
                unsigned char * BIndex = Model_Data->BlendingIndices;
                unsigned char * BWeight = Model_Data->BlendingWeights;

                //Dynamic Positions update//
                struct __Joint_Struct * MJoints= Model_Data->Joints;          

                array_foreach(struct __Vec3_Struct,Vrexp,
                                Model_Data->Vertices,Model_Data->VertexCount)
                {
                    
                    // Animation
                    float _temp[3]={0,0,0}; //<- Check for a XOR solution.


                    __LD_3D_Update_Calculate_Pose((float*)Vrexp,BWeight,
                    BIndex,MJoints,Anim,_temp,
                    __LD_3D_Update_Calculate_Pose_Position);
                    BIndex+=4;
                    BWeight+=4;

                    //Apply entire transformation matrix
                    __M4x4V4_PseudoV3_W1_MUL_(ptr,Iexp->Model.Matrix,_temp);
                    ptr+=3;
                   
                }
            }   
            glUnmapBuffer(GL_ARRAY_BUFFER);
            

            glBindBuffer(GL_ARRAY_BUFFER,Vex->VBO_Normals);
            glBufferData(GL_ARRAY_BUFFER,Vex->Sizeof_Vec3Buffers,NULL,Vex->Usage);
            ptr =(float*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
            Iend =  Vex->Last_instance; //Check this line for a better solution
            for(InstanceStructdef * Iexp = Vex->First_instance; Iexp <= Iend; Iexp++)
            {
                struct LD_Model_Object_Struct * Model_Data = Iexp->Model.Data;
            struct LD_Instance_Animation * Anim = &Iexp->Animation;
                        
            unsigned char * BIndex = Model_Data->BlendingIndices;
            unsigned char * BWeight = Model_Data->BlendingWeights;

            //Dynamic Positions update//
            struct __Joint_Struct * MJoints= Model_Data->Joints;    

            array_foreach(struct __Vec3_Struct,Vrexp,
                            Model_Data->Normals,Model_Data->VertexCount)
            {
                        // Animation
                    float _temp[3]={0,0,0}; //<- Check for a XOR solution.

                    __LD_3D_Update_Calculate_Pose((float*)Vrexp,BWeight,
                    BIndex,MJoints,Anim,_temp,
                    __LD_3D_Update_Calculate_Pose_Normal);
                    BIndex+=4;
                    BWeight+=4;

                    __M4x4_PseudoM3x3_V4_PseudoV3_W1_MUL_(ptr,Iexp->Model.Matrix,_temp);
                    ptr+=3;                          
                }               
            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
            glBindBuffer(GL_ARRAY_BUFFER,0);
        }
    } 
}
    /****************************/



struct LD_Directional_Light_Struct
{
    struct {float x,y,z;}Direction;
    float Factor;
    struct {float r,g,b,a;} Color;
};

struct LD_3D_Lighting_Struct
{
    float Ambient[3];
    unsigned int DirectionalsCount;
    DoublyLinkedList(struct LD_Directional_Light_Struct) * Directionals_LL;

} LD_3D_Lighting;

void LD_3D_Lighting_Init()
{
    memset(LD_3D_Lighting.Ambient,0,sizeof(float)*3);
    LD_3D_Lighting.Directionals_LL = DoublyLinkedList_Create();
}

void LD_3D_Ambient_Set(float r, float g, float b)
{LD_3D_Lighting.Ambient[0]=r; LD_3D_Lighting.Ambient[1]=g;LD_3D_Lighting.Ambient[2]=b;}

struct LD_Directional_Light_Struct * LD_3D_Directional_append(float * Direction, float Factor, float * RGBA)
{

    struct LD_Directional_Light_Struct tmp;
    memcpy(&tmp.Direction,Direction,sizeof(float)*3);
    tmp.Factor = Factor;
    memcpy(&tmp.Color,RGBA,sizeof(float)*4);
    
    void * Return = DoublyLinkedList_add(LD_3D_Lighting.Directionals_LL,tmp);
    
    LD_3D_Lighting.DirectionalsCount+=(Return != NULL);

    return (struct LD_Directional_Light_Struct *)Return;
}

void LD_3D_Directional_remove(struct LD_Directional_Light_Struct * Directional_Light)
{DoublyLinkedList_remove(LD_3D_Lighting.Ambient,Directional_Light,NULL);}

void LD_3D_Directional_Clear_all()
{ DoublyLinkedList_clear(LD_3D_Lighting.Directionals_LL,NULL);}


/************ Defferred Lighting process ******/


struct LD_3D_DefferredLighting_Struct
{
    struct LD_3D_DefferredLighting_Buffers_Struct
    {
        GLuint GeometryBuffer;
        GLuint LightBuffer;
        GLuint ColorBuffer;

        GLuint FrameBuffer;
        GLuint DepthBuffer;
    }Buffers;
    struct LD_3D_DefferredLighting_Configuration
    {
        struct LD_3D_DefferredLighting_Configuration_Options
        {
            int Width; 
            int Height;

        }Options;

        struct LD_3D_DefferredLighting_Configuration_GBuffer
        {
            GLuint Shader;
            struct LD_3D_DefferredLighting_Uniform_Locations_GBuffer
            {
                unsigned int ProjectionViewMatrix;
            } Uniforms;
        }GBuffer;

        struct LD_3D_DefferredLighting_Configuration_LBuffer
        {
            struct LD_3D_DefferredLighting_Configuration_LBuffer_Directional
            {
                GLuint Shader;
                struct LD_3D_DefferredLighting_Uniform_Locations_LBuffer
                {
                    unsigned int GBuffer;
                    unsigned int DirectionAndFactor;
                    unsigned int RGBA;
                } Uniforms;
            }Directional;


        }LBuffer;    

        struct LD_3D_DefferredLighting_Configuration_CBuffer
        {
            GLuint Shader;
            struct LD_3D_DefferredLighting_Uniform_Locations_CBuffer
            {     
                    unsigned int ProjectionViewMatrix;
                    unsigned int LBuffer;
                    unsigned int TexturesCount;
                    unsigned int * Textures;
            } Uniforms;
        }CBuffer;
    }Configuration;

}LD_3D_DL;


void LD_3D_GBuffer_Config(GLuint Shader)
{
    LD_3D_DL.Configuration.GBuffer.Shader = Shader;
    LD_3D_DL.Configuration.GBuffer.Uniforms.ProjectionViewMatrix = 
        glGetUniformLocation(Shader,"ProjectionViewMatrix");
}

void LD_3D_LBuffer_Config(
    GLuint DirShader
                            )
{
    //**Directional Light**//
    LD_3D_DL.Configuration.LBuffer.Directional.Shader = DirShader;
    LD_3D_DL.Configuration.LBuffer.Directional.Uniforms.GBuffer = 
        glGetUniformLocation(DirShader,"GBuffer");
    LD_3D_DL.Configuration.LBuffer.Directional.Uniforms.DirectionAndFactor =
        glGetUniformLocation(DirShader,"DirectionAndFactor");
    LD_3D_DL.Configuration.LBuffer.Directional.Uniforms.RGBA =
        glGetUniformLocation(DirShader,"RGBA");
}

void LD_3D_CBuffer_Config(GLuint Shader,GLuint* Textures,unsigned int TexturesCount)
{
    LD_3D_DL.Configuration.CBuffer.Shader = Shader;
    LD_3D_DL.Configuration.CBuffer.Uniforms.LBuffer = 
        LD_3D_DL.Buffers.LightBuffer;
    LD_3D_DL.Configuration.CBuffer.Uniforms.ProjectionViewMatrix =
        glGetUniformLocation(Shader,"ProjectionViewMatrix");
    LD_3D_DL.Configuration.CBuffer.Uniforms.TexturesCount = TexturesCount;
    LD_3D_DL.Configuration.CBuffer.Uniforms.Textures = Textures;
}

void LD_3D_DefferredLighting_Init(GLsizei WIDTH,GLsizei HEIGHT)
{
    LD_3D_DL.Configuration.Options.Width = WIDTH;
    LD_3D_DL.Configuration.Options.Height = HEIGHT;
    
    glGenTextures(1,&LD_3D_DL.Buffers.GeometryBuffer);
    glBindTexture(GL_TEXTURE_2D,LD_3D_DL.Buffers.GeometryBuffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,WIDTH,HEIGHT,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);

    glGenTextures(1,&LD_3D_DL.Buffers.LightBuffer);
    glBindTexture(GL_TEXTURE_2D,LD_3D_DL.Buffers.LightBuffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,WIDTH,HEIGHT,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);

    glGenTextures(1,&LD_3D_DL.Buffers.ColorBuffer);
    glBindTexture(GL_TEXTURE_2D,LD_3D_DL.Buffers.ColorBuffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,WIDTH,HEIGHT,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);


    glGenFramebuffers(1,&LD_3D_DL.Buffers.FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,LD_3D_DL.Buffers.FrameBuffer);

    glGenTextures(1,&LD_3D_DL.Buffers.DepthBuffer);
    glBindTexture(GL_TEXTURE_2D,LD_3D_DL.Buffers.DepthBuffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,WIDTH,HEIGHT,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,LD_3D_DL.Buffers.DepthBuffer,0);
  
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

//** Geometry Pass **//
void LD_3D_Draw_G_Pass() 
{
        //*****Models are about to be drawn*****//
	//*****In fact, the buffers are the ones about to be drawed*****//
    VRAMBufferStructdef * Vex =LD_3D.VRAMBuffer;
    VRAMBufferStructdef * Vend = Vex + LD_3D.Sizeof_VRAMBuffer;

    glDisable(GL_BLEND);
    for(;Vex<Vend;Vex++)
    {    
       
        if((Vex->FLAG & 0x80) == 0) //Si el búfer es dibujable
        {       
               
            glBindVertexArray(Vex->VAO_PositionNormal);
            glDrawElements(GL_TRIANGLES,Vex->IndicesCount,GL_UNSIGNED_INT,0);
        }
    }
    glEnable(GL_BLEND);
}

//** Light Pass **//
void LD_3D_Draw_L_Pass(GLuint GBuffer_Loc,GLuint GeometryBuffer,
                       GLuint DIR_DirFactor_loc,GLuint DIR_RGBA_loc,
                       GLenum Blend_sFactor, GLenum Blend_dFactor)
{


    //** Ambient Color **//
    glClearColor(LD_3D_Lighting.Ambient[0],LD_3D_Lighting.Ambient[1],
                        LD_3D_Lighting.Ambient[2],1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,GeometryBuffer);
    glUniform1i(GBuffer_Loc,0);
    GL_FB_Bind();

    glBlendFunc(GL_ONE,GL_ONE);
    //** Directional Light **//
    glUseProgram(LD_3D_DL.Configuration.LBuffer.Directional.Shader);

    if(LD_3D_Lighting.DirectionalsCount > 0)
    {
        DoublyLinkedList_foreach(DIRL,LD_3D_Lighting.Directionals_LL)
        {
            glUniform4fv(DIR_DirFactor_loc,1,(GLfloat* )
            & ((struct LD_Directional_Light_Struct*) DIRL)->Direction);
            glUniform4fv(DIR_RGBA_loc,1,(GLfloat* )
            & ((struct LD_Directional_Light_Struct*)DIRL)->Color);
            GL_FB_Draw();
        }
    }


    glBlendFunc(Blend_sFactor,Blend_dFactor);
}

//** Color Pass **//
void LD_3D_Draw_C_Pass(GLuint * Textures_Loc,unsigned int NumLocs,
                        GLuint LighBuffer_Loc,GLuint LightBuffer)
{
        //*****Models are about to be drawn*****//
	//*****In fact, are the buffers the ones about to be drawed*****//
    VRAMBufferStructdef * Vex =LD_3D.VRAMBuffer;
    VRAMBufferStructdef * Vend = Vex + LD_3D.Sizeof_VRAMBuffer;
 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,LightBuffer);
    glUniform1i(LighBuffer_Loc,0);

  #include <stdint-gcc.h>

    for(;Vex<Vend;Vex++)
    {   
        if((Vex->FLAG & 0x80) == 0) //Si el búfer es dibujable
        {    
                 
            char Culling = Vex->Configuration.Culling;

            void (*Func)(GLenum) = (void(*)(GLenum))  
            ((intptr_t)glEnable*Culling)+((intptr_t)glDisable*!Culling);
            Func(GL_CULL_FACE);



            for(int i = 0;i<Vex->TexturesCount;i++)
            {
                GLuint LocalTexture = Vex->Texture [i];
                LocalTexture*=(i<NumLocs);
                glActiveTexture(GL_TEXTURE1+i);
                glBindTexture(GL_TEXTURE_2D,LocalTexture);
                glUniform1i(Textures_Loc[i],1+i);
            }
            
           
            glBindVertexArray(Vex->VAO_PositionUV);
            glDrawElements(GL_TRIANGLES,Vex->IndicesCount,GL_UNSIGNED_INT,0);

            for(int i = 0;i<Vex->TexturesCount;i++)
            {
                glActiveTexture(GL_TEXTURE1+i);
                glBindTexture(GL_TEXTURE_2D,0);
                glUniform1i(Textures_Loc[i],1+i);
            }
        }
    }
}


GLuint LD_3D_DefferredLighting_Draw(void * ProjectionViewMatrix,
    GLenum Blend_sFactor, GLenum Blend_dFactor)
{   
        glBindFramebuffer(GL_FRAMEBUFFER,LD_3D_DL.Buffers.FrameBuffer);

        glViewport(0,0,
        LD_3D_DL.Configuration.Options.Width,
        LD_3D_DL.Configuration.Options.Height);

        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,
                                LD_3D_DL.Buffers.GeometryBuffer,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(LD_3D_DL.Configuration.GBuffer.Shader);
        glUniformMatrix4fv(LD_3D_DL.Configuration.GBuffer.Uniforms.ProjectionViewMatrix,
                            1,0,ProjectionViewMatrix);
    
        LD_3D_Draw_G_Pass();
        



        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, 
                                LD_3D_DL.Buffers.LightBuffer,0);
        LD_3D_Draw_L_Pass(LD_3D_DL.Configuration.LBuffer.Directional.Uniforms.GBuffer,
            LD_3D_DL.Buffers.GeometryBuffer,
            LD_3D_DL.Configuration.LBuffer.Directional.Uniforms.DirectionAndFactor,
            LD_3D_DL.Configuration.LBuffer.Directional.Uniforms.RGBA,
            Blend_sFactor,Blend_dFactor);
        



        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,
                                LD_3D_DL.Buffers.ColorBuffer,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(LD_3D_DL.Configuration.CBuffer.Shader);
        glUniformMatrix4fv(LD_3D_DL.Configuration.CBuffer.Uniforms.ProjectionViewMatrix,
                            1,0,ProjectionViewMatrix);
        LD_3D_Draw_C_Pass(LD_3D_DL.Configuration.CBuffer.Uniforms.Textures,
                        LD_3D_DL.Configuration.CBuffer.Uniforms.TexturesCount,
                        LD_3D_DL.Configuration.CBuffer.Uniforms.LBuffer,
                        LD_3D_DL.Buffers.LightBuffer);
        


        glBindFramebuffer(GL_FRAMEBUFFER,0);

        return LD_3D_DL.Buffers.ColorBuffer;
}


#undef __QuatInterpolation
#undef __M4x4_PseudoM3x3_V4_PseudoV3_W1_MUL_
#undef __M4x4V4_PseudoV3_W1_MUL_
#endif
