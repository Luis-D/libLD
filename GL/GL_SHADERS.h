/*IT HAVE TO BE INCLUDED BEFORE (NOT AFTER) THE INCLUSION OF AN OPENGL HEADER*/

/*This header handles Opengl shader loading*/


#ifndef __GL_SHADERS_H
#define __GL_SHADERS_H


/*This same function may be in many other places, so if it have not been defined,
  define it here*/
    #ifndef _LD_READ_READFULLTEXT_FUNC
    #define _LD_READ_READFULLTEXT_FUNC
    #include <string.h>
    #define ReadLineBuffer 256
    char * ReadFullText(FILE * file, int * String_Lenght_Return)
    {
        if(file == NULL){return NULL;}
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


GLuint GL_Shader_Create(const char * const * String_ptr_ptr, GLenum shaderType,GLint * success)
{
    GLuint _shader;
    _shader = glCreateShader(shaderType);
    //printf(" Source Code: \n%s\n",*String_ptr_ptr);
    glShaderSource(_shader, 1,String_ptr_ptr, NULL);
    glCompileShader(_shader);
    if(success!=NULL){glGetShaderiv(_shader, GL_COMPILE_STATUS, success);}
    return _shader;
}

GLuint GL_Shader_Load_File_EXT(const char * single_shader_file_path, GLenum shaderType,GLint * success)
{
    GLuint _shader;
    FILE * _File = fopen(single_shader_file_path,"rb");
    if(_File==NULL){printf("ERROR::404::\n%s\n",single_shader_file_path);return 0;}
	char * _Source = ReadFullText(_File,NULL);
    fclose(_File);
    _shader = GL_Shader_Create((const char * const *)&_Source,shaderType,success);
    free(_Source);
    return _shader;
}

GLuint GL_Shaders_Load_File(void * vertex_file_path, void * fragment_file_path)
{
    //printf("Loading Shader\n");

    int INFOLOG_LEN = 512;
    GLchar infoLog[INFOLOG_LEN];
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint shader_program;
    GLint success;

    vertex_shader = GL_Shader_Load_File_EXT( (char *) vertex_file_path,GL_VERTEX_SHADER,&success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX: \n%s\n", infoLog);
    }

    fragment_shader = GL_Shader_Load_File_EXT( (char *) fragment_file_path,GL_FRAGMENT_SHADER,&success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT: \n%s\n", infoLog);
    }
 
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::LINK: \n%s\n", infoLog);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}


GLuint GL_Shaders_Load_Strings(const char * Vertex_String, const char * Fragment_String)
{
    int INFOLOG_LEN = 512;
    GLchar infoLog[INFOLOG_LEN];
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint shader_program;
    GLint success;

    vertex_shader = GL_Shader_Create((const char * const *)&Vertex_String,GL_VERTEX_SHADER,&success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX: \n%s\n", infoLog);
    }

    fragment_shader = GL_Shader_Create((const char * const *)&Fragment_String,GL_FRAGMENT_SHADER,&success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT: \n%s\n", infoLog);
    }
 
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::LINK: \n%s\n", infoLog);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

#endif
