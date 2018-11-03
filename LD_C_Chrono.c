/*Visual Studio not implemented yet*/

#include "LD_C_Chrono.h"
#include <string.h>
#include <stdio.h>

static void __static_Delta_time_init_(int Max_FPS)
{
        struct Delta_Time_Struct * ptr = &CHRONO_STRUCT.DELTA_TIME;
        ptr->MaxFPS=Max_FPS;
        ptr->DeltaBaseFPS=BASEFPS;
        ptr->DBFPSNano = (1000000000L / ptr->DeltaBaseFPS);
        ptr->DFPSNano = 1000000000L / ptr->MaxFPS;
        ptr->tempCounter_A = ptr->DBFPSNano;
        ptr->Delta_Time=0.f;
}

static void __static_Delta_time_Start_()
{
    CHRONO_STRUCT.DELTA_TIME.Delta_Time =
    ((double)CHRONO_STRUCT.DELTA_TIME.tempCounter_A/1000000000.0)/
    ((double)CHRONO_STRUCT.DELTA_TIME.DBFPSNano/1000000000.0);

    //printf("%f\n", (double)CHRONO_STRUCT.DELTA_TIME.tempCounter_A);
}

#ifdef _WIN32   //<- IF WINDOWS
    #include <windows.h>

    LARGE_INTEGER __win_time_frequency;        // ticks per second

    void LD_Chrono_init()
    {
        QueryPerformanceFrequency(&__win_time_frequency);
    }

    float * Delta_time_init(int Max_FPS)
    {   
        LD_Chrono_init();
         struct Delta_Time_Struct * ptr = &CHRONO_STRUCT.DELTA_TIME;
        __static_Delta_time_init_(Max_FPS);
        return &ptr->Delta_Time;
    }

    void Delta_time_Frame_Start()
    {
        QueryPerformanceCounter((LARGE_INTEGER*) &CHRONO_STRUCT.DELTA_TIME.start);
        
        __static_Delta_time_Start_();
    }

    void Delta_time_Frame_End()
    {
        int64_t * ptr_1 = &CHRONO_STRUCT.DELTA_TIME.end;
        int64_t * ptr_2 = &CHRONO_STRUCT.DELTA_TIME.duration;
        int64_t * ptr_4 = &CHRONO_STRUCT.DELTA_TIME.tempCounter_A;

        QueryPerformanceCounter((LARGE_INTEGER*) ptr_1);

        *ptr_2 = ((*ptr_1 - CHRONO_STRUCT.DELTA_TIME.start) * 
        (__win_time_frequency.QuadPart) / (100000));

        *ptr_4 = (CHRONO_STRUCT.DELTA_TIME.DFPSNano - *ptr_2);
        if(*ptr_4<0){*ptr_4 = *ptr_2;}

        #ifdef __MINGW32__ //MINGW
            struct timespec time_;
            memcpy(&time_.tv_nsec,ptr_4,8);
            
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

        __static_Delta_time_init_(Max_FPS);

        return &ptr->Delta_Time;
    }

    void Delta_time_Frame_Start()
    {
        struct timespec time_;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_);
        memcpy(&CHRONO_STRUCT.DELTA_TIME.start,&time_.tv_nsec,8);
        
        __static_Delta_time_Start_();
    }

    void Delta_time_Frame_End()
    {
        uint64_t * ptr_1 = &CHRONO_STRUCT.DELTA_TIME.end;
        uint64_t * ptr_2 = &CHRONO_STRUCT.DELTA_TIME.duration;
        uint64_t * ptr_4 = &CHRONO_STRUCT.DELTA_TIME.tempCounter_A;

        struct timespec time_;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_);
        memcpy(ptr_1,&time_.tv_nsec,8);
        *ptr_2 = *ptr_1 - CHRONO_STRUCT.DELTA_TIME.start;
        *ptr_4 = (CHRONO_STRUCT.DELTA_TIME.DFPSNano - *ptr_2);
        if(*ptr_4<0){*ptr_4 = *ptr_2;}
        memcpy(&time_.tv_nsec,ptr_4,8);

        if(*ptr_4>=1000000000L)
        {
            time_.tv_sec = *ptr_4 % 1000000000L; 
            time_.tv_nsec-=time_.tv_sec;
        }
        else{time_.tv_sec=0;}

        nanosleep(&time_,NULL);
    }

    void Chrono_Frame_Start(Chrono_nano_Timer * timer)
    {
        struct timespec time_;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_);
        memcpy(&timer->start,&time_.tv_nsec,8);
    }

    void Chrono_Frame_End(Chrono_nano_Timer * timer)
    {
        struct timespec time_;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_);
        memcpy(&timer->end,&time_.tv_nsec,8);

        timer->duration = timer->end - timer->start;
    }

#endif