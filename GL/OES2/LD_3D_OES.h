#ifndef LD_3D_OES_H
#define LD_3D_OES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../libLDExtra/LD_Stack_Queue.h"
#include "../../libLDExtra/LD_Tricks.h"

/*  The same identity matrix can be defined in many othar places
        if it's not defined, define it here */
    #ifndef __IDENTITY_MATRIX_4X4_FLOAT_
    #define __IDENTITY_MATRIX_4X4_FLOAT_
    #define __IDENTITY_MATRIX_4X4_FLOAT_LD_DEF_L_OES_H
        extern float Identity_Matrix_4x4[16];
    #endif  

    //It's similar to IQM_RAW_Struct, but it doesn't have the first two integers
    //One Model per Instance
typedef struct LD_Model_Object_Struct 
{
    int VertexCount;
	int TrianglesCount;
	int Sizeof_Vec3Buffers;
    int Sizeof_Vec2Buffers;
    int IndicesCount;
    int JointsCount;

    struct __Vec3_Struct{float x,y,z;} * Vertices;
    struct __Vec3_Struct * Normals;
    struct __UV_Struct{float u,v;} * UV;
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

}LD_Animation_Object_Struct;

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

    unsigned int Sizeof_InstacesBuffer;
    struct LD_Instance_Struct
    {
        float Matrix[16];
        LD_Model_Object_Struct * Model_Data_ptr; //<- Its a pointer to its unique model
	
	struct LD_Instance_Anim_Descriptor_Struct{
	    LD_Animation_Object_Struct * Data; //<- a pointer to the current animation
	    char FLAG; //<-0: LSB is set for playing, when clear, it means freeze
			// 1: Looping frames
			// 2: Reverse
			// 3: Frames Interpolation
	    unsigned int CurrentAnimation;
	    float CurrentFrame;
	    float Speed;
	} Animation;
	 
    } * InstacesBuffer;

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
        GLuint VBO_Position;
        GLuint VBO_UV;
        GLuint VBO_Normals;
        GLuint VAO_PositionNormal;
        GLuint VAO_PositionUV;
        GLuint EBO;
    } * VRAMBuffer;

    struct LD_Light_System_Struct
    {
        float Ambient_Color[3];

        struct LD_Directional_Lights_System_Struct
        {
            unsigned int Lights_Count;
            struct LD_Dir_Light_Struct
            {
                float Vector [3]; float Intensity;
                float Color [3];
            }* LightsBuffer;
        }Directional_Lights_System;

    } Light_System;

} LD_3D;

typedef struct LD_VRAMBuffer_struct VRAMBufferStructdef;
typedef struct LD_Instance_Struct   InstanceStructdef;
typedef struct LD_Dir_Light_Struct  DirLightStructdef;

void LD_3D_init()
{
    float Cero [3] = {0,0,0};

    LD_3D.Models_Count=0;
    LD_3D.Animations_Count=0;
    LD_3D.Sizeof_InstacesBuffer=0;
    LD_3D.Sizeof_VRAMBuffer=0;
    memcpy(LD_3D.Light_System.Ambient_Color,Cero,sizeof(float)*3);
    LD_3D.Light_System.Directional_Lights_System.Lights_Count = 0;

    LD_3D.Models_LL = SinglyLinkedList_Create();
    LD_3D.Animations_LL = DoublyLinkedList_Create();
    LD_3D.InstacesBuffer=NULL;
    LD_3D.VRAMBuffer=NULL;
    LD_3D.Light_System.Directional_Lights_System.LightsBuffer = NULL;
}

LD_Model_Object_Struct * LD_3D_ModelsLinkedList_Append(void * Data_pointer)
{
    //The RAW model is memcpied
    void * Return = D_LinkedList_add(LD_3D.Models_LL,Data_pointer,sizeof(LD_Model_Object_Struct));
    LD_3D.Models_Count++;
    return (LD_Model_Object_Struct *)Return;
}

