#include "LD_OES2_FALLBACK_GLFW3.h"
#include <stdlib.h>
#include <stdio.h>

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

/*  These lines aren't supported or at least not correctly implemented yet
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
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

