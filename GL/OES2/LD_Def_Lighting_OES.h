/***************************************************************************
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

/***************************************************************************
    This library contains an implementation of Deferred Lighting, 
Indexed rendering and pseudo-instanced rendering.

    These functions were originally made for a system were it was more convenient
to process data in CPU to then send it to the GPU in order to avoid complexity in 
uniforms and shader programs.
    Vertex transformations in World Space should be made them in CPU. 
Transformations to Camera and Screen Space should be made in Vertex Shader.
    Also, in order to reduce the number of draw calls, a instacing-like method is 
provided, so as long as the same texture is shared, multiple models (unique or copies)
can be draw in one draw call. To achieve this, the models are all packed in the same
buffers. The maximum size of the these buffers are sistem dependant.
    Due to the lack of floating point textures, data must be normalized and "MAD'd" 
prior to it's  "drawing" in the textures, so negative values are preserved. 
Some precision is lost in the progress.
    The proccess consists on transforming data from the [-1,+1] space and the [0,+1] 
space to then transform it back from the [0,+1] space to the [-1,+1] space.
****************************************************************************/

/*October 1th, 2018: Functional and stable, but not finished yet*/

#include <strings.h>

#ifndef _LD_DEF_LIGHTING_OES_H
#define _LD_DEF_LIGHTING_OES_H

    /*  The same identity matrix can be defined in many othar places
        if it's not defined, define it here */
    #ifndef __IDENTITY_MATRIX_4X4_FLOAT_
    #define __IDENTITY_MATRIX_4X4_FLOAT_
        float Identity_Matrix_4x4[16]=
        {
            1.f,0.f,0.f,0.f,
            0.f,1.f,0.f,0.f,
            0.f,0.f,1.f,0.f,
            0.f,0.f,0.f,1.f
        };
    #endif  

//It's similar to IQM_RAW_Struct, but it doesn't have the first two integers
typedef struct LD_Model_Object_Struct 
{
    int VertexCount;
	int TrianglesCount;
	int Sizeof_Vec3Buffers;
    int Sizeof_Vec2Buffers;
    int IndicesCount;
    int JointsCount;

    struct __Vec3_Struct
    {
        float x,y,z;
    } * Vertices;

    struct __Vec3_Struct * Normals;

    struct __UV_Struct
    {
        float u,v;
    } * UV;

    int * Indices;

    struct __Joint_Struct
    {
        unsigned int name;
        int parent;
        float translate [3];
        float rotate [4];
        float scale [3];
    } * Joints;

    struct __Blend_Struct
    {
        unsigned char Indices[4];
        unsigned char Weights[4];
    }  * Blendings;
    
    struct __Pose_Struct
    {
        int parent;
        float translate [3];
        float rotate [4];
        float scale [3];
    } * Poses;
}LD_Model_Object_Struct;

void LD_Model_Object_Clear(LD_Model_Object_Struct * LD_Model)
{
    if(LD_Model->Vertices!=NULL) {free(LD_Model->Vertices);}
    if(LD_Model->Normals!=NULL)  {free(LD_Model->Normals);}
    if(LD_Model->UV!=NULL)       {free(LD_Model->UV);}
    if(LD_Model->Indices!=NULL)  {free(LD_Model->Indices);}
    if(LD_Model->Joints!=NULL)   {free(LD_Model->Joints);}
    if(LD_Model->Blendings!=NULL){free(LD_Model->Blendings);}
    if(LD_Model->Poses!=NULL)    {free(LD_Model->Poses);}
}

struct LD_3D_Struct
{
    int Models_Count;
    struct LD_ModelsLinkedList_Struct
    {
        LD_Model_Object_Struct Model;
        struct LD_ModelsLinkedList_Struct * Next;
    } * ModelsLinkedList_First;
    struct LD_ModelsLinkedList_Struct * ModelsLinkedList_Last;

    int Sizeof_InstacesBuffer;
    struct LD_Instance_Struct
    {
        float Matrix[16];
        LD_Model_Object_Struct * Model_Data_ptr;
    } * InstacesBuffer;