void LD_3D_ModelsLinkedList_Remove(LD_Model_Object_Struct * Model)
{
    DoublyLinkedList_remove(LD_3D.Models_LL,Model, LD_Model_Object_Clear);
    LD_3D.Models_Count--;
}

void LD_3D_ModelsLinkedList_System_Delete(struct LD_Model_Object_Struct * Stack_ptr)
{
    DoublyLinkedList_clear(LD_3D.Models_LL,LD_Model_Object_Clear);
    LD_3D.Models_Count=0;
}

LD_Animation_Object_Struct * LD_3D_AnimationsLinkedList_Append(void * Data_pointer)
{
    //Only the pointer is memcpied, not the entire struct
    void * Return = D_LinkedList_add(LD_3D.Animations_LL,&Data_pointer,sizeof(void*));
    LD_3D.Animations_Count++;
    return (LD_Animation_Object_Struct *)*(uintptr_t*)Return;
}

void LD_3D_AnimationsLinkedList_Remove(LD_Animation_Object_Struct * Model)
{
    DoublyLinkedList_remove(LD_3D.Animations_LL,Model, LD_Animation_Object_Clear);
    LD_3D.Animations_Count--;
}

void LD_3D_AnimationsLinkedList_System_Delete(struct LD_Animation_Object_Struct * Stack_ptr)
{
    DoublyLinkedList_clear(LD_3D.Animations_LL,LD_Animation_Object_Clear);
    LD_3D.Animations_Count=0;
}

unsigned int LD_3D_InstaceBuffer_Set_capacity(unsigned int New_Instance_Capacity)
{
    LD_3D.InstacesBuffer = realloc(LD_3D.InstacesBuffer,sizeof(InstanceStructdef) * New_Instance_Capacity);  

    if(LD_3D.InstacesBuffer == NULL){return 0;}
    LD_3D.Sizeof_InstacesBuffer=New_Instance_Capacity;

    return New_Instance_Capacity;
}

void LD_3D_Fill_Instance(InstanceStructdef * Instance_Ptr, LD_Model_Object_Struct * Model_Ptr)
{
    memcpy(Instance_Ptr->Matrix,Identity_Matrix_4x4,4*16);
    Instance_Ptr->Model_Data_ptr = Model_Ptr;
//    Instance_Ptr->Joints = malloc(sizeof(struct __Joint_Struct)*Model_Ptr->JointsCount);
    Instance_Ptr->Animation.Data = NULL;
 //   Instance_Ptr->Animation.Joints = NULL; //It points to an animation specific pose
}

void LD_3D_Instance_Set_Animation(InstanceStructdef * Instance, 
    LD_Animation_Object_Struct * Animation_Ptr,unsigned int Animation_Number, float Speed_Multiplier)
{
    Instance->Animation.FLAG = 1;
    Instance->Animation.Data = Animation_Ptr;
    Instance->Animation.CurrentAnimation=Animation_Number;
    Instance->Animation.CurrentFrame = 0;
    Instance->Animation.Speed = Speed_Multiplier;
}

void LD_3D_Instance_Replicate(InstanceStructdef * From, InstanceStructdef * To, InstanceStructdef * Master_Copy)
{
    for(;From<=To;From++)
    {
        memcpy(From,Master_Copy,sizeof(InstanceStructdef));
    }
}

int LD_3D_VRAMBuffer_allocate(int New_number_of_buffers)
{
    if(LD_3D.VRAMBuffer == NULL)
    {
        LD_3D.VRAMBuffer = (VRAMBufferStructdef *)
        malloc(sizeof(VRAMBufferStructdef) * New_number_of_buffers);
    }
    else
    { 
        LD_3D.VRAMBuffer = (VRAMBufferStructdef *) 
        realloc(LD_3D.VRAMBuffer,sizeof(VRAMBufferStructdef) * New_number_of_buffers);
    }

    if(LD_3D.VRAMBuffer == NULL){return -1;}
    for(VRAMBufferStructdef * ex =LD_3D.VRAMBuffer;
     ex < LD_3D.VRAMBuffer+New_number_of_buffers;ex++)
    {ex->FLAG=128;}
    LD_3D.Sizeof_VRAMBuffer=New_number_of_buffers;
    return LD_3D.Sizeof_VRAMBuffer;
}

