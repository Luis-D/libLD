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
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/


/*Time measurement header API */
/*It may be used for Delta Timing*/
/*It may be used for the main loop 
  so the exectuble won't become a resource eater monster*/


#ifndef _LD_CHRONO_H
#define _LD_CHRONO_H

#include <time.h> //<- Posix 
#include <stdint.h>

#define BASEFPS 30 //Used as reference point, it should not be modified


typedef struct Simple_Nano_Struct
{
    uint64_t start;
    uint64_t end;
    uint64_t duration;
}Simple_Nano_Struct;

struct Chrono_Struct
{
    struct Delta_Time_Struct
    {
        int MaxFPS;
        int DeltaBaseFPS;
        uint64_t DBFPSNano;
        uint64_t tempCounter_A;
        uint64_t DFPSNano;       
        uint64_t start;
        uint64_t end;
        uint64_t duration;
        float Delta_Time;
    } DELTA_TIME; 
}CHRONO_STRUCT;

#define Chrono_nano_Timer struct Simple_Nano_Struct

float * Delta_time_init(int Max_FPS);

void Delta_time_Frame_Start();

void Delta_time_Frame_End();

void Chrono_Frame_Start(Chrono_nano_Timer * timer);

void Chrono_Frame_End(Chrono_nano_Timer * timer);

void LD_Chrono_init(); //<- Some platforms needs initializacion

#endif