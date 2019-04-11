/* OPENGL */
/* This header defines a system where Textures are stored in banks */
/* This can make easiest the deletion and administration of textures */
/* This is useful when a centralized texture system scheme is wanted */

/* - Luis Delgado. */

#define TextureLoad OES_Texture_Load_DDS

#ifndef __GL_TEXTURE_BANK_
#define __GL_TEXTURE_BANK_

struct GL_TEXTURES_BANK_Struct
{
    int Number_of_banks;
    struct Bank_Struct
    {
        int Textures_Count;
        GLuint * TexturesBuffer;
    }* BanksBuffer;  
} GL_TEXTURES_BANK;

void GL_TEXTURES_BANK_init(unsigned  int Number_of_banks)
{
    GL_TEXTURES_BANK.BanksBuffer = (struct Bank_Struct *)
    malloc(sizeof(struct Bank_Struct)*Number_of_banks);
    GL_TEXTURES_BANK.Number_of_banks = Number_of_banks;

    for(int i = 0; i<Number_of_banks;i++)
    {
        GL_TEXTURES_BANK.BanksBuffer[i].Textures_Count=0;
        GL_TEXTURES_BANK.BanksBuffer[i].TexturesBuffer=(GLuint*)malloc(sizeof(GLuint));
    }
}

void GL_TEXTURES_BANK_Clear_Bank(unsigned  int Bank_ID)
{
    struct Bank_Struct * Bank = GL_TEXTURES_BANK.BanksBuffer+Bank_ID;
    GLuint * Exp =Bank->TexturesBuffer;
    GLuint * TB = Exp;
    int * tc = &Bank->Textures_Count;

    glDeleteTextures(*tc,TB);

    free(Exp);

    *tc = 0;
}

void GL_TEXTURES_BANK_resize(unsigned int New_Number_of_Banks)
{
    if(New_Number_of_Banks < GL_TEXTURES_BANK.Number_of_banks)
    {
        for(int i = New_Number_of_Banks ;i<GL_TEXTURES_BANK.Number_of_banks;i++)
        {
            GL_TEXTURES_BANK_Clear_Bank(i);
        }
    }

    struct Bank_Struct * tmp;
    tmp = (struct Bank_Struct *) 
    realloc(GL_TEXTURES_BANK.BanksBuffer,sizeof(struct Bank_Struct) 
    * 
    New_Number_of_Banks);

    if(tmp == NULL){printf("ERROR::MEM");return;}

    for(int o = 0; o<New_Number_of_Banks;o++)
    {
        GL_TEXTURES_BANK.BanksBuffer[o].Textures_Count=0;
        GL_TEXTURES_BANK.BanksBuffer[o].TexturesBuffer=(GLuint*)malloc(sizeof(GLuint));
    }

    GL_TEXTURES_BANK.Number_of_banks = New_Number_of_Banks;
    GL_TEXTURES_BANK.BanksBuffer = tmp;
}

GLuint GL_TEXTURES_BANK_Texture_Load(unsigned int Bank_ID,void * File_Path_or_Texture_Data)
{
    struct Bank_Struct * Bank = GL_TEXTURES_BANK.BanksBuffer+Bank_ID;
    int tc = Bank->Textures_Count+1;

    GLuint * Newptr = (GLuint*) realloc(Bank->TexturesBuffer,sizeof(GLuint)*tc);
    if(Newptr == NULL){printf("ERROR::MEM\n");return 0;}

    //printf("Nptr: %x\n",Newptr);

    Newptr[tc-1] = TextureLoad(File_Path_or_Texture_Data,GL_NEAREST,GL_NEAREST);

    Bank->TexturesBuffer = Newptr;
    Bank->Textures_Count = tc;

    return Newptr[tc-1];
}

GLuint GL_TEXTURES_BANK_Texture_Create(unsigned int Bank_ID, 
                                unsigned int WIDTH, unsigned int HEIGHT)
{
    GLuint RET;
    glGenTextures(1,&RET);
    glBindTexture(GL_TEXTURE_2D,RET);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,WIDTH,HEIGHT,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    
    /*CHECK GL_NEAREST*/
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    /*****************/
 
    glBindTexture(GL_TEXTURE_2D,0);

    struct Bank_Struct * Bank = GL_TEXTURES_BANK.BanksBuffer+Bank_ID;
    int tc = Bank->Textures_Count+1;

    GLuint * Newptr = (GLuint*) realloc(Bank->TexturesBuffer,sizeof(GLuint)*tc);
    if(Newptr == NULL){printf("ERROR::MEM\n");return 0;}

    Bank->TexturesBuffer = Newptr;
    Bank->Textures_Count = tc;

    Newptr[tc-1] = RET;

    return RET;
}

//-------------------------------------//

#endif