    int Sizeof_VRAMBuffer;
    struct LD_VRAMBuffer_struct
    {
        char FLAG; // el primer bite indica estaticidad, el último indica exclusión para dibujar
        struct LD_Instance_Struct * First_instance;
        struct LD_Instance_Struct * Last_instance;
        GLenum Usage;
        int Sizeof_Vec3Buffers;
        int Sizeof_Vec2Buffers;
        int IndicesCount;
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
            int Lights_Count;
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
    LD_3D.Sizeof_InstacesBuffer=0;
    LD_3D.Sizeof_VRAMBuffer=0;
    memcpy(LD_3D.Light_System.Ambient_Color,Cero,sizeof(float)*3);
    LD_3D.Light_System.Directional_Lights_System.Lights_Count = 0;

    LD_3D.ModelsLinkedList_First=NULL;
    LD_3D.ModelsLinkedList_Last=LD_3D.ModelsLinkedList_First;
    LD_3D.InstacesBuffer=NULL;
    LD_3D.VRAMBuffer=NULL;
    LD_3D.Light_System.Directional_Lights_System.LightsBuffer = NULL;
}


LD_Model_Object_Struct * LD_3D_ModelsLinkedList_Append(void * Data_pointer)
{
    if(LD_3D.ModelsLinkedList_First==NULL)
    {
        LD_3D.ModelsLinkedList_First= malloc(sizeof(struct LD_ModelsLinkedList_Struct));
        LD_3D.ModelsLinkedList_Last = LD_3D.ModelsLinkedList_First;
    }
    else
    {
        LD_3D.ModelsLinkedList_Last->Next = malloc(sizeof(struct LD_ModelsLinkedList_Struct));
        LD_3D.ModelsLinkedList_Last = LD_3D.ModelsLinkedList_Last->Next;
    }
    LD_3D.ModelsLinkedList_Last->Next=NULL;

    LD_Model_Object_Struct * Return = &LD_3D.ModelsLinkedList_Last->Model;

    memcpy(Return,Data_pointer,sizeof(LD_Model_Object_Struct));

    LD_3D.Models_Count++;

    return Return;
}

void LD_3D_ModelsLinkedList_System_Delete()
{
    LD_3D.Models_Count=0;

    struct LD_ModelsLinkedList_Struct * Exp = LD_3D.ModelsLinkedList_First;
    struct LD_ModelsLinkedList_Struct * tmp;


    while(Exp != NULL)
    {
        tmp = Exp->Next;
        LD_Model_Object_Clear(&Exp->Model);
        free(Exp);
        Exp= tmp;
    }
}

int LD_3D_InstaceBuffer_Set_capacity(int New_Instance_Capacity)
{
    
    if(LD_3D.InstacesBuffer == NULL)
    {
        LD_3D.InstacesBuffer =(InstanceStructdef*)  
        malloc(sizeof(InstanceStructdef)*New_Instance_Capacity);
    }
    else
    {
        LD_3D.InstacesBuffer = (InstanceStructdef*) 
        realloc(LD_3D.InstacesBuffer,sizeof(InstanceStructdef) * New_Instance_Capacity);
    }

    if(LD_3D.InstacesBuffer == NULL){return -1;}
    LD_3D.Sizeof_InstacesBuffer=New_Instance_Capacity;

    return New_Instance_Capacity;
}

void LD_3D_Fill_Instance(InstanceStructdef * Instance_Ptr, LD_Model_Object_Struct * Model_Ptr)
{
    memcpy(Instance_Ptr->Matrix,Identity_Matrix_4x4,4*16);
    Instance_Ptr->Model_Data_ptr = Model_Ptr;
}

void LD_3D_Instance_Replicate(InstanceStructdef * From, InstanceStructdef * To, InstanceStructdef * Master_Copy)
{
    for(;From<=To;From++)
    {
        memcpy(From,Master_Copy,sizeof(InstanceStructdef));
    }
}


int LD_3D_VRAMBuffer_allocate(int New_Instance_Capacity)
{
    if(LD_3D.VRAMBuffer == NULL)
    {
        LD_3D.VRAMBuffer = (VRAMBufferStructdef *)
        malloc(sizeof(VRAMBufferStructdef) * New_Instance_Capacity);
    }
    else
    { 
        LD_3D.VRAMBuffer = (VRAMBufferStructdef *) 
        realloc(LD_3D.VRAMBuffer,sizeof(VRAMBufferStructdef) * New_Instance_Capacity);
    }

    if(LD_3D.VRAMBuffer == NULL){return -1;}
    LD_3D.VRAMBuffer[New_Instance_Capacity-1].FLAG=128;
    LD_3D.Sizeof_VRAMBuffer=New_Instance_Capacity;
    return LD_3D.Sizeof_VRAMBuffer;
}

void LD_3D_VRAMBuffer_Clear(VRAMBufferStructdef* VRAMBuffer,char glDelete)
{
    VRAMBuffer->Sizeof_Vec3Buffers=0;
    VRAMBuffer->Sizeof_Vec2Buffers=0;
    VRAMBuffer->IndicesCount=0;
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
   // printf("%x | %d\n ",tptr, *so);
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

    

    glBindVertexArray(*VAO_PositionNormal);



        glBindBuffer(GL_ARRAY_BUFFER,*VBO_Position);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4*3, (GLvoid*)(0));
        glEnableVertexAttribArray(0);


