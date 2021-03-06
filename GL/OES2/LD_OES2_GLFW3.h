/*If included, this header must be included AFTER the inclusion of window.h*/
/*This header define rutines for OPENGL ES 2.0 context creation*/
/*It depends on GLFW3 so it have to be linked*/
/*GLFW3 needs to be initialized at run time*/
//GLFW Copyright (c) 2002-2006 Marcus Geelnard Copyright (c) 2006-2016 Camilla Löwy
/*Context have to be created prior to extension loading*/
/*There is no need for an extension loader*/
/*This header already provides a function for OES 2.0 extensions load*/
/*It also defines MACROS so the OES code can be written as GL code*/

/*Luis Delgado. 2018*/

#ifndef _LD_OES2_GLFW3_H
#define _LD_OES2_GLFW3_H

#ifndef LD_OES2_FALLBACK
    #define GLFW_INCLUDE_ES2
#endif 

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>


#ifndef   LD_OES2_FALLBACK
    /*************************
     * EXT
     * ***********************/
    #define glGenVertexArrays glGenVertexArraysOES
    #define glBindVertexArray glBindVertexArrayOES
    #define glDeleteVertexArrays glDeleteVertexArraysOES
    #define glMapBuffer glMapBufferOES
    #define GL_WRITE_ONLY GL_WRITE_ONLY_OES
    #define glUnmapBuffer glUnmapBufferOES
    #define glDrawBuffers glDrawBuffersNV

    typedef void (*GL_GENVERTEXARRAYS_OES) (GLsizei, GLuint*);
    GL_GENVERTEXARRAYS_OES glGenVertexArraysOES;

    typedef void (*GL_BINDVERTEXARRAYS_OES) (GLuint);
    GL_BINDVERTEXARRAYS_OES glBindVertexArray;

    typedef void (*GL_DELETEVERTEXARRAYS_OES) (GLsizei, GLuint*);
    GL_DELETEVERTEXARRAYS_OES glDeleteVertexArraysOES;

    typedef void * (*GL_MAPBUFFER_OES) (GLenum,GLenum);
    GL_MAPBUFFER_OES glMapBuffer;

    typedef void  (*GL_UNMAPBUFFER_OES) (GLenum);
    GL_UNMAPBUFFER_OES glUnmapBuffer;

    typedef void (*GL_BINDBUFFERRANGE_ARB) (GLenum,GLuint,GLuint,GLintptr,GLsizeiptr);
    GL_BINDBUFFERRANGE_ARB glBindBufferRange;

    typedef void (*GL_GLDRAWBUFFERS_NV) (GLsizei, const GLenum *);
    GL_GLDRAWBUFFERS_NV glDrawBuffers;

    #define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
    #define GL_RGB16F GL_RGB16F_EXT 
#endif 



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
#ifndef LD_OES2_FALLBACK
void GL_Extensions_Init(void);
#endif

/*if monitor == null then window will go windowed, else it will go fullscreen*/ 
void GLFW_FullScreen(void * Context_State,GLFWmonitor * monitor);

GLFWwindow* GLFW_Create_Window(void * Context_State);

void GLFW_Destroy_Window(void * Context_State);



GLFW_Context_Struct Context_State_Create(int Width,int Height,int FullScreen,
int RefreshRate,int Resizable, char * String, int MSAA)
{
    
    GLFW_Context_Struct tmp;
    tmp.Width = Width; tmp.Height = Height; tmp.FullScreen = FullScreen; 
    tmp.Resizable = Resizable; tmp.MSAA = MSAA; tmp.String = String;
    return tmp;
}


/*****************************
 * DEFS
 * ***************************/

#define PROBEF(X) printf("%s: %p\n",#X,X);

#ifndef   LD_OES2_FALLBACK
void GL_Extensions_Init(void)
{
    glGenVertexArrays = 
    (GL_GENVERTEXARRAYS_OES)glfwGetProcAddress("GenVertexArraysOES");


    glBindVertexArray = 
    (void (*)(GLuint))glfwGetProcAddress("BindVertexArrayOES");


    glDeleteVertexArrays = 
    (void(*)(GLsizei, GLuint*))glfwGetProcAddress("DeleteVertexArraysOES");
    

    glMapBuffer =
    (void*(*)(GLenum,GLenum)) glfwGetProcAddress("MapBufferOES");
    

    glUnmapBuffer = 
    (void(*)(GLenum)) glfwGetProcAddress("UnmapBufferOES");

/*
    PROBEF(glGenVertexArrays);
    PROBEF(glBindVertexArray);
    PROBEF(glDeleteVertexArrays);
    PROBEF(glMapBuffer);
    PROBEF(glUnmapBuffer);
*/
glBindBufferRange 
     = (void(*)(GLenum,GLuint,GLuint,GLintptr,GLsizeiptr)) glfwGetProcAddress("glBindBufferRange");
}  
#endif

void GLFW_FullScreen(void * Context_State,GLFWmonitor * monitor)
{
    GLFW_Context_Struct * Context = (GLFW_Context_Struct*)Context_State;
    Context->monitor = monitor;
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwSetWindowMonitor(Context->window,Context->monitor,0,0,
    Context->Width,Context->Height,Context->RefreshRate);
}

GLFWwindow* GLFW_Create_Window(void * Context_State) 
{ 
    GLFW_Context_Struct * Context = (GLFW_Context_Struct*)Context_State;
    Context->Pointer = NULL;
    if(Context->FullScreen){Context->monitor = glfwGetPrimaryMonitor();}
    else{Context->monitor=NULL;}

    #ifndef LD_OES2_FALLBACK
        //glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
        //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);    
    #else
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    #endif

/*
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
*/

    glfwWindowHint(GLFW_RESIZABLE,Context->Resizable);
    glfwWindowHint(GLFW_SAMPLES, Context->MSAA );
    glfwWindowHint(GLFW_REFRESH_RATE, Context->RefreshRate);
    GLFWwindow* window = glfwCreateWindow(Context->Width, Context->Height, Context->String, 
    Context->monitor, NULL);
    glfwMakeContextCurrent(window);
    return window;
}

void GLFW_Destroy_Window(void * Context_State)
{
    GLFW_Context_Struct * Context = (GLFW_Context_Struct*)Context_State;
    glfwSetWindowShouldClose(Context->window,GLFW_TRUE);
    if(Context->Pointer!=NULL){free(Context->Pointer);}
}

#endif