#ifndef LD_TEXT_BMP_OES_H
#define LD_TEXT_BMP_OES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Text/ACBMP.h"

#include "../../libLDExtra/LD_Stack_Queue.h"
#include "../../libLDExtra/LD_Tricks.h"
#include "../../LDM/LDM.h"
#include "../GL_UTILS.h"


struct LD_TEXT_String
{
    ACBMP_String * Raw_String;
    
    struct LD_TEXT_String_Transformation;
    {
	struct {float x, y;} Translation;
	struct {float x, y;} Scale;
    }Transformation;
	float Depth,Extra;
    struct LD_TEXT_String_Color{
	float r,g,b,a;} Color;

    unsigned int Pages;
   
    unsigned int CharsCount;
    struct LD_TEXT_String_Char
    {
	struct {struct LDTXY{float x,y;}Origin,Extent;}UV;
	struct LD_TEXT_String_Char_Transformation;
	{
	    float Matrix[4]; //<- 2D Affine transformation
	    struct {float x, y;} Translation;
	}Transformation;
	float Page, Channel;
	struct LD_TEXT_String_Color Color;	
    } * Chars; 

    struct LD_TEXT_String_Graphics
    {
	unsigned int Texture;
	unsigned int VBO; //<- Each vertex is a mat3x4 
	unsigned int EBO;
	unsigned int VAO;
    }Graphics
}

struct LD_TEXT_BMP
{
    struct LD_TEXT_Graphics
    {
	struct LD_TEXT_Graphics_Drawing
	{
	    struct LD_TEXT_Graphics_Drawing_Normal
	    {	
		unsigned int Shader;
	    }Normal;
	    struct LD_TEXT_Graphics_Drawing_Extended
	    {	
		unsigned int Shader;
	    }Extended;
	}Drawing;
    }Graphics;
    
    DoublyLinkedList(LD_TEXT_String) * Texts_LL;

}LD_TEXT;

#define SPACE 0.2
#define TAB SPACE*3

struct LD_TEXT_Sprite_Struct
{
    struct LD_TEXT_Sprite_Vertex_Struct
    {
	float x,y,u,v;
	float r,g,b,a;
	float Page,Channel, Depth, Extra;
    }Vertex[4];
};

