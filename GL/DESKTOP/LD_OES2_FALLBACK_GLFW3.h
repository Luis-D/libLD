/*If included, this header must be included AFTER the inclusion of window.h*/
/*This header define rutines for pseudo-OES2 context creation*/
/*In fact it creates regular DESKTOP OPENGL context*/
/*This header shall be used as a fallback for OES2 */
/*It depends on GLFW3 so it have to be linked*/
/*GLFW3 needs to be initialized at run time*/
//GLFW Copyright (c) 2002-2006 Marcus Geelnard Copyright (c) 2006-2016 Camilla Löwy
/*Context have to be created prior to extension loading*/

/*Luis Delgado. 2018*/

#ifndef _LD_OES2_GLFW3_H
#define _LD_OES2_GLFW3_H
#define __LD_OES2_FALLBACK

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>




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

/*if monitor == null then window will go windowed, else it will go fullscreen*/ 
void GLFW_FullScreen(void * Context_State,GLFWmonitor * monitor);

GLFWwindow* GLFW_Create_Window(void * Context_State);

void GLFW_Destroy_Window(void * Context_State);

#endif