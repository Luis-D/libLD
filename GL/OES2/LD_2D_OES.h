#ifndef LD_2D_OES_H
#define LD_2D_OES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../libLDExtra/LD_Stack_Queue.h"
#include "../../libLDExtra/LD_Tricks.h"
#include "../../LDM/LDM.h"

#include "../GL_UTILS.h"

typedef struct LD_Sprite_Object_Struct 
{
    struct LD_Sprite_Object_UV_Struct
    {
        struct {float x, y;} Origin;
        float Width, Height;
    } UV;
}LD_Sprite_Object_Struct;

struct LD_2D_Struct
{
    unsigned int Sprites_Count;
    DoublyLinkedList(LD_Sprite_Object_Struct) * Sprites_LL;

    unsigned int Sizeof_InstancesBuffer;
    struct LD_2D_Instance_Struct
    {
        struct LD_2D_Instance_Sprite
        {
            float Matrix[4]; //<- 2D Affine transformation
            struct {float x, y;} Translation;
            float Depth,Extra;
            struct {float r,g,b,a;} Color;
            struct LD_Sprite_Object_Struct * Data;
            //** All this data fits in a MAT4 despites it's in fact 4 MAT2 **//
        }Sprite;

    } * InstancesBuffer;

    unsigned int Sizeof_VRAMBuffer;
    struct LD_2D_VRAMBuffer_struct
    {
        char FLAG; //LSB is for Staticity, MSB is for draw Exclusion
        struct LD_2D_Instance_Struct * First_instance;
        struct LD_2D_Instance_Struct * Last_instance;
        GLenum Usage;

        unsigned int SpritesCount;
        unsigned int TexturesCount;
        GLuint VBO_PositionUV;
        GLuint VBO_DepthExtra;
        GLuint VBO_RGBA;
        GLuint VAO;
        GLuint EBO;
        GLuint * Texture;
        struct LD_2D_Configuration_Struct
        {
            char Culling;
        } Configuration;
    } * VRAMBuffer;
}LD_2D;

void LD_2D_Init()
{
    _zero(*(int*)&LD_2D.Sprites_Count);
    _zero(*(int*)&LD_2D.Sizeof_InstancesBuffer);
    _zero(*(int*)&LD_2D.Sizeof_VRAMBuffer);

    LD_2D.Sprites_LL = SinglyLinkedList_Create();
    _NULL(*(int*)&LD_2D.InstancesBuffer);
    _NULL(*(int*)&LD_2D.VRAMBuffer);
}

struct LD_Sprite_Object_Struct * LD_2D_SpriteLinkedList_Append
                        (float X,float Y,float W,float H)
{
    struct LD_Sprite_Object_Struct tmp;
    tmp.UV.Origin.x = X;
    tmp.UV.Origin.y = Y;
    tmp.UV.Width = W;
    tmp.UV.Height = H;
   
    void * Return=DoublyLinkedList_add(LD_2D.Sprites_LL,tmp);

    LD_2D.Sprites_Count+=(Return != NULL);

    return (LD_Sprite_Object_Struct *)Return;
}

void LD_2D_SpriteLinkedList_Remove(struct LD_Sprite_Object_Struct * Sprite)
{
    DoublyLinkedList_remove(LD_2D.Sprites_LL, Sprite, NULL);
    LD_2D.Sprites_Count-=(LD_2D.Sprites_Count>0);
}

void LD_2D_SpriteLinkedList_System_Delete()
{
    DoublyLinkedList_clear(LD_2D.Sprites_LL,NULL);
    _zero(LD_2D.Sprites_Count);
}


unsigned int LD_2D_InstanceBuffer_Set_Capacity(unsigned int New_Instance_Capacity)
{
    LD_2D.InstancesBuffer = _resize(struct LD_2D_Instance_Struct,LD_2D.InstancesBuffer,New_Instance_Capacity);
    if(LD_2D.InstancesBuffer == NULL){return 0;}

    LD_2D.Sizeof_InstancesBuffer=New_Instance_Capacity;


    return New_Instance_Capacity;
}


void LD_2D_Fill_Instance(struct LD_2D_Instance_Struct * Instance_Ptr, 
                        struct LD_Sprite_Object_Struct * Sprite_Ptr)
{
    Instance_Ptr->Sprite.Data = Sprite_Ptr;

   
    //Clear from Translation to Extra
    memset(&Instance_Ptr->Sprite.Translation,0,sizeof(float)*4); 
   // printf("lulululilililili\n");
    float M2I[] = {1,0,0,1};
    memcpy(Instance_Ptr->Sprite.Matrix,M2I,sizeof(float)*4);
   //M2IDENTITY(Instance_Ptr->Sprite.Matrix);

