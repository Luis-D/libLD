#include "LD_Collision_2D.h"

#include <stdio.h>
#include <math.h> // <- needs libm to be linked. -lm

/****************************************
 * DEFS
 ****************************************/

void Swept_AABB_2D_Create(Swept_AABB_2D_Struct * Swept_AABB_2D, 
float Center_Position_X, float Center_Position_Y,
float Half_Extent_X,float Half_Extent_Y)
{
    Swept_AABB_2D->Center_Position[0]=Center_Position_X;
    Swept_AABB_2D->Center_Position[1]=Center_Position_Y;
    Swept_AABB_2D->Half_Extent[0]=Half_Extent_X;
    Swept_AABB_2D->Half_Extent[1]=Half_Extent_Y;
}

int Swept_AABB_vs_Line_Segment_2D_Check
(void * _Swept_AABB_, Line_Segment_2D_Struct * Line_Segment, Collision_2D_Struct * Return_Collision,float ClockWise_Multiplier)
{
    Swept_AABB_2D_Struct* Swept_AABB = (Swept_AABB_2D_Struct*) _Swept_AABB_;

    //printf("\nCol= (%f,%f),(%f,%f) ",Line_Segment->Point_A[0],Line_Segment->Point_A[1],Line_Segment->Point_B[0],Line_Segment->Point_B[1]);

    //printf("vs (%f,%f),(%f,%f)\nHS: (%f,%f) | ",Swept_AABB->Center_Position[0],Swept_AABB->Center_Position[1],Swept_AABB->Center_Position[0]+Swept_AABB->Direction[0],Swept_AABB->Center_Position[1]+Swept_AABB->Direction[1],Swept_AABB->Half_Extent[0],Swept_AABB->Half_Extent[1]);
    
    float Line_Segment_tmp[4];
    memcpy(Line_Segment_tmp,Line_Segment,16);
    float SeparatingAxis [2];
    float LineDir[2];
    V2V2SUB(&Line_Segment_tmp[2],&Line_Segment_tmp[0],LineDir);
    V2CalculatePerpendicular(LineDir,SeparatingAxis,ClockWise_Multiplier);
    NormalizeVec2(SeparatingAxis,SeparatingAxis);
    //printf("LD: (%f,%f) | LP: (%f,%f)\n",LineDir[0],LineDir[1],SeparatingAxis[0],SeparatingAxis[1]);
    
    float* NormDir = Swept_AABB->Direction;
    float SeparatingAxisDotDir = V2V2Dot(SeparatingAxis,NormDir);
    
    //printf("SADD= (%f,%f).(%f,%f)=%f\n",SeparatingAxis[0],SeparatingAxis[1],NormDir[0],NormDir[1],SeparatingAxisDotDir);
    if(SeparatingAxisDotDir >= 0){return 0;}
    float d = V2V2Dot(SeparatingAxis,&Line_Segment_tmp[0]);
    float SeparatingAxisdotC = V2V2Dot(SeparatingAxis,Swept_AABB->Center_Position);

    float r = 
    (Swept_AABB->Half_Extent[0]*abs(SeparatingAxis[0]) + 
    Swept_AABB->Half_Extent[1]*abs(SeparatingAxis[1]));

    float * Returned_Time = &Return_Collision->Time;

    *Returned_Time = (r + d - SeparatingAxisdotC) / (SeparatingAxisDotDir);
    
    //printf("Time = %f\n",*Returned_Time);

    if(*Returned_Time >= 0 && *Returned_Time <= 1.f)
    {
        float tmp[2]; float tmp2[2]; 

        if(SeparatingAxis[0] == 0 || SeparatingAxis[1] == 0)
        {
            NormalizeVec2(LineDir,tmp2);
            tmp[0]=Swept_AABB->Half_Extent[0]*tmp2[0];
            tmp[1]=Swept_AABB->Half_Extent[1]*tmp2[1];
            V2V2ADD(LineDir,tmp,LineDir);
            V2V2SUB(&Line_Segment_tmp[2],LineDir,&Line_Segment_tmp[0]);
            V2V2ADD(LineDir,tmp,LineDir);       
        }

        float Q[2];
        V2ScalarMUL(Swept_AABB->Direction,*Returned_Time,tmp);
        V2V2ADD(Swept_AABB->Center_Position,tmp,tmp);
        V2ScalarMUL(SeparatingAxis,r,tmp2);
        V2V2SUB(tmp,tmp2,Q);
        V2V2SUB(Q,&Line_Segment_tmp[0],tmp);
        float ftemp= V2V2Dot(LineDir,LineDir);
        float ftemp2= V2V2Dot(LineDir,tmp);
        float dot1 = V2V2Dot(SeparatingAxis,Q);

//	printf("@ (%f,%f) ,0<=%f & %f<=%f\n",Q[0],Q[1],ftemp2,ftemp2,ftemp);

        if(0<=ftemp2 && ftemp2<=ftemp)
        {
            memcpy(Return_Collision->Point,Q,8);
            return 1;   
        }   
    }
    return 0;
}