void LD_Text_Update(struct LD_TEXT_String * String)
{
    float POS_UV[]=
    {-0.5,-0.5, 0,1,
     0.5,-0.5, 1,1,
     -0.5,0.5, 0,0,
     0.5,0.5, 1,0};
    float POS_SUM[]={0.5,0.5};
    unsigned int Indices []={0,2,1,1,2,3};

    struct LD_TEXT_String_Char * EXP=String->Chars;

    glBindBuffer(GL_ARRAY_BUFFER,String->Graphics.VBO);
    glBufferData(GL_ARRAY_BUFFER,
		    String->CharsCount*sizeof(struct LD_TEXT_Sprite_Struct),
					NULL,GL_DYNAMIC_DRAW);
    struct LD_TEXT_Sprite_Struct * ptr = 
    (struct LD_TEXT_Sprite_Struct *)  glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    array_foreach(struct LD_TEXT_Sprite,EXP,ptr,String->CharsCount)
    {
    	
	EXP++;
    } 
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

struct LD_TEXT_String * LD_Text_Create(struct ACBMP_String * Source, float Max, char Horizontal)
{
    struct ACBMP_String tmp;
    
    tmp.Raw_String = Source;
    tmp.CharsCount == Source->Length - Source->Blank;
    tmp.Chars = (struct LD_TEXT_String_Char*)
		malloc(sizeof(struct LD_TEXT_String_Char)*tmp.CharsCount);
    if(tmp.Chars == NULL){return NULL;}

    float IDENTITY[] = {1,0,0,1};
    memcpy(tmp.Transformation.Matrix,IDENTITY,sizeof(float)*4);
    memset(&tmp.Transformation.Translation,0,sizeof(float)*8);

    tmp.Pages = Source->Font->Common->pages;
    
    struct LDTXY Scale={.x= (float) Source->Font->Common->scaleW,
			.y= (float) Source->Font->Common->scaleH};    
    struct LDTXY ACU={.x = 0,.y=0};
    struct LDTXY COND = {.x = ((float) !Horizonal), .y=((float) Horizonal) };    
    struct LDTXY NOTCOND = {.x = COND.y, .y=COND.x };    
    
    struct LDTXY LASTSIZE;

    struct ACBMPFontChars ** SRCEXP = Source->Chars;
    array_foreach(struct LD_TEXT_String_Char,CharEXP,tmp.Chars,CharsCount)
    {
	char FLAG = 0;
	unsigned int ID = (*SRCEXP)->id;

	if(ID != '\n')
	{
	    
	    FLAG = 1;
	}	
	if(ID != '\r')
	{
	    
	    FLAG = 1;
	}	
	if(ID != '\t')
	{
	    
	    FLAG = 1;
	}	

	if (FLAG)
	{
		float UVXY [] = {(float) (*SRCEXP)->x, (float) (*SRCEXP)->y};
		V2DIV(&CharEXP->UV.Origin,UXVY,Scale);
		float UVXYEXT [] = {(float) (*SRCEXP)->width, (float) (*SRCEXP)->height};
		V2DIV(&CharEXP->UV.Extent,UXVYEXT,Scale);
		    
		M2IDENTITY(CharEXP->Transformation.Matrix);
		memcpy(&CharEXP->Transformation.Translation,ACU,sizeof(float)*8);

		CharEXP->Page = (*SRCEXP)->page;
		CharEXP->Channel = (*SRCEXP)->chnl;

		LASTSIZE.x = UVXYEXT.x + (*SRCEXP)->xadvance;
		LASTSIZE.y = UVXYEXT.y;
	}


	V2MUL(&LASTSIZE,&LASTSIZE,&COND);
	V2ADD(&ACU,&ACU,&LASTSIZE);

	struct LDTXY IF =   (.x= (float) !(ACU.x+LASTSIZE.x >= Max),
			     .y= (float) !(ACU.y+LASTSIZE.y >= Max));
	
	V2MUL(&ACU,&ACU,IF);

	SRCEXP+=!FLAG;
    }


    glGenBuffers(1,tmp.Graphics.VBO);
    glGenBuffers(1,tmp.Graphics.EBO);
    glGenVertexArrays(1,tmp.Graphics.VAO);

    LD_Text_Update(&tmp);   
 
    void * RETURN = DoublyLinkedList_add(LD_TEXT.Texts_LL,tmp);

    return (struct LD_TEXT_String*) RETURN;
}

/*
int BE_Text_Create(ACBMPFontChars* Characters, int Type)
{
    auto ini =  BE_Text.StringsBuffer;
    auto endd = BE_Text.StringsBuffer+BE_Text.Sizeof_StringsBuffer;

    //Hacer búsqueda binaria//
    for(;ini<endd;ini++)
    {
        if(String_ID == ini->ID)
        {break;}
    }
    if(ini>=endd){return -2;}

    //Colocar String_ID en el mapa, pero reordenándolo todo
    
    int * TMsize = &BE_Text.TextsMap.Texts_Count;
    TextMapArraydef * Inicio = BE_Text.TextsMap.Array;
    TextMapArraydef * Mapa = Inicio;
    TextMapArraydef * ttmp = Inicio;
  

  // printf("Trabajando con: %d\n",String_ID);
    if(*TMsize == 0)
    {
        Inicio->ID = String_ID;
        //printf("Sin MED\n");
    }
    else
    {     
        int Ex_tama = (*TMsize);
        TextMapArraydef * Final = Inicio + Ex_tama-1;
        TextMapArraydef * Medio = Inicio + (Ex_tama/2);
        int Mid_tama, ID;

        while(Ex_tama>0)
        {
            Mid_tama = (Ex_tama/2);
           
            Medio = Inicio + Mid_tama;
            
            ID = Medio->ID;
            //printf("Ex_tama: %d | Mid_tama: %d | MED: %d \n",Ex_tama,Mid_tama,ID);

            //Si se va para la izquierda, no se hace nada especial
            if(String_ID > ID) //Si se va para la derecha...
            {
                //printf("derecha\n");
                Inicio = Medio+1;
            
                if(Inicio+(Mid_tama/2) > Final){
                    //printf("se pasa\n");
                    Mid_tama--;}
                
                
            }

            Ex_tama = Mid_tama;
            //printf("Ex_tama: %d\n",Ex_tama);
        }

        //Inicio es la posición encontrada

        int * Limit = &(BE_Text.TextsMap.Texts_Limit);
        int Diferencia = (((int) Inicio - (int) Mapa)/sizeof(TextMapArraydef))+1;
        int Diferencia_2 = (((int) Final - (int) Inicio)/sizeof(TextMapArraydef))+1;

        if( Diferencia > *Limit || Ex_tama+1 > *Limit)
        {
            (*Limit)*=2;

            BE_Text.TextsMap.Array =(TextMapArraydef*) 
            realloc(BE_Text.TextsMap.Array,sizeof(TextMapArraydef) * (*Limit));
        }

        memcpy(Inicio+1,Inicio,sizeof(TextMapArraydef) * Diferencia_2);
        Inicio->ID = String_ID;
    }

    Inicio->Text = (TextStructdef*) malloc(sizeof(TextStructdef));

    auto Texto = Inicio->Text;
    Texto->FLAG=0; Texto->Font = Font_ID;

    float Espacio=BE_Text.Dialogue.Conf.Espacio;
    float Salto_de_Linea=BE_Text.Dialogue.Conf.Salto_de_Linea;
    float NormalWidth=BE_Text.Dialogue.Conf.Witdh;
    float NormalHeigth= BE_Text.Dialogue.Conf.Height;
    //-----------------------------//

    switch(Type) //Rellenar texto de acuerdo al tipo
    {
        default:
        case 0: //Diálogo
        {
            //Texto->Shown_Chars=0;

            float Espacio=BE_Text.Dialogue.Conf.Espacio;
            float Salto_de_Linea=BE_Text.Dialogue.Conf.Salto_de_Linea;
            float NormalWidth=BE_Text.Dialogue.Conf.Witdh;
            float NormalHeigth= BE_Text.Dialogue.Conf.Height;
        }break;
    }

    OGLES_String_Create_UTF8((uint8_t*)ini->CharArray,ini->SizeofString,
    &BE_Text.FontsBuffer[Font_ID].OGLFont,&Texto->Text,NormalWidth,NormalHeigth,
    Espacio,Salto_de_Linea);

     (*TMsize)++;

    return (*TMsize);
}

void BE_Text_Strings_Clear()
{
    auto ini =  BE_Text.StringsBuffer;
    auto endd = ini+BE_Text.Sizeof_StringsBuffer;

    for(;ini<endd;ini++)
    {
        free(ini->CharArray);
    }

    free(BE_Text.StringsBuffer);

    BE_Text.Sizeof_StringsBuffer=0;   
}

void BE_Text_Texts_Clear()
{
    auto ptr = &BE_Text.TextsMap;
    auto ini = ptr->Array;
    auto endd = ini +ptr->Texts_Count;

    for(;ini<endd;ini++)
    {
        OGLES_String_Clear(&ini->Text->Text);
        free(ini->Text);
    }

    free(ptr->Array);
}

void BE_Text_Dialogue_Change_Color(float R,float G,float B,float A)
{
    auto ptr = &BE_Text.Dialogue.Conf;
    ptr->R=R;ptr->G=G;ptr->B=B;ptr->A=A;
}

void BE_Text_Hypertext_Set_Entity(BE_Text_Struct::SuperText_Struct * SuperText,
                                int String_ID) //Es una búsqueda binaria
{
    if(String_ID <0){SuperText->Entity = NULL;return;} 

    int * TMsize = &BE_Text.TextsMap.Texts_Count;

    TextMapArraydef * Inicio = BE_Text.TextsMap.Array;
    TextMapArraydef * Mapa = Inicio;

    int Ex_tama = (*TMsize);
    TextMapArraydef * Final = Inicio + Ex_tama-1;
    TextMapArraydef * Medio = Inicio + (Ex_tama/2);
    int Mid_tama, ID;

    //printf("Buscando: %d | ",String_ID);("ini: %d\n",Inicio->ID);

    while(Ex_tama>=0)
    {
        Mid_tama = (Ex_tama/2);
        Medio = Inicio + Mid_tama ;
        
        ID = Medio->ID;

        //printf("%d\n",ID);

        if(String_ID == ID){Inicio=Medio;break;} //revisar por posible falta de precisión

        //Si se va para la izquierda, no se hace nada especial
        if(String_ID > ID) //Si se va para la derecha...
        {
            //printf("Para la derecha\n");
            Inicio = Medio+1;
        }

        Ex_tama = Mid_tama -1;
        //printf("Tama: %d\n",Ex_tama);

    } 
    //printf("Final: %d\n\n",Inicio->ID);
    SuperText->Entity = Inicio;
    SuperText->Longitud = Inicio->Text->Text.Longitud;
}

void BE_Text_Shown_Chars(BE_Text_Struct::SuperText_Struct * SuperText,char Options)
{
    auto ptr = &SuperText->Shown_Chars;

    switch(Options)
    {  
        case 0:{(*ptr)++;}break;
        case 1:{(*ptr)--;}break;
        default:
        case 2:{*ptr = -1;}break;
        case 4:{*ptr = 0;}break;
    }
}
//------------------------------------//
*/

#undef SPACE
#undef TAB
#endif
