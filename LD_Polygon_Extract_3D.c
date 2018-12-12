#include "LD_Polygon_Extract_3D.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*****************************************************
 *  DEFS
******************************************************/

Edge_2D_Struct * Triangles_3D_Extract_Border_Edges_to_2D_no_Z
(float * Compatible_RAW_3D_Triangles_Buffer,
int Number_of_Triangles_in_Buffer,
int * Edges_Count_Return) 
{
    if(Number_of_Triangles_in_Buffer<=0){*Edges_Count_Return=0;return NULL;}

    Edge_2D_Linked_List_Struct * LinkedList = 
    (Edge_2D_Linked_List_Struct*) malloc(sizeof(Edge_2D_Linked_List_Struct));

    Edge_2D_Linked_List_Struct * LinkedListCurrent = LinkedList;
    Edge_2D_Linked_List_Struct * LinkedListExplorer;
    Edge_2D_Linked_List_Struct * LinkedListNext;
    Edge_2D_Linked_List_Struct * LinkedListPast;

    int EC=0;

    Triangle_3D_struct * TriangleBuffer = 
    (Triangle_3D_struct*)Compatible_RAW_3D_Triangles_Buffer;
    Triangle_3D_struct * TriangleBuffer_END = (TriangleBuffer+Number_of_Triangles_in_Buffer);

    Edge_3D_Struct tmpEdge3D;
    Edge_2D_Struct * exp;

    Vertex_3D_Struct * vert;
    Vertex_3D_Struct * vert_exp;
    Vertex_3D_Struct * vert_LAST;

    for(;TriangleBuffer<TriangleBuffer_END;TriangleBuffer++)
    {
        vert = &TriangleBuffer->Point_A;
        vert_exp =vert;
        vert_LAST = &TriangleBuffer->Point_C;
        for(;vert_exp<=vert_LAST;vert_exp++)
        {

            memcpy(&tmpEdge3D.Point_A,vert_exp,sizeof(Vertex_3D_Struct));
            if(vert_exp==vert_LAST)
            {memcpy(&tmpEdge3D.Point_B,vert,sizeof(Vertex_3D_Struct));}
            else{memcpy(&tmpEdge3D.Point_B,vert_exp+1,sizeof(Vertex_3D_Struct));}

             
            char boolean=0;

            if(EC>0)
            {         
                
                LinkedListExplorer = LinkedList;  

                while(LinkedListExplorer!=NULL)
                {
                    exp = &LinkedListExplorer->Data;
                    LinkedListNext = LinkedListExplorer->Next;

                    char bool1=    exp->Point_A[0] == tmpEdge3D.Point_A[0] && 
                        exp->Point_A[1] == tmpEdge3D.Point_A[1] &&
                        exp->Point_B[0] == tmpEdge3D.Point_B[0] && 
                        exp->Point_B[1] == tmpEdge3D.Point_B[1];

                    char bool2 =    
                        exp->Point_B[0] == tmpEdge3D.Point_A[0] && 
                        exp->Point_B[1] == tmpEdge3D.Point_A[1] &&
                        exp->Point_A[0] == tmpEdge3D.Point_B[0] && 
                        exp->Point_A[1] == tmpEdge3D.Point_B[1]; 

                    if(bool1 || bool2)
                    {
                        if(LinkedListExplorer==LinkedList)
                        {
                            LinkedList=LinkedListNext;
                            
                        }
                        else
                        {
                            LinkedListPast->Next=LinkedListNext;
                           
                        }

                        free(LinkedListExplorer);
                    EC--;
                    boolean =1;
                    break;
                    }

                    LinkedListPast=LinkedListExplorer;
                    LinkedListExplorer=LinkedListNext;

                    
                }
            }
            if(boolean==0)
            {
                LinkedListCurrent->Next = (Edge_2D_Linked_List_Struct*)
                malloc(sizeof(Edge_2D_Linked_List_Struct));

                LinkedListExplorer = LinkedListCurrent->Next;

                exp = &LinkedListCurrent->Data;

                memcpy(exp->Point_A,&tmpEdge3D.Point_A,4*2);
                memcpy(exp->Point_B,&tmpEdge3D.Point_B,4*2);

                LinkedListCurrent=LinkedListExplorer;      
                LinkedListCurrent->Next=NULL;
                EC++;  
            }
        }
    }


    LinkedListExplorer = LinkedList;  


    Edge_2D_Struct * Return = (Edge_2D_Struct*) malloc
    (sizeof(Edge_2D_Struct) * EC);

    LinkedListCurrent=LinkedList;

    Edge_2D_Struct * tmp;

    for(exp=Return;exp<(Return+EC);exp++)
    {
        tmp = &LinkedListCurrent->Data;     

        memcpy(exp->Point_A,tmp->Point_A,4*2);
        memcpy(exp->Point_B,tmp->Point_B,4*2);

        LinkedListCurrent=LinkedListCurrent->Next;
    }

    LinkedListCurrent=LinkedList;

    while(LinkedListCurrent!=NULL)
    {
        LinkedListExplorer = LinkedListCurrent->Next;
        free(LinkedListCurrent);
        LinkedListCurrent=LinkedListExplorer;
    }

    if(Edges_Count_Return != NULL){*Edges_Count_Return = EC;}
    return Return;
}

float * Triangle_3D_to_2D
    (float * Compatible_RAW_3D_Triangle_Buffer, int Number_of_Triangles_in_Buffer,
    char Component_to_eliminate)
{   
    Triangle_2D_struct * RET = (Triangle_2D_struct*) 
    malloc(sizeof(Triangle_2D_struct)*Number_of_Triangles_in_Buffer);
  
    if(RET==NULL){return NULL;}
 
    int tmp = 0; int SZ = Number_of_Triangles_in_Buffer * 3;

    float * RETExp = (float*) RET;
    float * T3DExp = Compatible_RAW_3D_Triangle_Buffer;

    switch(Component_to_eliminate)
    {
	case 0:
	{
	    T3DExp++;
	    break;
	}
	
	case 1:
	{
	    memcpy(RETExp,T3DExp,sizeof(float));
	    RETExp++;T3DExp+=2;
	    break;
	}
    }

    for(;tmp<SZ;tmp++)
    {
//	printf("(%f,%f,%f) =",T3DExp[0],T3DExp[1],T3DExp[2]);
	memcpy(RETExp,T3DExp, sizeof(float)*2);
//	printf("(%f,%f)\n",RETExp[0],RETExp[1]);
	T3DExp+=3; //Jump three floats, jump to the next vertex
	RETExp+=2; //Jump two floats, jump to the next vertex
    
    }
 
    return (float*) RET;
}

