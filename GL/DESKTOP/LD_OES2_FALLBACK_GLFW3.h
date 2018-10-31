/*If included, this header must be included AFTER the inclusion of window.h*/
/*This header define rutines for pseudo-OES2 context creation*/
/*In fact it creates regular DESKTOP OPENGL context*/
/*This header shall be used as a fallback for OES2 */
/*It depends on GLFW3 so it have to be linked*/
/*GLFW3 needs to be initialized at run time*/
//GLFW Copyright (c) 2002-2006 Marcus Geelnard Copyright (c) 2006-2016 Camilla Löwy
/*Context have to be created prior to extension loading*/
/*There is no need for an extension loader*/
/*This header already provides a function for extensions load*/

/*Luis Delgado. 2018*/

#ifndef _LD_OES2_GLFW3_H
#define _LD_OES2_GLFW3_H
#define __LD_OES2_FALLBACK

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

/*************************
 * EXT
 * ***********************/
typedef GLuint (*glCreateShader_type) (GLenum);
glCreateShader_type glCreateShader;
void (*glCompileShader) (GLuint);
void (*glShaderSource) (GLuint,GLsizei,const GLchar * const *,const GLint*);
void (*glDeleteShader) (GLuint);
void (*glGetShaderiv) (GLuint,GLenum,GLint *);
void (*glGetShaderInfoLog) (GLuint ,GLsizei ,GLsizei*,GLchar*);
GLuint (*glCreateProgram) (void);
void (*glGetProgramiv) (GLuint program,GLenum,GLint *);
void (*glGetProgramInfoLog) (GLuint,GLsizei,GLsizei*,GLchar*);
void (*glAttachShader) (GLuint,GLuint);
void (*glLinkProgram)  (GLuint program);

/*************************
 * Strucs
 * ***********************/
typedef struct
{
    int Width, Height;
    int FullScreen, RefreshRate;
    int Resizable;
    int MSAA;
    char * String;
    GLFWwindow * window;
    GLFWmonitor * monitor;
    void * Pointer;
} GLFW_Context_Struct;

GLFW_Context_Struct Context_State_Create(int Width,int Height,int FullScreen,
int RefreshRate,int Resizable, char * String, int MSAA);


/*************************
 * DECL
 * ***********************/
void Extensions_Init(void);

/*if monitor == null then window will go windowed, else it will go fullscreen*/ 
void GLFW_FullScreen(void * Context_State,GLFWmonitor * monitor);

GLFWwindow* GLFW_Create_Window(void * Context_State);

void GLFW_Destroy_Window(void * Context_State);

#endif