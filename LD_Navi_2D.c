#include "LD_Navi_2D.h"
#include "Extern/LD_Geometry.h"
#include "Extern/LD_Math.h"
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

	if(Response_Algorithm != NULL)
        {Response_Algorithm(Swept_Object,&Collision);}
	else{break;}
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
//		printf("Cons (%lx): %d\n",Dst,Dst->Connections_Count);
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

/*
	printf("(%lx)|(%f,%f),(%f,%f),(%f,%f) = ",Nodes,Triangle_2D_RAW_Compatible_Buffer[0],Triangle_2D_RAW_Compatible_Buffer[1],Triangle_2D_RAW_Compatible_Buffer[2],Triangle_2D_RAW_Compatible_Buffer[3],Triangle_2D_RAW_Compatible_Buffer[4],Triangle_2D_RAW_Compatible_Buffer[5]);	printf("%f,%f\n",Nodes->x,Nodes->y);
*/

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
	//	printf("Connection added: %lx <-> %lx\n",Nodes,Ex);
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


//---------- A* ----------// 

typedef struct __Node_List_2D__
{
    struct __NLL_2D__
    {
	float G,H,F;
	struct __NLL_2D__ * Parent;
	_NavNode_2D_*Node;

	int Branches_Count;
	struct _NavNode_2D_ ** Branches_Array;

	struct 	__NLL_2D__ * Next;
	struct  __NLL_2D__ * Last;
    }*First;
    struct __NLL_2D__ * Last;
}__Node_List_2D___;
typedef struct __NLL_2D__       __NLL_2D__;
typedef struct __Node_List_2D__ __Node_List_2D__;

static __Node_List_2D__ * Node_List_Init()
{
    __Node_List_2D__ * ret = (__Node_List_2D__*) malloc(sizeof(__Node_List_2D__));
    ret->First=NULL;
    ret->Last=NULL;
    return ret;
}

static __NLL_2D__ * Node_List_add(__Node_List_2D__*List, _NavNode_2D_*Node,__NLL_2D__*Parent,float G,float H)
{
    float F = G+H;
    __NLL_2D__*tmp=List->Last;
    if(List->First == NULL)
    {
	List->First = (__NLL_2D__*)malloc(sizeof(__NLL_2D__));
	List->Last = List->First;
	tmp=List->Last;
	tmp->Next=NULL;
	tmp->Last=NULL;
//printf("\t\tAdded to Head\n");
    }
    else
    {
	__NLL_2D__*New = (__NLL_2D__*)malloc(sizeof(__NLL_2D__));
	
	char Greater = (F > tmp->F);
	//printf("%f > %f ?\n",F,tmp->F);
	
	if(Greater){
//printf("\t\tAdded to tail\n");
	    List->Last->Next = New;
	    New->Last=List->Last;
	    New->Next=NULL;
	    List->Last = New;
//printf("\t\t\tLast: %x\n",New->Last->Node);
	}else{
	    //getchar();
	   // printf("\tNo \n");printf("\t%lx <-- %lx\n",tmp->Node,tmp->Parent->Node);
	    while((tmp!=NULL) && (F <= tmp->F))
	    {tmp=tmp->Last;}

	    //printf("\t(%x)\n",tmp);

	    if(tmp==NULL)
	    {
//printf("\t\tAdded to Head (%f)\n",F);
		List->First->Last=New;
		New->Next=List->First;
		New->Last=NULL;
		List->First=New;
//printf("\t\t\tNext: %x\n",New->Next->Node);
	    }
	    else
	    {
//printf("\t\tAdded to Body (%f)\n",F);
//printf("\t\t\t| Last: %x\n",tmp->Node);
	    //getchar();
		New->Last=tmp;
		New->Next=tmp->Next;
		tmp->Next->Last=New;
		tmp->Next=New;
	    }
	}

	tmp = New;
    }


    {
	_NavNode_2D_ * cmp = NULL;
	if(Parent != NULL){cmp = Parent->Node;}
	
    __NLL_2D__* RET = tmp;
     int NumberOfConnections = Node->Connections_Count; 
     if(NumberOfConnections > 0)
     {
        _NavNode_2D_** Init = (_NavNode_2D_**) 
        malloc(sizeof(_NavNode_2D_*)*NumberOfConnections);
        if(Init ==NULL){return NULL;}
        RET->G = G;
        RET->H = H;
        RET->F=F;
        RET->Branches_Array = Init;
        
        struct _NavNode_2D_** End=Init+NumberOfConnections;
        struct _NavNode_2D_Con_ * CON=Node->Connections_Array;
        
         for(;Init<End;Init++,CON++)
        {
            *Init=NULL;
            _NavNode_2D_ * Curr = CON->Node;

            if(Curr != cmp)
            {
                *Init = Curr;
            }

        }
    }

    RET->Branches_Count = NumberOfConnections;
	tmp->Parent = Parent;
	tmp->F=F;tmp->G=G;tmp->H=H;
	tmp->Node=Node;
	//printf("NODE: %lx, ",tmp->Node);
	//if(Parent!=NULL)printf("PARENT: %lx",Parent->Node);
	//printf("\n");
    }
    return tmp;
}

static void * Node_List_Append(__Node_List_2D__*List,__NLL_2D__*Node)
{
    if(List->Last==NULL )
    {
	List->Last = Node;
	List->First = Node;
    }
    else
    {
 
	List->Last->Next=Node;
	Node->Next=NULL;
    }	
    Node->Last = List->Last;
}

