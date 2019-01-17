#ifndef _LD_KB_GLFW_H_
#define _LD_KB_GLFW_H_

#include <GLFW/glfw3.h>

struct _LD_GLFW_Keyboard_Struct{
    GLFWwindow * Window;
} LD_Keyboard;

void LD_GLFW_Keyboard_Init(GLFWwindow*Window)
{
    LD_Keyboard.Window=Window;
}

int LD_GLFW_GetKey(int Key)
{return (glfwGetKey(LD_Keyboard.Window,Key));}

#endif