void LD_3D_VRAMBuffer_Clear(VRAMBufferStructdef* VRAMBuffer,char glDelete)
{
    VRAMBuffer->Sizeof_Vec3Buffers=0;
    VRAMBuffer->Sizeof_Vec2Buffers=0;
    VRAMBuffer->IndicesCount=0;
    VRAMBuffer->FLAG |= 0x80;
    if(glDelete)
    {
        glDeleteBuffers(1,&VRAMBuffer->VBO_Position);
        glDeleteBuffers(1,&VRAMBuffer->VBO_UV);
        glDeleteBuffers(1,&VRAMBuffer->VBO_Normals);
        glDeleteBuffers(1,&VRAMBuffer->EBO);
        glDeleteVertexArrays(1,&VRAMBuffer->VAO_PositionNormal);
        glDeleteVertexArrays(1,&VRAMBuffer->VAO_PositionUV);
    }
}

void LD_3D_VRAMBuffer_Delete()
{
    int * i = &LD_3D.Sizeof_VRAMBuffer;
    for(;i--;)
    {
        LD_3D_VRAMBuffer_Clear(LD_3D.VRAMBuffer+*i,1);
    }

    free(LD_3D.VRAMBuffer);
    LD_3D.VRAMBuffer = NULL;
}


void LD_3D_Fill_VRAMBuffer(VRAMBufferStructdef * VRAMPtr, InstanceStructdef * InstacesBuffer_First, InstanceStructdef * InstacesBuffer_Last,char FLAG)
{
    VRAMPtr->FLAG=FLAG;
    VRAMPtr->First_instance = InstacesBuffer_First;
    VRAMPtr->Last_instance = InstacesBuffer_Last;

    int * so = &VRAMPtr->Sizeof_Vec3Buffers; *so=0;
    int * si = &VRAMPtr->Sizeof_Vec2Buffers; *si=0;
    int * vc = &VRAMPtr->IndicesCount; *vc=0;

    for (InstanceStructdef * ex = InstacesBuffer_First;ex<=InstacesBuffer_Last;ex++ )
    {
        *so += ex->Model_Data_ptr->Sizeof_Vec3Buffers;
        *si += ex->Model_Data_ptr->Sizeof_Vec2Buffers;
        *vc += ex->Model_Data_ptr->IndicesCount;
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
    for (InstanceStructdef * iptr = InstacesBuffer_First;iptr<= InstacesBuffer_Last ;iptr++)
    {
        sb = iptr->Model_Data_ptr->Sizeof_Vec3Buffers;
        memcpy(tptr,iptr->Model_Data_ptr->Vertices,sb);
        tptr+=sb;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER,*VBO_Normals);
    glBufferData(GL_ARRAY_BUFFER,*so,NULL,*Usage);  
    tptr = (uint8_t*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    for (InstanceStructdef * iptr = InstacesBuffer_First;iptr<= InstacesBuffer_Last ;iptr++)
    {
        sb = iptr->Model_Data_ptr->Sizeof_Vec3Buffers;
        memcpy(tptr,iptr->Model_Data_ptr->Normals,sb);
        tptr+=sb;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER,*VBO_UV);
    glBufferData(GL_ARRAY_BUFFER,*si,NULL,*Usage);  
    tptr = (uint8_t*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    for (InstanceStructdef * iptr = InstacesBuffer_First;iptr<= InstacesBuffer_Last ;iptr++)
    {
        sb = iptr->Model_Data_ptr->Sizeof_Vec2Buffers;
        memcpy(tptr,iptr->Model_Data_ptr->UV,sb);
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
        for (InstanceStructdef * iptr = InstacesBuffer_First;iptr<= InstacesBuffer_Last ;iptr++)
        {
            sb = iptr->Model_Data_ptr->IndicesCount;
	    //printf("IC: %d @ %lx \n",sb, iptr->Model_Data_ptr);
            for(int iii=0;iii<sb;iii++)
            {
                indptr[IndexIndex] = iptr->Model_Data_ptr->Indices[iii] + IndexAcc ;
                IndexIndex++;
		//printf("->%d\n",indptr[IndexIndex]);
            }
            IndexAcc+=iptr->Model_Data_ptr->VertexCount;
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

void LD_3D_Ambient_Light_Set(float * Vec3_Ambient_Color)
{
    memcpy(LD_3D.Light_System.Ambient_Color,Vec3_Ambient_Color,12);
}

int LD_3D_Directional_LightsBuffer_Allocate(int Capacity)
{
    DirLightStructdef * nptr;

    if(LD_3D.Light_System.Directional_Lights_System.LightsBuffer==NULL)
    {
        nptr = 
        (DirLightStructdef*) malloc(sizeof(DirLightStructdef) * Capacity);
    }
    else
    {
        nptr= (DirLightStructdef*) 
        realloc(LD_3D.Light_System.Directional_Lights_System.LightsBuffer,
        sizeof(DirLightStructdef) * Capacity);
    }

    if(nptr == NULL){return -1;}

    LD_3D.Light_System.Directional_Lights_System.LightsBuffer = nptr;
    LD_3D.Light_System.Directional_Lights_System.Lights_Count= Capacity;
    return Capacity;
}

int LD_3D_Directional_LightsBuffer_Delete()
{
    DirLightStructdef* Exp = LD_3D.Light_System.Directional_Lights_System.LightsBuffer;
   // DirLightStructdef* End = Exp + LD_3D.Light_System.Directional_Lights_System.Lights_Count;
    LD_3D.Light_System.Directional_Lights_System.Lights_Count =0;

    free(Exp);
    LD_3D.Light_System.Directional_Lights_System.LightsBuffer = NULL;
}

int LD_3D_Directional_Light_Set (DirLightStructdef * Directional_Light, float * Vec3_Vector, float Intensity, float * Vec3_Color)
{
    memcpy(Directional_Light->Vector,Vec3_Vector,12);
    Directional_Light->Intensity = Intensity;
    float Cero [3]={0,0,0};
    if(Vec3_Color != NULL){memcpy(Directional_Light->Color,Vec3_Color,12);}
    else{memcpy(Directional_Light->Color,Cero,12);}
}

void LD_3D_Draw_G_Pass()
{
        //*****Models are about to be drawn*****//
	//*****In fact, are the buffers the ones about to be drawed*****//
    VRAMBufferStructdef * Vex =LD_3D.VRAMBuffer;
    VRAMBufferStructdef * Vend = Vex + LD_3D.Sizeof_VRAMBuffer;

    for(;Vex<Vend;Vex++)
    {    
        if((Vex->FLAG & 0x80) == 0) //Si el búfer es dibujable
        {            
            glBindVertexArray(Vex->VAO_PositionNormal);
            glDrawElements(GL_TRIANGLES,Vex->IndicesCount,GL_UNSIGNED_INT,0);
        }
    }
}

void LD_3D_Draw_L_Pass_Ambient()
{
    float * Ambient = LD_3D.Light_System.Ambient_Color;
    //Dibujar primero la luz ambiental//
    glClearColor(Ambient[0],Ambient[1],Ambient[2],1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LD_3D_Draw_L_Pass_Directional(GLint Vec4_Vector_Intensity_Location, GLint Vec3_Color_Location, GLuint Square_VAO)
{
        //******* Directional lights are about to be drawn, one pass per light, its drawed on a texture*******//
    DirLightStructdef * Vex =LD_3D.Light_System.Directional_Lights_System.LightsBuffer;
    DirLightStructdef * Vend = Vex + LD_3D.Light_System.Directional_Lights_System.Lights_Count;

    glBindVertexArray(Square_VAO);
    for(;Vex<Vend;Vex++)
    {
        glUniform4fv(Vec4_Vector_Intensity_Location,1,(GLfloat* )Vex);
        glUniform3fv(Vec3_Color_Location,1,Vex->Color);

        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
    }
}



    /** Mathematical operations **/
    /*By default it uses the libLDCC Extern/LD_Math.h*/
#include "../../Extern/LD_Math.h"
#define __M4x4V4_PseudoV3_W1_MUL_ M4x4V4_PseudoV3_W1_MUL
#define __M4x4_PseudoM3x3_V4_PseudoV3_W1_MUL_ M4x4_PseudoM3x3_V4_PseudoV3_W1_MUL

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

#define QuatInterpolation V4Lerp

void __LD_3D_Update_Get_Frames(struct LD_Instance_Anim_Descriptor_Struct * Animation,
struct __Joint_Struct ** AJoints,struct __Joint_Struct ** NJoints)
{
    LD_Animation_Object_Struct * Data = Animation->Data;

    char IS_NOT_REVERSED = ((Animation->FLAG & 4) != 4);
    char IS_LOOPING = ((Animation->FLAG & 2) == 2);
    #define IS_REVERSED !IS_NOT_REVERSED
    #define IS_NOT_LOOPING !IS_LOOPING

    unsigned int MaxFrame = Data->Animations[Animation->CurrentAnimation].num_frames-1;
    unsigned int FirstFrame= MaxFrame * IS_REVERSED;
    unsigned int LastFrame = MaxFrame * IS_NOT_REVERSED;
    unsigned int CurrFrame = (int) Animation->CurrentFrame;
    #define IS_IN_BOUNDS (CurrFrame>=0 && CurrFrame<=MaxFrame)
    CurrFrame=(CurrFrame*IS_IN_BOUNDS)+(!IS_IN_BOUNDS*LastFrame );
    #define IS_LAST_FRAME (CurrFrame == LastFrame)
    #define IS_NOT_LAST_FRAME (CurrFrame != LastFrame)
    unsigned int NextFrame = ((CurrFrame
            +((CurrFrame<LastFrame)*IS_NOT_REVERSED)
            -((CurrFrame>LastFrame)*(!IS_NOT_REVERSED))) 
            *IS_NOT_LAST_FRAME)
            +
            (IS_LAST_FRAME*(
            (IS_NOT_LOOPING*LastFrame)));

    float FrameRate = Data->Animations[Animation->CurrentAnimation].framerate;
    
    //printf("(%x)=(%x)(%x) %d -> %d\n",Animation->FLAG,IS_REVERSED,IS_LOOPING,CurrFrame,NextFrame);

    //Animation->CurrentFrame+=(FrameRate/BASE_FPS * Delta)*Animation->Speed;

    *AJoints = Data->Poses+(Data->PosesCount*CurrFrame);
    *NJoints = Data->Poses+(Data->PosesCount*NextFrame);

    #undef IS_IN_BOUNDS
    #undef IS_LAST_FRAME
    #undef IS_NOT_LAST_FRAME
    #undef IS_REVERSED
    #undef IS_NOT_LOOPING
}

void __LD_3D_Update_Calculate_Pose(struct __Vec3_Struct * Original_Vector,
unsigned char * BWeight,unsigned char * BIndex,
struct __Joint_Struct * MJoints, 
struct __Joint_Struct * AJoints,
struct __Joint_Struct * NJoints,
void * Result_Vector, float Frame, char INTERPOLATION)
{ 
    float CurW = 0;
    //printf("V: (%f,%f,%f)\n",Vrexp->x,Vrexp->y,Vrexp->z);
    for(char _w_ = 0;_w_<4&&CurW<1.f ;_w_++)
    {
        float WW = BWeight[_w_] / 255.f;CurW+=WW;
        unsigned char BB = BIndex[_w_];
        struct __Joint_Struct * AJoint = AJoints+BB; //<- Current Pose
        struct __Joint_Struct * NJoint = NJoints+BB; //<- Next Pose
        struct __Joint_Struct * MJoint = MJoints+BB; //<- Base Pose

        float _temp_temp[3]; //<-Temporal transform for this cycle.
        
        float QNew[4]; memcpy(QNew,AJoint->rotate,sizeof(float)*4);          
        float TNew[3]; memcpy(TNew,AJoint->translate,sizeof(float)*3); 
        
        if(INTERPOLATION)
        {
            float Currentframe = (float) ((int) Frame);
            float factor = ( Frame -Currentframe );
            V3Lerp(TNew,NJoint->translate,factor,TNew);
            QuatInterpolation(QNew,NJoint->rotate,factor,QNew);
        }

        float QDiff[4]; Quaternion_Diff_Unit(MJoint->rotate,QNew,QDiff);
        V3V3SUB((float*)Original_Vector,(float*)MJoint->translate,_temp_temp); 
        QuaternionRotateV3(QDiff,_temp_temp,_temp_temp);
        V3V3ADD(_temp_temp,TNew,_temp_temp);
        V3ScalarMUL(_temp_temp,WW,_temp_temp);
        V3V3ADD(Result_Vector,_temp_temp,Result_Vector);
    }
}

void __LD_3D_Update_Calculate_Pose_Normal(struct __Vec3_Struct * Original_Vector,
unsigned char * BWeight,unsigned char * BIndex,
struct __Joint_Struct * MJoints, 
struct __Joint_Struct * AJoints,
struct __Joint_Struct * NJoints,
void * Result_Vector, float Frame, char INTERPOLATION)
{ 
    float CurW = 0;
    //printf("V: (%f,%f,%f)\n",Vrexp->x,Vrexp->y,Vrexp->z);
    for(char _w_ = 0;_w_<4&&CurW<1.f ;_w_++)
    {
        float WW = BWeight[_w_] / 255.f;CurW+=WW;
        unsigned char BB = BIndex[_w_];
        struct __Joint_Struct * AJoint = AJoints+BB; //<- Current Pose
        struct __Joint_Struct * NJoint = NJoints+BB; //<- Next Pose
        struct __Joint_Struct * MJoint = MJoints+BB; //<- Base Pose

        float _temp_temp[3]; //<-Temporal transform for this cycle.
        
        float QNew[4]; memcpy(QNew,AJoint->rotate,sizeof(float)*4);          
        
        if(INTERPOLATION)
        {
            float Currentframe = (float) ((int) Frame);
            float factor = ( Frame -Currentframe );
            QuatInterpolation(QNew,NJoint->rotate,factor,QNew);
        }
        float QDiff[4]; Quaternion_Diff_Unit(MJoint->rotate,QNew,QDiff);
        QuaternionRotateV3(QDiff,Original_Vector,_temp_temp);
        V3V3ADD((float*)Result_Vector,(float*)_temp_temp,(float*)Result_Vector);
    }
}

#undef QuatInterpolation

void LD_3D_Update(float Delta, float BASE_FPS)
{
    VRAMBufferStructdef * Vex =LD_3D.VRAMBuffer;
    VRAMBufferStructdef * Vend = Vex + LD_3D.Sizeof_VRAMBuffer;

    InstanceStructdef * Iexp;
    InstanceStructdef * Iend;
    struct __Vec3_Struct * Vrexp;
    struct __Vec3_Struct * Vrend;

    for(;Vex<Vend;Vex++)
    {
        if((Vex->FLAG & 1) == 0 & (Vex->FLAG & 0x80) == 0) //If the buffer is non-static and drawable
        {      
		    glBindBuffer(GL_ARRAY_BUFFER,Vex->VBO_Position);
            glBufferData(GL_ARRAY_BUFFER,Vex->Sizeof_Vec3Buffers,NULL,Vex->Usage);
            float * ptr =(float*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
            Iend =  Vex->Last_instance; //Check this line for a better solution
            for(Iexp = Vex->First_instance; Iexp <= Iend; Iexp++)
            {
                struct __Joint_Struct * MJoints= Iexp->Model_Data_ptr->Joints;          
                struct __Joint_Struct * AJoints;
                struct __Joint_Struct * NJoints;

                if(Iexp->Animation.Data==NULL){AJoints = MJoints;NJoints=MJoints;}
                else
                {__LD_3D_Update_Get_Frames(&Iexp->Animation,&AJoints,&NJoints);}

		        //Dynamic Positions update
                Vrexp = Iexp->Model_Data_ptr->Vertices;
                Vrend = Vrexp + Iexp->Model_Data_ptr->VertexCount;
                unsigned char * BIndex = Iexp->Model_Data_ptr->BlendingIndices;
                unsigned char * BWeight = Iexp->Model_Data_ptr->BlendingWeights;

                char INTERPOLATION = (Iexp->Animation.FLAG & 8) == 8;
 
                for(;Vrexp<Vrend;Vrexp++) 
                {
                    // Animation
                    float _temp[3]={0,0,0};

                    __LD_3D_Update_Calculate_Pose(Vrexp,BWeight,
                    BIndex,MJoints,AJoints,NJoints,
                    _temp,Iexp->Animation.CurrentFrame,INTERPOLATION);
                    BIndex+=4;
                    BWeight+=4;

                    //Apply entire transformation matrix
                    __M4x4V4_PseudoV3_W1_MUL_(Iexp->Matrix,_temp,ptr);
                    ptr+=3;
                   
                }
            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
            

            glBindBuffer(GL_ARRAY_BUFFER,Vex->VBO_Normals);
                glBufferData(GL_ARRAY_BUFFER,Vex->Sizeof_Vec3Buffers,NULL,Vex->Usage);
                 ptr =(float*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
                Iend =  Vex->Last_instance; //Check this line for a better solution
                for(Iexp = Vex->First_instance; Iexp <= Iend; Iexp++)
                {
                    struct __Joint_Struct * MJoints= Iexp->Model_Data_ptr->Joints;          
                    struct __Joint_Struct * AJoints;
                    struct __Joint_Struct * NJoints;

                    if(Iexp->Animation.Data==NULL){AJoints = MJoints;NJoints=MJoints;}
                    else
                    {__LD_3D_Update_Get_Frames(&Iexp->Animation,&AJoints,&NJoints);}
                    char INTERPOLATION = (Iexp->Animation.FLAG & 8) == 8;                       


                    /** Dynamic Normals update**/
                    Vrexp = Iexp->Model_Data_ptr->Normals;
                    Vrend = Vrexp + Iexp->Model_Data_ptr->VertexCount;
                    unsigned char * BIndex = Iexp->Model_Data_ptr->BlendingIndices;
                    unsigned char * BWeight = Iexp->Model_Data_ptr->BlendingWeights; 

                    for(;Vrexp<Vrend;Vrexp++) 
                    {
                        /** Animation **/
                        float _temp[3]={0,0,0};

                        __LD_3D_Update_Calculate_Pose_Normal(Vrexp,
                        BWeight,BIndex,
                        MJoints,AJoints,NJoints,_temp,
                        Iexp->Animation.CurrentFrame,INTERPOLATION);
                        BIndex+=4;
                        BWeight+=4; 

                        /*Apply only affine transformation*/
                        __M4x4_PseudoM3x3_V4_PseudoV3_W1_MUL_(Iexp->Matrix,_temp,ptr);
                        ptr+=3;                          
                    }               

                    /***************************************/
                        /** Update Frame counter**/
                        
                 Iexp->Animation.CurrentFrame+=
                    (Iexp->Animation.Data->Animations[Iexp->Animation.CurrentAnimation].framerate
                    /BASEFPS * Delta) 
                    *Iexp->Animation.Speed * (1.f+(-2.f * ((Iexp->Animation.FLAG & 4) == 4)));
        if(((Iexp->Animation.FLAG & 2) == 2) && Iexp->Animation.CurrentFrame>(float) Iexp->Animation.Data->Animations[Iexp->Animation.CurrentAnimation].num_frames)
        {Iexp->Animation.CurrentFrame=0;}
                    
                    /***************************************/
                }

                glUnmapBuffer(GL_ARRAY_BUFFER);
            glBindBuffer(GL_ARRAY_BUFFER,0);
        }
    } 
}
    /****************************/

#endif
