//C11

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
        float Delta_Time;
        int MaxFPS;
        int DeltaBaseFPS;
        long long DBFPSNano;
        long long tempCounter_A;
	    long long tempCounter_B;
        long long DFPSNano;       
        long long start;
        long long end;
        long long duration;
    } DELTA_TIME; 
}CHRONO_STRUCT;

#define Chrono_nano_Timer struct Simple_Nano_Struct

float * Delta_time_init(int Max_FPS);

void Delta_time_Frame_Start();

void Delta_time_Frame_End();

void Chrono_Frame_Start(Chrono_nano_Timer * timer);

void Chrono_Frame_End(Chrono_nano_Timer * timer);

void LD_Chrono_init(); //<- Some platforms needs initializacion


/**************
 * DEFS
 * ************/
#include <string.h>
#include <stdio.h>

#ifdef _WIN32   //<- IF WINDOWS
    #include <windows.h>

    LARGE_INTEGER __win_time_frequency;        // ticks per second


    float * Delta_time_init(int Max_FPS)
    {   
        QueryPerformanceFrequency(&__win_time_frequency);
        struct Delta_Time_Struct * ptr = &CHRONO_STRUCT.DELTA_TIME;
        ptr->MaxFPS=Max_FPS;
        ptr->DeltaBaseFPS=BASEFPS;
        ptr->DBFPSNano = (1000000000L / ptr->DeltaBaseFPS);
        ptr->DFPSNano = 1000000000L / ptr->MaxFPS;
        ptr->tempCounter_A = ptr->DBFPSNano;
        ptr->Delta_Time=0.f;
        return &ptr->Delta_Time;
    }

    void Delta_time_Frame_Start()
    {
        QueryPerformanceCounter((LARGE_INTEGER*) &CHRONO_STRUCT.DELTA_TIME.start);



         double Calc =
        (CHRONO_STRUCT.DELTA_TIME.DBFPSNano/1000000000.0) /
        (CHRONO_STRUCT.DELTA_TIME.tempCounter_A/ 1000000000.0);
    CHRONO_STRUCT.DELTA_TIME.Delta_Time = 1.0 /(Calc);

        //printf("%f\n",Calc);
    }

    void Delta_time_Frame_End()
    {
        long long * ptr_1 = &CHRONO_STRUCT.DELTA_TIME.end;
        long long * ptr_2 = &CHRONO_STRUCT.DELTA_TIME.duration;
        long long * ptr_4 = &CHRONO_STRUCT.DELTA_TIME.tempCounter_A;

        QueryPerformanceCounter((LARGE_INTEGER*) ptr_1);

        *ptr_2 = ((*ptr_1 - CHRONO_STRUCT.DELTA_TIME.start) * 
        (__win_time_frequency.QuadPart) / ( 1000000000L));

        //printf("->%lld\n",*ptr_2);

        *ptr_4 = (CHRONO_STRUCT.DELTA_TIME.DFPSNano - *ptr_2);
        if(*ptr_4<0){*ptr_4 = *ptr_2;}

        #ifdef __MINGW32__ //MINGW
            struct timespec time_;
            memcpy(&time_.tv_nsec,ptr_4,sizeof(long long));
            
            if(*ptr_4>=1000000000L)
            {
                time_.tv_sec = *ptr_4 % 1000000000L; 
                time_.tv_nsec-=time_.tv_sec;
            }
            else{time_.tv_sec=0;}

            nanosleep(&time_,NULL);

        #elif _MSC_VER    //Visual Studio
            //No implementation yet

        #endif 
    }

    void Chrono_Frame_Start(Chrono_nano_Timer * timer)
    {
        QueryPerformanceCounter((LARGE_INTEGER*) &timer->start);
    }
//    printf(" %f\n",Calc);
    void Chrono_Frame_End(Chrono_nano_Timer * timer)
    {
        QueryPerformanceCounter((LARGE_INTEGER*) &timer->end);
        timer->duration = 
        ((timer->end - timer->start) * (__win_time_frequency.QuadPart) / (100000));
    }

#elif __unix   //<- IF POSIX COMPLIANT

    void LD_Chrono_init(){return;}

    float * Delta_time_init(int Max_FPS)
    {
        struct Delta_Time_Struct * ptr = &CHRONO_STRUCT.DELTA_TIME;

        ptr->MaxFPS=Max_FPS;
        ptr->DeltaBaseFPS=BASEFPS;
        ptr->DBFPSNano = (1000000000L / ptr->DeltaBaseFPS);
        ptr->DFPSNano = 1000000000L / ptr->MaxFPS;
        ptr->tempCounter_A = ptr->DBFPSNano;
        ptr->Delta_Time=0.f;

        return &ptr->Delta_Time;
    }

    void Delta_time_Frame_Start()
    {
        struct timespec time_;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_);
        memcpy(&CHRONO_STRUCT.DELTA_TIME.start,&time_.tv_nsec,sizeof(long));
        memcpy(&CHRONO_STRUCT.DELTA_TIME.tempCounter_B,&time_.tv_sec,sizeof(long));

         double Calc =
        (CHRONO_STRUCT.DELTA_TIME.DBFPSNano/1000000000.0) /
        (CHRONO_STRUCT.DELTA_TIME.tempCounter_A/ 1000000000.0);
        CHRONO_STRUCT.DELTA_TIME.Delta_Time = 1.0 /(Calc);

        //printf("%f\n",Calc);

    }

    void Delta_time_Frame_End()
    {
        long long * ptr_1 = &CHRONO_STRUCT.DELTA_TIME.end;
        long long * ptr_2 = &CHRONO_STRUCT.DELTA_TIME.duration;
        long long * ptr_4 = &CHRONO_STRUCT.DELTA_TIME.tempCounter_A;
        long long * ptr_5 = &CHRONO_STRUCT.DELTA_TIME.tempCounter_B;
        long long * ptr_6 = &CHRONO_STRUCT.DELTA_TIME.start;
	    long long * ptr_7 = &CHRONO_STRUCT.DELTA_TIME.DFPSNano;

        struct timespec time_;
        
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_);
        memcpy(ptr_1,&time_.tv_nsec,sizeof(long));


        if( * ptr_6 > *ptr_1)
        {
            *ptr_2 = (1000000000L-(*ptr_6))+(*ptr_1);
        }
        else
	    {
            *ptr_2 = *ptr_1 - CHRONO_STRUCT.DELTA_TIME.start;
        }

        *ptr_4 = (CHRONO_STRUCT.DELTA_TIME.DFPSNano - *ptr_2);
        if(*ptr_4<0){*ptr_4 = 0;}
        memcpy(&time_.tv_nsec,ptr_4,sizeof(long));
    
        if(*ptr_4>=1000000000L)
        {
            time_.tv_sec = *ptr_4 % 1000000000L; 
            time_.tv_nsec-=time_.tv_sec;
        }
        else{time_.tv_sec=0;}

	    *ptr_4= (*ptr_2+*ptr_4);
         nanosleep(&time_,NULL);
    }

    void Chrono_Frame_Start(Chrono_nano_Timer * timer)
    {
        struct timespec time_;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_);
        memcpy(&timer->start,&time_.tv_nsec,sizeof(long));
    }

    void Chrono_Frame_End(Chrono_nano_Timer * timer)
    {
        struct timespec time_;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_);
        memcpy(&timer->end,&time_.tv_nsec,sizeof(long));

        timer->duration = timer->end - timer->start;
    }
#endif

#endif
