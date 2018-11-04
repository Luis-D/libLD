#include "LD_Navi_2D.h"
#include <string.h>

#define Testing_Limit 1000 //Not implementent
#define EPSILON 0.00       //Precision, not implementet


/************
 * DEFS
 * ************/

int Edges_Map_2D_vs_Swept(Edges_Map_2D_struct * Edges_Map,void * Swept_Object, 
int (*Swept_Algorithm) (void*,Line_Segment_2D_Struct*,Collision_2D_Struct*,float),
void (*Response_Algorithm) (void*,Collision_2D_Struct*),
float ClockWise_Multiplier) //-1.f
{
    Collision_2D_Struct Collision_tmp;
    while(1)
    {
        Collision_2D_Struct Collision; Collision.Time=2.f;

        int NC=0;
        for(Line_Segment_2D_Struct* Edexp = Edges_Map->EdgesBuffer; Edexp < Edges_Map->EdgesBuffer+Edges_Map->Edges_Count;Edexp++)
        {
            if(Swept_Algorithm(Swept_Object,(Line_Segment_2D_Struct*) 
            (Edexp),&Collision_tmp, ClockWise_Multiplier))
            {   NC++;
                if(Collision_tmp.Time<Collision.Time)
                {memcpy(&Collision,&Collision_tmp,sizeof(Collision_2D_Struct));}
            }
        }
       
        if(Collision.Time==2.f){return 0;}

   
        Response_Algorithm(Swept_Object,&Collision);
    }

    return 1;
}
