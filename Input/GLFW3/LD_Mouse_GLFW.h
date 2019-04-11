#ifndef _LD_MOUSE_GLFW_H
#define _LD_MOUSE_GLFW_H

#include <GLFW/glfw3.h>

struct _LD_GLFW_Mouse_Struct{
        /**
 Banderas de botones:
bit 0: Left click
bit 1: Right click
bit 2: Middle click
bit 3: Any button
bit 4: Wheel up
bit 5: Wheel down
bit 6: Mouse activated
bit 7: Mouse in screen
**/
    struct
    {
        struct _LD_GLFW_Mouse_Position_Double
        {
            double x,y;
        }Relative;
    }Pixel;

    struct _LD_GLFW_Mouse_Position_Plane
    {
        float x,y;
    }Space;

    GLFWwindow* Window;
 
    char Flags;           ///<-- Bandera de botones del Mouse
} LD_Mouse;


 void cursor_enter_callback(GLFWwindow* window, int entered)
{
    char * ptr = &LD_Mouse.Flags;
    if(entered){ (*ptr) |= 128;}
    else{*ptr &= ~128;}
}

char SCROLLFLAG = 0;
float SCROLLYAXIS = 0.0f;
 void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    /*
    char *  ptr = &LD_Mouse.Flags;
    *ptr &= 207;
    if(yoffset<0){*ptr&=239;*ptr|=32;}///Wheel down
    if(yoffset>0){*ptr&=223;*ptr|=16;}///Wheel up
    //printf("%f\n",yoffset);
    */
   SCROLLYAXIS= yoffset;
    SCROLLFLAG=1;
}

void LD_GLFW_Mouse_Init(GLFWwindow* Window)
{
    glfwSetCursorEnterCallback(Window, cursor_enter_callback);
    glfwSetScrollCallback(Window, scroll_callback);
    LD_Mouse.Window = Window;
}

void LD_GLFW_Mouse_Update(GLFWwindow* window)
{
    
    char * ptr = &LD_Mouse.Flags;
    *ptr &=128;
    
    if(SCROLLFLAG)
    {
        SCROLLFLAG=0;
        if(SCROLLYAXIS<0){*ptr&=239;*ptr|=32;}///Wheel down
        if(SCROLLYAXIS>0){*ptr&=223;*ptr|=16;}///Wheel up
    }
  
    
    char flag = 1;
    for (int i=0;i<4;i++)
    {
        flag <<= i;
        if (glfwGetMouseButton(window, i) == GLFW_PRESS)
            {/*printf("%x\n",flag)*/;*ptr |= (flag);*ptr|=8;}
    }

    glfwGetCursorPos(window, &LD_Mouse.Pixel.Relative.x,&LD_Mouse.Pixel.Relative.y);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float XX = (float) (-1.0+((LD_Mouse.Pixel.Relative.x/(width*1.0))*2.0));
    float YY = (float) (-1.0+((LD_Mouse.Pixel.Relative.y/(height*1.0))*2.0));
    LD_Mouse.Space.x = XX;
    LD_Mouse.Space.y = YY;

   
  

    // printf("%x\n",(char)LD_Mouse.Flags);
    
}

float * LD_GLFW_Mouse_Get_Position()
{return &LD_Mouse.Space.x;}

char LD_GLFW_Mouse_Get_Bit(unsigned int Bit)
{return (LD_Mouse.Flags >> Bit) & 1;}

#endif