 //printf("lililililililili\n");
    float nc[4]={1,1,1,1};
    memcpy(&Instance_Ptr->Sprite.Color,nc,sizeof(float)*4);

 //printf("lolololololololo\n");

}

void LD_2D_Instance_Identity(struct LD_2D_Instance_Struct * Instance_Ptr)
{
    M2IDENTITY(Instance_Ptr->Sprite.Matrix);
    memset(&Instance_Ptr->Sprite.Translation,0,sizeof(float)*2);
}

void LD_2D_Instance_Color(struct LD_2D_Instance_Struct * Instance_Ptr, void * RGBA)
{memcpy(&Instance_Ptr->Sprite.Color,RGBA,sizeof(float)*4);}

void LD_2D_Instance_Depth(struct LD_2D_Instance_Struct * Instance_Ptr, float Depth)
{Instance_Ptr->Sprite.Depth = Depth;}

void LD_2D_Instance_Scale(struct LD_2D_Instance_Struct * Instance_Ptr,float * ScaleVector)
{
    Instance_Ptr->Sprite.Matrix[0]*=ScaleVector[0];
    Instance_Ptr->Sprite.Matrix[3]*=ScaleVector[1];
}

void LD_2D_Instance_Translate(struct LD_2D_Instance_Struct * Instance_Ptr,float * Vector)
{memcpy(&Instance_Ptr->Sprite.Translation,Vector,sizeof(float)*2);}

void LD_2D_Instance_Translate_add(struct LD_2D_Instance_Struct * Instance_Ptr,float * Vector)
{
    float * ptr = (float*) &Instance_Ptr->Sprite.Translation;
    V2ADD(ptr,ptr,Vector);
}


#include <math.h>
#include "../../libLDExtra/LD_Tricks.h"
void LD_2D_Instance_Rotate(struct LD_2D_Instance_Struct * Instance_Ptr,float Radians)
{
    float Matrix[4] = {cosf(Radians),sinf(Radians),-sinf(Radians),cosf(Radians)};
   // printV4(((float*)Instance_Ptr->Sprite.Matrix)); printf(" * ");
   // printV4(Matrix); printf("\n");
    M2MUL(Instance_Ptr->Sprite.Matrix,Instance_Ptr->Sprite.Matrix,Matrix);
}


void LD_2D_Instance_System_Delete()
{
    int * i = &LD_2D.Sizeof_InstancesBuffer;
    for(;i--;)
    {
        free(LD_2D.InstancesBuffer+*i);
    }

    free(LD_2D.InstancesBuffer);
    _NULL(*(int*)&LD_2D.InstancesBuffer);
}


unsigned int LD_2D_VRAMBuffer_allocate(unsigned int New_number_of_buffers)
{
    LD_2D.VRAMBuffer =_resize(struct LD_2D_VRAMBuffer_struct,LD_2D.VRAMBuffer,New_number_of_buffers);
    if(LD_2D.VRAMBuffer == NULL){return 0;}

    array_foreach(struct LD_2D_VRAMBuffer_struct,ex,LD_2D.VRAMBuffer,New_number_of_buffers)
    {
	    ex->TexturesCount=0;
        {ex->FLAG=128;}
        ex->Configuration.Culling=1;
    }

    LD_2D.Sizeof_VRAMBuffer=New_number_of_buffers;
    return New_number_of_buffers;
}

unsigned int LD_2D_VRAMBuffer_Set_Texture_Capacity(struct LD_2D_VRAMBuffer_struct * VRAMBuffer, unsigned int Cap)
{
    GLuint * tmp = (GLuint*)malloc(sizeof(GLuint)*Cap);
    if(tmp==NULL){return 0;}
    VRAMBuffer->Texture = tmp;
    VRAMBuffer->TexturesCount=Cap;
    return Cap;
}

void LD_2D_VRAMBuffer_Set_Texture(struct LD_2D_VRAMBuffer_struct* VRAMBuffer, 
                                unsigned int Index,GLuint  Texture)
{
    VRAMBuffer->Texture[Index] = Texture;
}

void LD_2D_VRAMBuffer_Clear(struct LD_2D_VRAMBuffer_struct* VRAMBuffer)
{
    if(VRAMBuffer->TexturesCount > 0)
    {free(VRAMBuffer->Texture);}
    VRAMBuffer->FLAG |= 0x80;
    
    if(VRAMBuffer->SpritesCount > 0)
    {
        glDeleteBuffers(1,&VRAMBuffer->VBO_RGBA);
        glDeleteBuffers(1,&VRAMBuffer->VBO_DepthExtra);
        glDeleteBuffers(1,&VRAMBuffer->VBO_PositionUV);
        glDeleteVertexArrays(1,&VRAMBuffer->VAO);
    }
    
    _zero(VRAMBuffer->SpritesCount);
    _zero(VRAMBuffer->TexturesCount);
}


