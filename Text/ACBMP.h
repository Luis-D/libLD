/* This Header contains functions to load Angel Code BMP Fonts data*/

#ifndef LD_ACBMPF_H
#define LD_ACBMPF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    struct{
        uint16_t fontSize;
        uint8_t bitField;
        uint8_t charSet;
        uint16_t stretchH;
        uint8_t aa;
        uint8_t paddingUp;
        uint8_t paddingRight;
        uint8_t paddingDown;
        uint8_t paddingLeft;
        uint8_t spacingHoriz;
        uint8_t spacingVert;
        uint8_t outline;
        char * fontName;
    } Info;

    struct{
        uint16_t lineHeight;
        uint16_t base;
        uint16_t scaleW;
        uint16_t scaleH;
        uint16_t pages;
        uint8_t bitField;
        uint8_t alphaChnl;
        uint8_t redChnl;
        uint8_t greenChnl;
        uint8_t blueChnl;
    } Common;
    struct{
        char * pageNames; 
        char ** NamePointers; 
    } Pages;

     struct ACBMPFontChars{
        uint32_t id;
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
        uint16_t xoffset;
        uint16_t yoffset;
        uint16_t xadvance;
        uint8_t page;
        uint8_t chnl;
    } * Chars;

    unsigned int NumberOfChars;
}ACBMPFont;

typedef struct ACBMP_String
{
    unsigned int Length;
    struct ACBMPFontChars ** Chars;
    ACBMPFont * Font;
    unsigned int Blanks; 
}ACBMP_String;

ACBMPFont * Load_AngelCodeBMPFont(const char * filename)
{
    FILE * file = fopen(filename, "rb");
    if (file == NULL){printf("ERROR: %s (?) \n", filename);return NULL;}
	
	ACBMPFont * FontStruct = (ACBMPFont*) malloc (sizeof(ACBMPFont));	

        uint32_t Tmp;
        fread(&Tmp,4,1,file);
        //printf("FFV: %x\n",FileFormatVersion);
        if(Tmp != 0x03464d42)
        {printf("ERROR @ %s \n",filename);return NULL;}
            
	    fseek(file,1,SEEK_CUR);
            fread(&Tmp,4,1,file);
            fread(&(FontStruct->Info),14,1,file);
            Tmp -=14;
            FontStruct->Info.fontName =(char *) malloc (Tmp);
            fread(FontStruct->Info.fontName,Tmp,1,file);

            fseek(file,5,SEEK_CUR);
            fread(&(FontStruct->Common),15,1,file);

            fseek(file,1,SEEK_CUR);
            fread(&Tmp,4,1,file);
            FontStruct->Pages.pageNames = (char *)malloc(Tmp);
            fread(FontStruct->Pages.pageNames,Tmp,1,file);
            char * cc = FontStruct->Pages.pageNames;
            char * lc = cc + Tmp-1;
            char ** np = FontStruct->Pages.NamePointers;
            int nz = 1;
            np = (char**) malloc(nz);
            do
            {
                np = (char**) realloc(np,nz);
                np[nz-1] = cc;
                cc =(char*) memchr(cc,'\0',Tmp);
                nz++;
            }
            while(cc<lc);
            nz--;

            fseek(file,1,SEEK_CUR);
            fread(&Tmp,4,1,file);

            FontStruct->Chars =(struct ACBMPFontChars*)  malloc (Tmp);
            fread(FontStruct->Chars,Tmp,1,file);

            FontStruct->NumberOfChars = Tmp / 20;

        
        fclose(file);
	
	return FontStruct; 
}

struct ACBMP_String * ACBMP_String_from_Unicode(void * Unicode_UTF32, unsigned int Length,
						ACBMPFont * Font)
{
    struct ACBMP_String * RETURN = (struct ACBMP_String*) 
				    malloc (sizeof(struct ACBMP_String));
    if(RETURN == NULL){return NULL;}

    RETURN->Chars = (struct ACBMPFontChars **) 
		    malloc (sizeof(struct ACBMPFontChars*)*Length);
    if(RETURN->Chars == NULL){free(RETURN);return NULL;}

    RETURN->Length = Length;
    RETURN->Font = Font;

    unsigned int Blanks = 0;

    unsigned int * UNICODE_EXP = Unicode_UTF32;
    unsigned int * UNICODE_END = UNICODE_EXP+Length;
    struct ACBMPFontChars * RETURN_EXP = RETURN->Chars;
    struct ACBMPFontChars * FONT_INIT = Font->Chars;
    struct ACBMPFontChars * FONT_END = FONT_INIT+Font->NumberOfChars;

    for(;UNICODE_EXP < UNICODE_END; UNICODE_EXP++,RETURN_EXP++)
    {    
	/*Linear searching... not the best approach... */
	for(struct ACBMPFontChars* FONT_EXP = FONT_INIT;FONT_EXP<FONT_END;FONT_EXP++)
	{
	    char FOUND = (*UNICODE_EXP == FONT_EXP->id);
	    

	    if(FOUND)
	    {
		char BLANK = (*UNICODE_EXP == '\n' || *UNICODE_EXP == '\r' || 
			    *UNICODE_EXP == '\t');
		Blanks+=BLANK;
		*RETURN_EXP = FONT_EXP;
		break;
	    }
	}
    }

    RETURN->Blanks = Blanks;

    return RETURN;
}

void ACBMP_String_Destroy(struct ACBMP_String * STRING)
{
    free(STRING->Chars);
    free(STRING);
}

#endif
