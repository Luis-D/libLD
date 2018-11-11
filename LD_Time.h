/***********************************************************************
MIT License

Copyright (c) 2018 Luis Delgado

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITD TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/

/*Time Measurement FRAMEWORK*/

/* There are several functions to measure time in order to emulate clocks and timers */
/* It depends on the libLDCC Chrono API */
/* Its behavior may deffer from OS to OS as the libLDCC Chrono API does */

/* - Luis Delgado*/


#ifndef _LD_TIMEr_H
#define _LD_TIMEr_H

#include "LD_C_Chrono.h"

struct LD_Timer_Struct
{
    float * Delta_Time;
    int Size_of_TimersBuffer;

    struct _LD_Timer_Timer_Struct
    { 
        float Limit;
        float Sum;
        float Acumulator;
        int Tic;
    } * TimersBuffer;

} LD_Timer;

void LD_Timer_Timer_Create_Index(int Index,float Seconds)
{
    struct _LD_Timer_Timer_Struct * ptr2 = &LD_Timer.TimersBuffer[Index];

    ptr2->Limit = Seconds;
    ptr2->Sum = 1.f/(BASEFPS * 1.f);
    ptr2->Acumulator=0.f;
    ptr2->Tic=0;   
}

int LD_Timer_Timer_Create(float Seconds) 
{
    int *  tmp = &LD_Timer.Size_of_TimersBuffer;
    (*tmp)++;
    int tmp2 = *tmp-1;
    LD_Timer.TimersBuffer = (struct _LD_Timer_Timer_Struct *) 
    realloc(LD_Timer.TimersBuffer,sizeof(struct _LD_Timer_Timer_Struct) * *tmp);

    struct _LD_Timer_Timer_Struct * ptr2 = &LD_Timer.TimersBuffer[tmp2];

    ptr2->Limit = Seconds;
    ptr2->Sum = 1.f/BASEFPS; 
    ptr2->Acumulator=0.f;
    ptr2->Tic=0;

    return (tmp2);
}

float * LD_Timer_init(int Timers_Count,int FPS)
{
    LD_Timer.Delta_Time = Delta_time_init(FPS);
 
    LD_Timer.Size_of_TimersBuffer = Timers_Count;
    LD_Timer.TimersBuffer = (struct _LD_Timer_Timer_Struct *) 
    malloc (sizeof(struct _LD_Timer_Timer_Struct) * Timers_Count);

    for(struct _LD_Timer_Timer_Struct * A = LD_Timer.TimersBuffer;A < LD_Timer.TimersBuffer+LD_Timer.Size_of_TimersBuffer;A++)
    {
        A->Sum=0;
    }


    return LD_Timer.Delta_Time;
}

int LD_Timer_Restart(int Timer_ID)
{
    LD_Timer.TimersBuffer[Timer_ID].Acumulator=0;
    LD_Timer.TimersBuffer[Timer_ID].Tic=0;
}

int LD_Timer_Clear(int Timer_ID)
{
    LD_Timer.TimersBuffer[Timer_ID].Sum=0;

}

int LD_Timer_TimersBuffer_Resize(int New_Count)
{
    struct _LD_Timer_Timer_Struct * tmp = 
    (struct _LD_Timer_Timer_Struct *) realloc
    (LD_Timer.TimersBuffer,New_Count);

    if(tmp==NULL){printf("ERROR::MEM\n");return -1;}

    LD_Timer.TimersBuffer = tmp;
    LD_Timer.Size_of_TimersBuffer = New_Count;

    return New_Count;   
}

void LD_Timer_Update()
{
    struct _LD_Timer_Timer_Struct * A = LD_Timer.TimersBuffer;
    struct _LD_Timer_Timer_Struct * B = A+LD_Timer.Size_of_TimersBuffer;
    float * tmp = NULL; float * tmp2 = NULL; int *tmp3 = NULL;

    for(;A<B;A++)
    {
        if(A->Sum>0)
        {
            tmp = &A->Acumulator;
            tmp2 = &A->Limit;
            tmp3 = &A->Tic;        *tmp3 = 0;
            (*tmp)+=A->Sum * *LD_Timer.Delta_Time;
            if(*tmp >= *tmp2)
            {
                //Search for a mathematical way to do this loop//
                while(*tmp >= *tmp2)
                {
                    (*tmp)-=(*tmp2);
                    if (*tmp<0){*tmp=0;}
                    (*tmp3)++;
                    //printf("%f | %f\n",*tmp,*tmp2);
                }
                //printf("Fin\n");
                //---------------------------------------------//
            }
        }
    }
}
///---------------------------------////

#endif