void LD_2D_VRAMBuffer_System_Clear()
{
    int  i = LD_2D.Sizeof_VRAMBuffer;
    while( i --> 0)
    {
        LD_2D_VRAMBuffer_Clear(LD_2D.VRAMBuffer+i);    
    }
}

void LD_2D_VRAMBuffer_System_Delete()
{
    LD_2D_VRAMBuffer_System_Clear();

    free(LD_2D.VRAMBuffer);
    _NULL(*(int*)&LD_2D.VRAMBuffer);
}



void LD_2D_Fill_VRAMBuffer(struct LD_2D_VRAMBuffer_struct* VRAMPtr, 
struct LD_2D_Instance_Struct * InstancesBuffer_First, 
struct LD_2D_Instance_Struct * InstancesBuffer_Last,char FLAG)
{
    GLfloat PT2 []=
    {-1,-1,0,1,  
     1,-1, 1,1,
     -1,1, 0,0,    
     1,1 ,  1,0};
     unsigned int Indices []={0,2,1,1,2,3};


    VRAMPtr->FLAG=FLAG;
    VRAMPtr->First_instance = InstancesBuffer_First;
    VRAMPtr->Last_instance = InstancesBuffer_Last;
 
    int * sc = &VRAMPtr->SpritesCount; _zero(*sc);

    for (struct LD_2D_Instance_Struct * ex = InstancesBuffer_First;ex<=InstancesBuffer_Last;ex++ )
    {
        (*sc) ++;
    }

    GLuint * VBO_RGBA = &VRAMPtr->VBO_RGBA;
    GLuint * VBO_DepthExtra = &VRAMPtr->VBO_DepthExtra;
    GLuint * VBO_PositionUV = &VRAMPtr->VBO_PositionUV;
    GLuint * VAO = &VRAMPtr->VAO;
    GLuint * EBO =&VRAMPtr->EBO;

    GLenum * Usage = &VRAMPtr->Usage;

    if((FLAG & 1) == 1){*Usage=GL_STATIC_DRAW;}
    else{*Usage=GL_DYNAMIC_DRAW;;}

    glGenBuffers(1,VBO_DepthExtra);
    glGenBuffers(1,VBO_RGBA);
    glGenBuffers(1,VBO_PositionUV);
    glGenBuffers(1,EBO);

    glBindBuffer(GL_ARRAY_BUFFER,*VBO_PositionUV);
    glBufferData(GL_ARRAY_BUFFER,(*sc)*(4*4*4),NULL,*Usage);  
    //uint8_t * tptr = (uint8_t*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    float * ftptr = (float *)  glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
   
    for (struct LD_2D_Instance_Struct * iptr = InstancesBuffer_First;iptr<= InstancesBuffer_Last ;iptr++)
    {
        memcpy(ftptr,PT2,4*4*4);
        ftptr+=4*4;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER,*VBO_RGBA);
    glBufferData(GL_ARRAY_BUFFER,(*sc)*(4*4*4),NULL,*Usage);  
    ftptr = (float*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    for (struct LD_2D_Instance_Struct * iptr = InstancesBuffer_First;iptr<= InstancesBuffer_Last ;iptr++)
    {
        for(int uwu = 0; uwu<4 ;uwu++)
        {         
            memcpy(ftptr+(uwu*4),&iptr->Sprite.Color,sizeof(float)*4);
            //printV4(((float*)ftptr+(uwu*sizeof(float)*4)));printf("\n");
        }

        ftptr+=4*4;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER,*VBO_DepthExtra);
    glBufferData(GL_ARRAY_BUFFER,(*sc)*(4*4*2),NULL,*Usage);  
    ftptr = (float*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    for (struct LD_2D_Instance_Struct * iptr = InstancesBuffer_First;iptr<= InstancesBuffer_Last ;iptr++)
    {
        for(int uwu = 0; uwu<4 ;uwu++)
        {         
            memcpy(ftptr+(uwu*2),&iptr->Sprite.Depth,sizeof(float)*2); //Depth and Extra

        }

        ftptr+=2*4;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER,0);


    glGenVertexArrays(1,VAO);

   // printf("-------\n");
    glBindVertexArray(*VAO);
        glBindBuffer(GL_ARRAY_BUFFER,*VBO_PositionUV);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*4, (GLvoid*)(0));
        glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER,*VBO_RGBA);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4*4, (GLvoid*)(0));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER,*VBO_DepthExtra);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*4, (GLvoid*)(0));
        glEnableVertexAttribArray(2);

      

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,*EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,(*sc) * (6*4),NULL,*Usage);  

        int * indptr = (int *) glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_WRITE_ONLY);
	//printf("indptr = %lx\n",indptr);
        int IndexIndex=0,IndexAcc=0;
        for (struct LD_2D_Instance_Struct * iptr = InstancesBuffer_First;iptr<= InstancesBuffer_Last ;iptr++)
        {
	    //printf("IC: %d @ %p \n",IndexAcc/6, iptr->Sprite.Data);
            for(int iii=0;iii<6;iii++)
            {
                indptr[IndexIndex] =Indices[iii] +IndexAcc;
                
                indptr++;
            }
            IndexAcc+=4;
        }

    //printf("-------\n");
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    glBindVertexArray(0);


}




