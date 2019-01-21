#ifndef _LD_MOUSE_GLFW_H
#define _LD_MOUSE_GLFW_H

#include <GLFW/glfw3.h>

struct _LD_GLFW_Mouse_Struct{
        /**
 Banderas de botones:
bit 0: Left click
bit 1: Right click
bit 2: MIddle click
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

 
    char Flags;           ///<-- Bandera de botones del Mouse
} LD_Mouse;


 void cursor_enter_callback(GLFWwindow* window, int entered)
{
    char * ptr = &LD_Mouse.Flags;

}


 void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    char *  ptr = &LD_Mouse.Flags;
    *ptr &= 207;
    if(yoffset<0){*ptr&=239;*ptr|=32;}///Wheel down
    if(yoffset>0){*ptr&=223;*ptr|=16;}///Wheel up
}

void LD_GLFW_Mouse_Init(GLFWwindow* window)
{
    glfwSetCursorEnterCallback(window, cursor_enter_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

void LD_GLFW_Mouse_Update(GLFWwindow* window)
{

    char * ptr = &LD_Mouse.Flags;
    *ptr &= 240;
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

    LD_Mouse.Space.x = (float) (-1.0+((LD_Mouse.Pixel.Relative.x/(width*1.0))*2.0));
    LD_Mouse.Space.y = (float) (-1.0+((LD_Mouse.Pixel.Relative.y/(height*1.0))*2.0));
}

#endif