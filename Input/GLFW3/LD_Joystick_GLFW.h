#ifndef _LD_INPUT_GLFW_JOYSTICK_H_
#define _LD_INPUT_GLFW_JOYSTICK_H_


struct _LD_GLFW_Joystick_Struct
{
    const float * Axes;
    const unsigned char * Buttons;
    const char * String;
}LD_Joystick;

#define LD_GLFW_Joystick_Exists glfwJoystickPresent;

void LD_GLFW_Joystick_Get_Info(int JoystickConstant)
{
    if(glfwJoystickPresent(JoystickConstant))
    {
        int count;

        LD_Joystick.Buttons = glfwGetJoystickButtons(JoystickConstant, &count);

        /*
        const unsigned char* axes = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
        array_foreach(const unsigned char, item, axes,count)
        {
        printf("%d ",*item);
        }printf("\n");
    */
        LD_Joystick.Axes = glfwGetJoystickAxes(JoystickConstant, &count);
    /* 
        array_foreach(const float, item, LD_Joystick.Axes,count)
        {       
        printf("%f ",*item);
        }printf("\n");*/  

        LD_Joystick.String = glfwGetJoystickName(JoystickConstant);
    }
} 



int LD_GLFW_Joystick_Get_Button(int Button)
{
	return (LD_Joystick.Buttons[Button]);
}

int LD_GLFW_Joystick_Get_Pressed(int Button)
{
	return (LD_Joystick.Buttons[Button]);
}

float LD_GLFW_Joystick_Get_Axis(int Axis)
{
	return LD_Joystick.Axes[Axis];
}

const char * LD_GLFW_Joystick_Get_Name(){return LD_Joystick.String; }

#endif
