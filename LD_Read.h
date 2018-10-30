#ifndef _LD_READ_H
#define _LD_READ_H

/*This header contains functions that can come in handy when reading files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ReadLineBuffer 256

int StringToInt32(char * String)
{
    return (int) strtol(String,NULL,10);
}

float StringToFloat(char * String)
{
    return (float) strtof(String,NULL); //strtol(String,NULL,10);
}

/*This same function may be in many other places, so if it have not been defined,
  define it here*/
    #ifndef _LD_READ_READFULLTEXT_FUNC
    #define _LD_READ_READFULLTEXT_FUNC
    char * ReadFullText(FILE * file, int * String_Lenght_Return)
    {
        char * DynamicString=NULL;
        if(String_Lenght_Return!=NULL){*String_Lenght_Return=0;}

        char fieof = !feof(file);

        if(fieof)
        {
            char eor=0;
            int maxl = ReadLineBuffer;
            int buffsize = maxl;
            int leng, LENG = 0;
            int index =0;

            DynamicString =(char*) malloc(maxl * sizeof(char));
            do
            {
                fgets (DynamicString+index , buffsize , file);
                fieof = !feof(file);
                eor=1;
                leng = strlen(DynamicString+index)-1;
                if(leng >= buffsize-1 || fieof)
                {
                    maxl*=2;
                    DynamicString = (char*) realloc(DynamicString,maxl);
                    eor=0;
                    index+=leng+1;
                }

                LENG += leng;

            }
            while(eor==0);
            if(String_Lenght_Return!=NULL){*String_Lenght_Return = LENG+4;}
        }

        return DynamicString;
    }
    #endif

#endif