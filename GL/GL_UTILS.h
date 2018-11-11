/*IT HAVE TO BE INCLUDED BEFORE (NOT AFTER) THE INCLUSION OF AN OPENGL HEADER*/

/*This header contains useful code for OPENGL applications development*/
/*Extensions MUST be loaded first*/


#ifndef __GL_UTILS_H
#define __GL_UTILS_H

struct __LD_GL_SIMPLE_ATTR0_OBJ  /*(glDrawElements)*/
{GLuint VAO,EBO,VBO;};
struct __LD_GL_SIMPLE_ATTR0_OBJ GL_Square;
struct __LD_GL_SIMPLE_ATTR0_OBJ GL_Framebuffer;
#define glDrawFB() glDrawElements(GL_Framebuffer.VAO,6,GL_UNSIGNED_INT,0)
#define glDrawSQR() glDrawElements(GL_Square.VAO,6,GL_UNSIGNED_INT,0)
/*The VAO format is interleaved and it may be accessed in the attribute 0.
So in the same vec4 the xy is the Vertex Position and in the zw is the UV*/
void __LD_GL_SIMPLE_ATTR0_OBJ_CREATE_(struct __LD_GL_SIMPLE_ATTR0_OBJ * OBJ, GLfloat * Data, unsigned int * Indices)
{
    glGenVertexArrays(1,&(*OBJ).VAO);
    glGenBuffers(1,&(*OBJ).VBO);
    glGenBuffers(1,&(*OBJ).EBO);
    glBindVertexArray((*OBJ).VAO);
    glBindBuffer(GL_ARRAY_BUFFER, (*OBJ).VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(Data) ,Data,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 16, (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,(*OBJ).EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(Indices),Indices,GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void GL_Utils_init()
{
    GLfloat PT []=
    {
    -1,-1,0,0,  
     -1,1, 0,1,
     1,-1, 1,0,    
     1,1 ,  1,1
    };
    unsigned int Indices []={0,1,2,2,1,3};
    __LD_GL_SIMPLE_ATTR0_OBJ_CREATE_(&GL_Framebuffer,PT,Indices);

    GLfloat PT2 []=
    {-1,-1,0,1,  
     1,-1, 1,1,
     -1,1, 0,0,    
     1,1 ,  1,0};
   unsigned int Indices2 []={0,2,1,1,2,3};
    __LD_GL_SIMPLE_ATTR0_OBJ_CREATE_(&GL_Square,PT2,Indices2);
}


#endif