void LD_2D_Update(float Delta, float BASE_FPS)
{
   GLfloat PT2 []=
    {-1,-1,0,1,  
     1,-1, 1,1,
     -1,1, 0,0,    
     1,1 ,  1,0};
    unsigned int Indices []={0,2,1,1,2,3};


    array_foreach(struct LD_2D_VRAMBuffer_struct,Vex,LD_2D.VRAMBuffer,LD_2D.Sizeof_VRAMBuffer)
    {
        if((Vex->FLAG & 1) == 0 & (Vex->FLAG & 0x80) == 0) //If the buffer is non-static and drawable
        {   
		    glBindBuffer(GL_ARRAY_BUFFER,Vex->VBO_PositionUV);
            glBufferData(GL_ARRAY_BUFFER,Vex->SpritesCount*(4*4)*4,NULL,Vex->Usage);
            float * ptr =(float*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
            struct LD_2D_Instance_Struct * Iend =  Vex->Last_instance; //Check this line for a better solution
            for(struct LD_2D_Instance_Struct * Iexp = Vex->First_instance; Iexp <= Iend; Iexp++)
            {
                struct LD_Sprite_Object_Struct * Sprite_Data = Iexp->Sprite.Data;

                //**Transform Position and UVs **// 

                float * Matrix = Iexp->Sprite.Matrix;
                float * Translation =(float*) &Iexp->Sprite.Translation;
                float * UV =(float*) &Sprite_Data->UV.Origin.x;

                struct ____TMP____ {float x,y,u,v;};
                array_foreach(struct ____TMP____,Vrexp,(struct ____TMP____*)PT2,4)
                {
                    M2MULV2(ptr,Matrix,Vrexp);
                    V2ADD(ptr,ptr,Translation);
                    V2ADD(ptr+2,UV,((float*)Vrexp)+2);
                    (ptr+2)[0] *= (UV+2)[0];
                    (ptr+2)[1] *= (UV+2)[1]; 
                    ptr+=4;      
                }
            }   
            glUnmapBuffer(GL_ARRAY_BUFFER);       

		    glBindBuffer(GL_ARRAY_BUFFER,Vex->VBO_RGBA);
            glBufferData(GL_ARRAY_BUFFER,Vex->SpritesCount*(4*4)*4,NULL,Vex->Usage);
            ptr =(float*) glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
            Iend =  Vex->Last_instance; //Check this line for a better solution
            for(struct LD_2D_Instance_Struct * Iexp = Vex->First_instance; Iexp <= Iend; Iexp++)
            {
                //printf("%p\n",Iexp);
                //**Color. RGBA **// 
                float * RGBA =(float*) &Iexp->Sprite.Color;

                struct ____TMP____ {float r,g,b,a;};
                array_foreach(struct ____TMP____,Vrexp,(struct ____TMP____*)PT2,4)
                {
                    memcpy(ptr,RGBA,sizeof(float)*4);
                    ptr+=4;      
                }
            }   
            glUnmapBuffer(GL_ARRAY_BUFFER);       


        }
    } 

    glBindBuffer(GL_ARRAY_BUFFER,0);

  
}



//************* 2D Drawing process *************//


struct LD_2D_Drawing_Normal_Struct
{
    struct LD_2D_Drawing_Normal_Buffers_Struct
    {
        GLuint ColorBuffer;

        GLuint FrameBuffer;
        GLuint DepthBuffer;
    }Buffers;
    struct LD_2D_Drawing_Normal_Configuration
    {
        struct LD_2D_Drawing_Normal_Configuration_Options
        {
            int Width; 
            int Height;

        }Options;

        struct LD_2D_Drawing_Normal_Configuration_CBuffer
        {
            GLuint Shader;
            struct LD_2D_Drawing_Normal_Uniform_Locations_CBuffer
            {
                unsigned int ProjectionViewMatrix;
                unsigned int TexturesCount;
                unsigned int * Textures;
            } Uniforms;
        }CBuffer;

    }Configuration;

}LD_2D_DL;

void LD_2D_CBuffer_Config(GLuint Shader,GLuint* Textures,unsigned int TexturesCount)
{
    LD_2D_DL.Configuration.CBuffer.Shader = Shader;
    LD_2D_DL.Configuration.CBuffer.Uniforms.ProjectionViewMatrix =
        glGetUniformLocation(Shader,"ProjectionViewMatrix");
    LD_2D_DL.Configuration.CBuffer.Uniforms.TexturesCount = TexturesCount;
    LD_2D_DL.Configuration.CBuffer.Uniforms.Textures = Textures;
}

void LD_2D_Drawing_Normal_Init(GLsizei WIDTH,GLsizei HEIGHT)
{
    LD_2D_DL.Configuration.Options.Width = WIDTH;
    LD_2D_DL.Configuration.Options.Height = HEIGHT;
    

    glGenTextures(1,&LD_2D_DL.Buffers.ColorBuffer);
    glBindTexture(GL_TEXTURE_2D,LD_2D_DL.Buffers.ColorBuffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,WIDTH,HEIGHT,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D,0);


    glGenFramebuffers(1,&LD_2D_DL.Buffers.FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,LD_2D_DL.Buffers.FrameBuffer);

    glGenTextures(1,&LD_2D_DL.Buffers.DepthBuffer);
    glBindTexture(GL_TEXTURE_2D,LD_2D_DL.Buffers.DepthBuffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,WIDTH,HEIGHT,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,LD_2D_DL.Buffers.DepthBuffer,0);
  
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}



void LD_2D_Draw_C_Pass(GLuint * Textures_Loc,unsigned int NumLocs)
{
        //*****Models are about to be drawn*****//
	//*****In fact, are the buffers the ones about to be drawed*****//
    struct LD_2D_VRAMBuffer_struct * Vex =LD_2D.VRAMBuffer;
    struct LD_2D_VRAMBuffer_struct * Vend = Vex + LD_2D.Sizeof_VRAMBuffer;

    for(;Vex<Vend;Vex++)
    {   
        if((Vex->FLAG & 0x80) == 0) //Si el búfer es dibujable
        {    
            
            for(int i = 0;i<Vex->TexturesCount;i++)
            {
                GLuint LocalTexture = Vex->Texture [i];
                LocalTexture*=(i<NumLocs);
                glActiveTexture(GL_TEXTURE0+i);
                glBindTexture(GL_TEXTURE_2D,LocalTexture);
                glUniform1i(Textures_Loc[i],0+i);
            }
            
           
            glBindVertexArray(Vex->VAO);
            glDrawElements(GL_TRIANGLES,Vex->SpritesCount*6,GL_UNSIGNED_INT,0);

            for(int i = 0;i<Vex->TexturesCount;i++)
            {
                glActiveTexture(GL_TEXTURE0+i);
                glBindTexture(GL_TEXTURE_2D,0);
                glUniform1i(Textures_Loc[i],0+i);
            }
        }
    }
}



GLuint LD_2D_Normal_Draw(void * ProjectionViewMatrix, unsigned int Alt_Texture)
{   
    GLuint Texture = ((Alt_Texture==0)*LD_2D_DL.Buffers.ColorBuffer)+Alt_Texture;

        glBindFramebuffer(GL_FRAMEBUFFER,LD_2D_DL.Buffers.FrameBuffer);

/*
        glViewport(0,0,
        LD_2D_DL.Configuration.Options.Width,
        LD_2D_DL.Configuration.Options.Height);
*/
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,
                                Texture,0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glUseProgram(LD_2D_DL.Configuration.CBuffer.Shader);
        glUniformMatrix4fv(LD_2D_DL.Configuration.CBuffer.Uniforms.ProjectionViewMatrix,
                            1,0,ProjectionViewMatrix);
    
        LD_2D_Draw_C_Pass(LD_2D_DL.Configuration.CBuffer.Uniforms.Textures,
                            LD_2D_DL.Configuration.CBuffer.Uniforms.TexturesCount);

        glBindFramebuffer(GL_FRAMEBUFFER,0);

        return Texture;
}

#endif