        glBindBuffer(GL_ARRAY_BUFFER,*VBO_Normals);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4*3, (GLvoid*)(0));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,*EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,*vc * 4,NULL,*Usage);  
        tptr = (uint8_t*) glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_WRITE_ONLY);
        //printf("%x | %d\n ",tptr, *vc);
        for (InstanceStructdef * iptr = InstacesBuffer_First;iptr<= InstacesBuffer_Last ;iptr++)
        {
            sb = iptr->Model_Data_ptr->IndicesCount;

            memcpy(tptr,iptr->Model_Data_ptr->Indices,sb * 4);
            tptr+=sb;
        }

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
    /*By default it uses the libLDCC LD_Math.h*/
#include "../../LD_Math.h"
#define __M4x4V4_PseudoV3_W1_MUL_ M4x4V4_PseudoV3_W1_MUL



void LD_3D_Update()
{
    VRAMBufferStructdef * Vex =LD_3D.VRAMBuffer;
    VRAMBufferStructdef * Vend = Vex + LD_3D.Sizeof_VRAMBuffer;

    InstanceStructdef * Iexp;
    InstanceStructdef * Iend;
    struct __Vec3_Struct * Vrexp;
    struct __Vec3_Struct * Vrend;

    for(;Vex<Vend;Vex++)
    {
        //printf("SÍ\n");
        if((Vex->FLAG & 1) == 0 & (Vex->FLAG & 0x80) == 0) //Si el búfer no es estático y es dibujable
        {
            
            glBindBuffer(GL_ARRAY_BUFFER,Vex->VBO_Position);
                glBufferData(GL_ARRAY_BUFFER,Vex->Sizeof_Vec3Buffers,NULL,Vex->Usage);
                float * ptr =(float*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
                //printf("%x\n", ptr);
                Iend =  Vex->Last_instance; //Revisar linea para rendimiento
                for(Iexp = Vex->First_instance; Iexp <= Iend; Iexp++)
                {
                    /** Actualización del modelo dinámico **/
                    Vrexp = Iexp->Model_Data_ptr->Vertices;
                    Vrend = Vrexp + Iexp->Model_Data_ptr->VertexCount;

                    //fastmemcpy(ptr,Vrexp,Vex->Sizeof_VBO);
                    
                    for(;Vrexp<Vrend;Vrexp++) 
                    {
                        __M4x4V4_PseudoV3_W1_MUL_(Iexp->Matrix,(float*)Vrexp,ptr);
                        ptr+=3;
                        //fastmemcpy(ptr,Vrexp->Normal_and_V,16);
                    }
                    /***************************************/
                }

                glUnmapBuffer(GL_ARRAY_BUFFER);
            glBindBuffer(GL_ARRAY_BUFFER,0);
        }
    } 
}



    /****************************/

#endif