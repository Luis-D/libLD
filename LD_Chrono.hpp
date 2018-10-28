/*Time measurement header API */
/*This is C++, and it uses chrono library in order to be as portable as posible*/
/* It also include function to handle easy Delta Timing and Thread yielding 
 *  so the main loop won't become a resource eater monster.
 *  - Luis
*/

#ifndef _LD_CHRONO_H
#define _LD_CHRONO_H

#include <chrono>
#include <thread>

#define BASEFPS 30

typedef struct
{
    typedef struct
    {
        int MaxFPS;
        int DeltaBaseFPS;
        long long DBFPSMilli;
        long long DFPSMilli;
        long long tempCounter_A;
        long long tempCounter_B;
        std::chrono::nanoseconds DFPSNano;
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        std::chrono::nanoseconds duration;
        float Delta_Time;
    } Delta_Time_Struct; 
    
    typedef struct
    {
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        std::chrono::nanoseconds duration;
    }Simple_Nano_Struct;

    Delta_Time_Struct DELTA_TIME;

} Chrono_Struct; Chrono_Struct CHRONO_STRUCT;

#define Chrono_Timer Chrono_Struct::Simple_Nano_Struct

static auto STATIC_DTptr =  &CHRONO_STRUCT.DELTA_TIME;

float * Delta_time_init(int Max_FPS)
{
    auto ptr = &CHRONO_STRUCT.DELTA_TIME;

    ptr->MaxFPS=Max_FPS;
    ptr->DeltaBaseFPS=BASEFPS;
    ptr->DBFPSMilli = (1000000000 / ptr->DeltaBaseFPS);
    ptr->DFPSMilli =  1000000000 / ptr->MaxFPS;
    ptr->tempCounter_A = ptr->DBFPSMilli ;
    ptr->tempCounter_B = ptr->DBFPSMilli;
    ptr->DFPSNano = std::chrono::nanoseconds(1000000000 / ptr->MaxFPS);
    ptr->Delta_Time=0;
    return &ptr->Delta_Time;
}

void Delta_time_Frame_Start()
{
    STATIC_DTptr->start = std::chrono::high_resolution_clock::now();
    STATIC_DTptr->Delta_Time = ((STATIC_DTptr->tempCounter_A)/1000000.f) / (STATIC_DTptr->DBFPSMilli/1000000.f)  ;
}

void Delta_time_Frame_End()
{
    auto ptr_1 = &STATIC_DTptr->end;
    auto ptr_2 = &STATIC_DTptr->duration;
    auto ptr_3 = &STATIC_DTptr->tempCounter_B;
    auto ptr_4 = &STATIC_DTptr->tempCounter_A;

        *ptr_1  = std::chrono::high_resolution_clock::now();
        *ptr_2 = *ptr_1 - STATIC_DTptr->start;
        *ptr_3 = ptr_2->count();
        std::chrono::nanoseconds SleepTime = std::chrono::nanoseconds(STATIC_DTptr->DFPSMilli)- std::chrono::nanoseconds(*ptr_2);
        *ptr_4 = SleepTime.count();
        if(*ptr_4>=0){*ptr_4 = *ptr_4+ *ptr_3;}else{*ptr_4 = *ptr_3;}
        std::this_thread::sleep_for(SleepTime-std::chrono::nanoseconds(0000015000));
}

void Chrono_Frame_Start(Chrono_Struct::Simple_Nano_Struct * Nano_Struct)
{
       Nano_Struct->start = std::chrono::high_resolution_clock::now();
}

void Chrono_Frame_End(Chrono_Struct::Simple_Nano_Struct * Nano_Struct)
{
        Nano_Struct->end = std::chrono::high_resolution_clock::now();
        Nano_Struct->duration = Nano_Struct->end - Nano_Struct->start;
}

#endif