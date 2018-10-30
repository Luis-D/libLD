#ifndef _LD_OES2_GLFW3_H
#define _LD_OES2_GLFW3_H

#define GLFW_INCLUDE_ES2
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>


typedef void (*GL_GENVERTEXARRAYS_OES) (GLsizei, GLuint*);
GL_GENVERTEXARRAYS_OES glGenVertexArraysOES;
#define glGenVertexArrays glGenVertexArraysOES

#define glBindVertexArray glBindVertexArrayOES
typedef void (*GL_BINDVERTEXARRAYS_OES) (GLuint);
GL_BINDVERTEXARRAYS_OES glBindVertexArray;

typedef void (*GL_DELETEVERTEXARRAYS_OES) (GLsizei, GLuint*);
GL_DELETEVERTEXARRAYS_OES glDeleteVertexArraysOES;
#define glDeleteVertexArrays glDeleteVertexArraysOES

#define glMapBuffer glMapBufferOES
typedef void * (*GL_MAPBUFFER_OES) (GLenum,GLenum);
GL_MAPBUFFER_OES glMapBuffer;
#define GL_WRITE_ONLY GL_WRITE_ONLY_OES

#define glUnmapBuffer glUnmapBufferOES
typedef void  (*GL_UNMAPBUFFER_OES) (GLenum);
GL_UNMAPBUFFER_OES glUnmapBuffer;

typedef void (*GL_BINDBUFFERRANGE_ARB) (GLenum,GLuint,GLuint,GLintptr,GLsizeiptr);
GL_BINDBUFFERRANGE_ARB glBindBufferRange;

#define glDrawBuffers glDrawBuffersNV
typedef void (*GL_GLDRAWBUFFERS_NV) (GLsizei, const GLenum *);
GL_GLDRAWBUFFERS_NV glDrawBuffers;

#define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
#define GL_RGB16F GL_RGB16F_EXT 


void Extensions_Init()
{
glGenVertexArraysOES = 
(GL_GENVERTEXARRAYS_OES)glfwGetProcAddress("glGenVertexArraysOES");

glBindVertexArray = 
(GL_BINDVERTEXARRAYS_OES)glfwGetProcAddress("glBindVertexArrayOES");

glDeleteVertexArraysOES = 
(GL_DELETEVERTEXARRAYS_OES)glfwGetProcAddress("glDeleteVertexArraysOES");

glMapBuffer =
(GL_MAPBUFFER_OES) glfwGetProcAddress("glMapBufferOES");

glUnmapBuffer = 
(GL_UNMAPBUFFER_OES) glfwGetProcAddress("glUnmapBufferOES");

glBindBufferRange = (GL_BINDBUFFERRANGE_ARB) glfwGetProcAddress("glBindBufferRange");

}

/*************************
 * DECL
 * ***********************/
void GLFW_FullScreen(GLFWwindow * window);
GLFWwindow* GLFW_Create_Window(void * Context_State);
void GLFW_Destroy_Window(void * Context_State);


#endif