#include "LD_Navi_2D.h"
#include "LD_Math.h"
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
    //printf("Obj: %lx | Cant: %d\n",Swept_Object,Edges_Map->Edges_Count);
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

void Edges_Map_2D_Clear(Edges_Map_2D_struct * Edges_Map)
{
    Edges_Map->Edges_Count=0;
    free(Edges_Map->EdgesBuffer);
}

#define __ELEMENTSCOUNT 2

void __NavNode_2D_Nodes_Connect_(struct _NavNode_2D_*Dst, struct _NavNode_2D_*Src,float ** Common_Edge_ptr_ptr)
{
		(Dst->Connections_Count)++;
		
		if(Dst->Connections_Count==1)
		{
		    Dst->Connections_Array = (struct _NavNode_2D_Con_*) 
		    malloc(sizeof(struct _NavNode_2D_Con_));
		}
		else
		{
		    Dst->Connections_Array = (struct _NavNode_2D_Con_*)
		    realloc(Dst->Connections_Array,
		    sizeof(struct _NavNode_2D_Con_)*Dst->Connections_Count);
		}

		struct _NavNode_2D_Con_ * Cur =
		&Dst->Connections_Array[Dst->Connections_Count-1];
		
		Cur->Node = Src;
		Cur->Common_Edge_Length = V2Distance(Common_Edge_ptr_ptr[0],Common_Edge_ptr_ptr[1]);
}

int Navi_Map_2D_Generate(float * Triangle_2D_RAW_Compatible_Buffer, int Triangles_in_Buffer,
Navi_Map_2D_struct * Navi_Map)
{
    struct _NavNode_2D_*Nodes=(struct _NavNode_2D_*)malloc(sizeof(struct _NavNode_2D_)*Triangles_in_Buffer);  
    if(Nodes==NULL){return 0;}
    
    Navi_Map->Node_Count = Triangles_in_Buffer;
    Navi_Map->Node_Array = Nodes;
    struct _NavNode_2D_ * init = Nodes;
    
    struct _NavNode_2D_ * Node_End = Nodes + Triangles_in_Buffer;

    for(;Nodes<Node_End;Nodes++)
    {
	Triangle_2D_Barycenter(Triangle_2D_RAW_Compatible_Buffer,(float*)&Nodes->x);
	Nodes->Connections_Count = 0;


	printf("(%lx)|(%f,%f),(%f,%f),(%f,%f) = ",Nodes,Triangle_2D_RAW_Compatible_Buffer[0],Triangle_2D_RAW_Compatible_Buffer[1],Triangle_2D_RAW_Compatible_Buffer[2],Triangle_2D_RAW_Compatible_Buffer[3],Triangle_2D_RAW_Compatible_Buffer[4],Triangle_2D_RAW_Compatible_Buffer[5]);	printf("%f,%f\n",Nodes->x,Nodes->y);


	//Check for connections with other nodes, checking other triangles
	struct _NavNode_2D_ * Ex = Nodes-1;
	float * PreTriEx = Triangle_2D_RAW_Compatible_Buffer;
	for(;Ex>=init;Ex--,PreTriEx-=(__ELEMENTSCOUNT*3))
	{
	    float * TriEx = Triangle_2D_RAW_Compatible_Buffer;
	    float * TriEx__end = TriEx+(__ELEMENTSCOUNT*3);	    
	    float * _Vertex__end = PreTriEx;
	    float * Vertex_init = PreTriEx-(__ELEMENTSCOUNT*3);

	    char Equals = 0;	   
	    float * Vertex_ptr[__ELEMENTSCOUNT];  

	    for(;TriEx<TriEx__end;TriEx+=__ELEMENTSCOUNT)
	    {
		for(float * Vertex_=Vertex_init;Vertex_<_Vertex__end;Vertex_+=__ELEMENTSCOUNT)
		{
		   // printf("(%f,%f) & (%f,%f)\n",TriEx[0],TriEx[1],Vertex_[0],Vertex_[1]);	

		    if(Vertex_[0] == TriEx[0] && Vertex_[1] == TriEx[1])
		    {Vertex_ptr[Equals]=Vertex_;Equals++;}
		    if(Equals>1)
		    {
		//	 printf("(%f,%f) , (%f,%f) ",Vertex_ptr[0][0],Vertex_ptr[0][1],Vertex_ptr[1][0],Vertex_ptr[1][1]);	
			TriEx=TriEx__end;break;}
		  //  printf("\n");
		}
	    }

	    if (Equals>1)
	    {
		printf("Connection added: %lx <-> %lx\n",Nodes,Ex);
		__NavNode_2D_Nodes_Connect_(Nodes,Ex,Vertex_ptr);
		__NavNode_2D_Nodes_Connect_(Ex,Nodes,Vertex_ptr);
	    }
	}	
	Triangle_2D_RAW_Compatible_Buffer+=__ELEMENTSCOUNT*3;

    }

    return 1;
}

void Navi_Map_2D_Clear(Navi_Map_2D_struct * Navi_Map)
{
    if(Navi_Map->Node_Count > 0)
    {
	struct _NavNode_2D_*Nodes = Navi_Map->Node_Array;
	struct _NavNode_2D_*Node_End = Nodes + Navi_Map->Node_Count;

	for(;Nodes<Node_End;Nodes++)
	{
	    if(Nodes->Connections_Count>0)
	    {
		free(Nodes->Connections_Array);
	    }
	}
	free(Nodes);
	Navi_Map->Node_Array=NULL;
	Navi_Map->Node_Count=0;
    }
}



char Navi_Map_2D_FindPath(Navi_Map_2D_struct * Navi_Map, _NavNode_2D_ * Origin, _NavNode_2D_ * Destiny,float Object_Radius)
{
    float DestinyPoint[2]; memcpy(DestinyPoint,&Destiny->x,8);
    char Unsolved=1;
    _NavNode_2D_ * Explorer = Origin;

    while(Unsolved)
    {
	
    }
        
}
