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

GLuint GL_Shader_Load_EXT(const char * single_shader_file_path, GLenum shaderType,GLint * success)
{
    GLuint _shader;
    FILE * _File = fopen(single_shader_file_path,"rb");
	char * _Source = ReadFullText(_File,NULL);
    fclose(_File);
    printf("%s Source Code: \n%s\n",single_shader_file_path,_Source);
    _shader = glCreateShader(shaderType);
    glShaderSource(_shader, 1,(const char * const *) &_Source, NULL);
    glCompileShader(_shader);
    if(success!=NULL){glGetShaderiv(_shader, GL_COMPILE_STATUS, success);}
    free(_Source);
    return _shader;
}

GLuint GL_Shaders_Load(const char * vertex_file_path, const char * fragment_file_path)
{
    GLchar infoLog[512];
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint shader_program;
    GLint success;

    vertex_shader = GL_Shader_Load_EXT(vertex_file_path,GL_VERTEX_SHADER,&success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX: \n%s\n", infoLog);
    }

    fragment_shader = GL_Shader_Load_EXT(fragment_file_path,GL_FRAGMENT_SHADER,&success);
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