static void * Node_List_remove_NoFree(__Node_List_2D__*List,__NLL_2D__*Node)
{
   __NLL_2D__*N = Node->Next;
   __NLL_2D__*L = Node->Last;
    if(N!=NULL){N->Last = L;}
    else{List->Last = L;}
    if(L!=NULL){L->Next = N;}
    else{List->First=N;}
}

static __NLL_2D__* Node_List_Search(__Node_List_2D__*List, _NavNode_2D_*Node)
{
    __NLL_2D__ * Last = List->Last;
    while(Last!=NULL)
    {
	if(Last->Node == Node)
	{
	     return Last;
	}
	Last=Last->Next;
    }    
    return NULL;
}

static __NLL_2D__* Node_List_Search_LowF(__Node_List_2D__*List)
{
    __NLL_2D__* Explorer = List->First;
//    printf("In List: \n");
    while(Explorer!=NULL)
    {
//	printf("\t%lx",Explorer->Node);
//	printf(" (%f)\n",Explorer->F);
	Explorer=Explorer->Next;
    }
    
   return List->First; 
}

static void Node_List_Destroy(__Node_List_2D__ * List)
{
    __NLL_2D__*Last = List->Last;
    __NLL_2D__*tmp;
    while(Last!=NULL)
    {	
	tmp = Last;
	free(Last->Branches_Array);
	free(Last);
	Last=tmp->Next;
    }
    free(List);
}



static Path_2D_struct *  __Path_2D_Construct()
{
    Path_2D_struct * tmp = (Path_2D_struct*) malloc(sizeof(Path_2D_struct));
    tmp->First = NULL; tmp->Last=NULL;
}

//Add elements but starting from the last element to the first, in reverse
static void __Path_2D_add_reverse_(Path_2D_struct * Path, _NavNode_2D_ * Node)
{
    if(Path->First ==NULL)
    {
	Path->Last =  (_NavNode_2D_LL*) malloc(sizeof(_NavNode_2D_LL));
	Path->First = Path->Last;
	Path->First->Next=NULL;
	memcpy(Path->Destiny,&Node->x,4*__ELEMENTSCOUNT);
    }    
    else
    {
	Path->First->Last = (_NavNode_2D_LL*) malloc(sizeof(_NavNode_2D_LL));
	Path->First->Last->Next =Path->First; 
	Path->First = Path->First->Last;
	memcpy(Path->Origin,&Node->x,4*__ELEMENTSCOUNT);
    }

    Path->First->Last=NULL;
    Path->First->Node = Node;
}

Path_2D_struct * Navi_Map_2D_FindPath(_NavNode_2D_ * Origin, _NavNode_2D_ * Destiny, float Object_Radius)
{
    Path_2D_struct * RET = __Path_2D_Construct();
    float DestinyPos[2]; memcpy(DestinyPos,&Destiny->x,8);
    float OriginPos[2]; memcpy(OriginPos,&Origin->x,8);
    float H = V2Distance(&Origin->x,DestinyPos);
    float G=0;

    __Node_List_2D__ * OpenList = Node_List_Init();
    __Node_List_2D___ * ClosedList = Node_List_Init();
    Node_List_add(OpenList,Origin,NULL,0,0);
    
    while(1)
    {
	__NLL_2D__ * Curr = Node_List_Search_LowF(OpenList);
//__NLL_2D__* Ppp=NULL;if(Curr->Parent!=NULL){printf("P: %lx | ",Curr->Parent->Node);};	
//printf("In %lx, Branches: %d | Destiny: %lx\n",Curr->Node,Curr->Branches_Count,Destiny);

	if(Curr->Node == Destiny)
	{
//	    printf("Destiny reached, starting backtrack\n");
            while(Curr != NULL)
            {
//		printf("%lx\n",Curr->Node);
                __Path_2D_add_reverse_(RET,Curr->Node);
                Curr=Curr->Parent;
            }
//	    printf("Arrived!\n");
            break;
	}

	Node_List_remove_NoFree(OpenList,Curr);
	Node_List_Append(ClosedList,Curr);

	int Branches_Count = Curr->Branches_Count;
	_NavNode_2D_** Branches = Curr->Branches_Array;
	_NavNode_2D_** B_END =	Branches+Branches_Count;

	for(;Branches<B_END;Branches++)
	{
	    if(*Branches!=NULL)
	    {
//		printf("\t Branch: %lx",*Branches);
		if(Node_List_Search(ClosedList,*Branches)!=NULL)
		{
//		    printf("\t\tVisited\n");
		    continue;
		}
		else
		{
		    G = Curr->G + V2Distance(&(*Branches)->x,&Curr->Node->x);
		    H = V2Distance (&(*Branches)->x,&Destiny->x);
	   
		    __NLL_2D__*Found =  Node_List_Search(OpenList,*Branches);
		    if(Found !=NULL)
		    {
				if(G>Found->G)
				{
					continue;
				}
		    }
//		    printf("\t\tAdded to OpenList\n");
		    Node_List_add(OpenList,*Branches,Curr,G,H);
		}
	    }
	}
//	printf("End of cicle\n");	
    }
    
    Node_List_Destroy(OpenList);
    Node_List_Destroy(ClosedList);
    
    return RET;
}

void Path_2D_Destroy(Path_2D_struct * Path)
{
    if(Path != NULL)
    {
	_NavNode_2D_LL * Explorer = Path->First;
	while(Explorer != NULL)
	{
	    _NavNode_2D_LL * tmp = Explorer;
	    Explorer=Explorer->Next;
	    free(tmp); 
	}
	free(Path);
    }
}

//-----------------------------------------------//