int Swept_AABB_2D_vs_Static_AABB_2D_Check(void * _Swept_AABB_, AABB_2D_Struct * Static_AABB,Collision_2D_Struct * Collision_Data)
{
    Swept_AABB_2D_Struct* Swept_AABB = (Swept_AABB_2D_Struct*) _Swept_AABB_;

    float tfirst=0.0f,tlast=1.0f;

    float Col_Normal [3] ={1.f,1.f,1.f};
    float Col_Mask [3] = {0,0,0};

    for (int i = 0; i < 3; i++)
    {
        float v =Swept_AABB->Direction[i];
        float B_Center = Swept_AABB->Center_Position[i];
        float B_Max = B_Center + Swept_AABB->Half_Extent[i];
        float B_Min = B_Center - (B_Max*2.f);
        float A_Center = Static_AABB->Center_Position[i];
        float A_Max = A_Center + Static_AABB->Half_Extent[i];
        float A_Min = A_Center - (A_Max*2.f);

        Col_Mask[i]=0.f;

        if(v<0.f)
        {
            if(B_Max < A_Min){return 0;}
            if(A_Max < B_Min){tfirst = fmaxf( (A_Max-B_Min)/(v) , tfirst);}
            if(B_Max > A_Min){tlast = fminf( (A_Min-B_Max)/(v), tlast);}

            Col_Mask[i]=1.f;
        }

        if(v>0.f)
        {
            if(B_Min > A_Max){return 0;}
            if(B_Max < A_Min){tfirst = fmaxf( (A_Min-B_Max)/(v) , tfirst);}
            if(A_Max > B_Min){tlast = fminf( (A_Max-B_Min)/(v), tlast);}  
            Col_Mask[i]=-1.f;         
        }

        if (tfirst > tlast) {return 0;}
    }

    float Col_Point[2];
    V2ScalarMUL (Swept_AABB->Direction,tfirst,Col_Point);
    V2V2ADD(Swept_AABB->Center_Position,Col_Point,Col_Point);

    Collision_Data->Time = tfirst;
    memcpy(Collision_Data->Point,Col_Point,8);

    return 1;
}

int Swept_AABB_2D_vs_Swept_AABB_2D_Check(Swept_AABB_2D_Struct * Swept_AABB_A,Swept_AABB_2D_Struct * Swept_AABB_B,Collision_2D_Struct * Collision_Data)
{
    V2V2SUB(Swept_AABB_B->Direction,Swept_AABB_A->Direction,Swept_AABB_B->Direction);
    return Swept_AABB_2D_vs_Static_AABB_2D_Check
    (Swept_AABB_B,(AABB_2D_Struct*)Swept_AABB_A,Collision_Data);
}

void Swept_AABB_2D_Response_Slide
(void * _Swept_AABB_, Collision_2D_Struct * Collision_Data)
{
    Swept_AABB_2D_Struct* Swept_AABB = (Swept_AABB_2D_Struct*) _Swept_AABB_;

    float Collision_Position[2];
    V2ScalarMUL(Swept_AABB->Direction,Collision_Data->Time,Collision_Position);
    V2V2ADD(Collision_Position,Swept_AABB->Center_Position,Collision_Position);

    float Collision_Normal [2];
    V2V2SUB(Collision_Position,Collision_Data->Point,Collision_Normal);
    NormalizeVec2(Collision_Normal,Collision_Normal);
 
    float Intended_Position [2];
    V2V2ADD(Collision_Data->Point,Swept_AABB->Direction,Intended_Position);

    float LineNormal [2]; V2CalculatePerpendicular(Collision_Normal,LineNormal,1.f);
    NormalizeVec2(LineNormal,LineNormal);
    float ComparitionNormal[2];
    V2V2SUB(Intended_Position,Collision_Data->Point,ComparitionNormal);
    float t = V2V2Dot(ComparitionNormal,LineNormal);
    float ProjectedPoint[2];
    V2ScalarMUL(LineNormal,t,ProjectedPoint);
    V2V2ADD(Collision_Data->Point,ProjectedPoint,ProjectedPoint);

    float New_Direction[2];
    V2V2SUB(ProjectedPoint,Collision_Data->Point,New_Direction);
    V2V2ADD(Collision_Position,New_Direction,Collision_Position);

    memcpy(Swept_AABB->Direction,New_Direction,4*2); 
}
