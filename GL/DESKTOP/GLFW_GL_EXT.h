/* This header defines a routine for extension loading for DESKTOP OPENGL */
/* This header shall be included AFTER the inclusion of the GLFW header*/
/* -Luis Delgado. 2018*/

#ifndef __LD_DESKGL_GLFW_EXT_H
#define __LD_DESKGL_GLFW_EXT_H

#define _GetProcAddress glfwGetProcAddress

/*************************
 * EXT
 * ***********************/
typedef GLuint (*glCreateShader_type) (GLenum);
glCreateShader_type glCreateShader;
void (*glCompileShader) (GLuint);
void (*glShaderSource) (GLuint,GLsizei,const GLchar * const *,const GLint*);
void (*glDeleteShader) (GLuint);
void (*glGetShaderiv) (GLuint,GLenum,GLint *);
void (*glGetShaderInfoLog) (GLuint ,GLsizei ,GLsizei*,GLchar*);
GLuint (*glCreateProgram) (void);
void (*glGetProgramiv) (GLuint program,GLenum,GLint *);
void (*glGetProgramInfoLog) (GLuint,GLsizei,GLsizei*,GLchar*);
void (*glAttachShader) (GLuint,GLuint);
void (*glLinkProgram)  (GLuint program);
void (*glBindVertexArray) (GLuint);
void (*glDeleteVertexArrays) (GLsizei, GLuint*);
void * (*glMapBuffer) (GLenum,GLenum);
void (*glUnmapBuffer) (GLenum);
void (*glBindBufferRange) (GLenum,GLuint,GLuint,GLintptr,GLsizeiptr);
void (*glDeleteBuffers) (GLsizei n,  const GLuint * buffers);
void (*glGenBuffers) (GLsizei n,  GLuint * buffers);
void (*glGenVertexArrays) (GLsizei n,  GLuint * buffers);
void (*glBindBuffer) (GLenum ,GLuint);
void (*glBufferData)(GLenum,GLsizeiptr,const GLvoid *,GLenum);
void (*glVertexAttribPointer)(GLuint,GLint,GLenum,GLboolean,GLsizei,const GLvoid*);
void (*glVertexAttribIPointer)(GLuint,GLint,GLenum,GLsizei,const GLvoid*);
void (*glVertexAttribLPointer)(GLuint,GLint,GLenum,GLsizei,const GLvoid*);
void (*glEnableVertexAttribArray)(GLuint);
void (*glDisableVertexAttribArray)(GLuint);
void (*glEnableVertexArrayAttrib) (GLuint,GLuint);
void (*glDisableVertexArrayAttrib) (GLuint,GLuint);

void (*glUniform1f)(GLint,GLfloat);
void (*glUniform2f)(GLint,GLfloat,GLfloat);
void (*glUniform3f)(GLint,GLfloat,GLfloat,GLfloat);
void (*glUniform4f)(GLint,GLfloat,GLfloat,GLfloat,GLfloat);
 
void (*glUniform1i)(GLint,GLint);
void (*glUniform2i)(GLint,GLint,GLint);
void (*glUniform3i)(GLint,GLint,GLint,GLint);
void (*glUniform4i)(GLint,GLint,GLint,GLint,GLint);
 
void (*glUniform1ui)(GLint,GLuint);
void (*glUniform2ui)(GLint,GLuint,GLuint);
void (*glUniform3ui)(GLint,GLuint,GLuint,GLuint);
void (*glUniform4ui)(GLint,GLuint,GLuint,GLuint,GLuint);
 
void (*glUniform1fv)(GLint,GLsizei,const GLfloat*);
void (*glUniform2fv)(GLint,GLsizei,const GLfloat*);
void (*glUniform3fv)(GLint,GLsizei,const GLfloat*);
void (*glUniform4fv)(GLint,GLsizei,const GLfloat*);

void (*glUniform1iv)(GLint,GLsizei,const GLint*);
void (*glUniform2iv)(GLint,GLsizei,const GLint*);
void (*glUniform3iv)(GLint,GLsizei,const GLint*);
void (*glUniform4iv)(GLint,GLsizei,const GLint*);

void (*glUniform1uiv)(GLint,GLsizei,const GLuint*);
void (*glUniform2uiv)(GLint,GLsizei,const GLuint*);
void (*glUniform3uiv)(GLint,GLsizei,const GLuint*);
void (*glUniform4uiv)(GLint,GLsizei,const GLuint*);
 
void (*glUniformMatrix2fv)(GLint,GLsizei,GLboolean,const GLfloat*);
void (*glUniformMatrix3fv)(GLint,GLsizei,GLboolean,const GLfloat*);
void (*glUniformMatrix4fv)(GLint,GLsizei,GLboolean,const GLfloat*);
void (*glUniformMatrix2x3fv)(GLint,GLsizei,GLboolean,const GLfloat*);
void (*glUniformMatrix3x2fv)(GLint,GLsizei,GLboolean,const GLfloat*);
void (*glUniformMatrix2x4fv)(GLint,GLsizei,GLboolean,const GLfloat*);
void (*glUniformMatrix4x2fv)(GLint,GLsizei,GLboolean,const GLfloat*);
void (*glUniformMatrix3x4fv)(GLint,GLsizei,GLboolean,const GLfloat*);
void (*glUniformMatrix4x3fv)(GLint,GLsizei,GLboolean,const GLfloat*);
void (*glCompressedTexImage2D)
(GLenum,GLint,GLenum,GLsizei,GLsizei,GLint,GLsizei,const GLvoid *);
GLint (*glGetUniformLocation)(GLuint,const GLchar *);
void (*glGenFramebuffers)(GLsizei,GLuint*);
void (*glBindFramebuffer)(GLenum,GLuint);
void (*glFramebufferTexture2D)(GLenum,GLenum,GLenum,GLuint,GLint);
void (*glUseProgram)(GLuint);

void GL_Extensions_Init(void)
{
    glUseProgram = (void(*)(GLuint))_GetProcAddress("glUseProgram");

    glGenFramebuffers = (void(*)(GLsizei,GLuint*))_GetProcAddress("glGenFramebuffers");
    glBindFramebuffer = (void(*)(GLenum,GLuint))_GetProcAddress("glBindFramebuffer");
    glFramebufferTexture2D = (void(*)(GLenum,GLenum,GLenum,GLuint,GLint))
    _GetProcAddress("glFramebufferTexture2D");
    glCompressedTexImage2D = (void (*) 
    (GLenum,GLint,GLenum,GLsizei,GLsizei,GLint,GLsizei,const GLvoid *))
    _GetProcAddress("glCompressedTexImage2D");
    glGetUniformLocation = (GLint (*)(GLuint,const GLchar *))
    _GetProcAddress("glGetUniformLocation");
    glUniformMatrix2fv = (void (*) (GLint,GLsizei,GLboolean,const GLfloat*))
    _GetProcAddress("glUniformMatrix2fv");
    glUniformMatrix3fv = (void (*) (GLint,GLsizei,GLboolean,const GLfloat*))
    _GetProcAddress("glUniformMatrix3fv");
    glUniformMatrix4fv = (void (*) (GLint,GLsizei,GLboolean,const GLfloat*))
    _GetProcAddress("glUniformMatrix4fv");
    glUniformMatrix2x3fv = (void (*) (GLint,GLsizei,GLboolean,const GLfloat*))
    _GetProcAddress("glUniformMatrix2x3fv");
    glUniformMatrix3x2fv = (void (*) (GLint,GLsizei,GLboolean,const GLfloat*))
    _GetProcAddress("glUniformMatrix3x2fv");
    glUniformMatrix2x4fv = (void (*) (GLint,GLsizei,GLboolean,const GLfloat*))
    _GetProcAddress("glUniformMatrix2x4fv");
    glUniformMatrix3x4fv = (void (*) (GLint,GLsizei,GLboolean,const GLfloat*))
    _GetProcAddress("glUniformMatrix3x4fv");
    glUniformMatrix4x3fv = (void (*) (GLint,GLsizei,GLboolean,const GLfloat*))
    _GetProcAddress("glUniformMatrix4x3fv");
    glUniform1f = (void (*) (GLint,GLfloat)) 
    _GetProcAddress("glUniform1f");
    glUniform2f = (void (*) (GLint,GLfloat,GLfloat)) 
    _GetProcAddress("glUniform2f");
    glUniform3f = (void (*) (GLint,GLfloat,GLfloat,GLfloat)) 
    _GetProcAddress("glUniform3f");
    glUniform4f = (void (*) (GLint,GLfloat,GLfloat,GLfloat,GLfloat)) 
    _GetProcAddress("glUniform4f");
    glUniform1i = (void (*) (GLint,GLint)) 
    _GetProcAddress("glUniform1i");
    glUniform2i = (void (*) (GLint,GLint,GLint)) 
    _GetProcAddress("glUniform2i");
    glUniform3i = (void (*) (GLint,GLint,GLint,GLint)) 
    _GetProcAddress("glUniform3i");
    glUniform4i = (void (*) (GLint,GLint,GLint,GLint,GLint)) 
    _GetProcAddress("glUniform4i");
    glUniform1ui = (void (*) (GLint,GLuint)) 
    _GetProcAddress("glUniform1ui");
    glUniform2ui = (void (*) (GLint,GLuint,GLuint)) 
    _GetProcAddress("glUniform2ui");
    glUniform3ui = (void (*) (GLint,GLuint,GLuint,GLuint)) 
    _GetProcAddress("glUniform3ui");
    glUniform4ui = (void (*) (GLint,GLuint,GLuint,GLuint,GLuint)) 
    _GetProcAddress("glUniform4ui");
    glUniform1fv = (void (*) (GLint,GLsizei,const GLfloat*)) 
    _GetProcAddress("glUniform1fv");
    glUniform2fv = (void (*) (GLint,GLsizei,const GLfloat*)) 
    _GetProcAddress("glUniform2fv");
    glUniform3fv = (void (*) (GLint,GLsizei,const GLfloat*)) 
    _GetProcAddress("glUniform3fv");
    glUniform4fv = (void (*) (GLint,GLsizei,const GLfloat*)) 
    _GetProcAddress("glUniform4fv");
    glUniform1iv = (void (*) (GLint,GLsizei,const GLint*)) 
    _GetProcAddress("glUniform1iv");
    glUniform2iv = (void (*) (GLint,GLsizei,const GLint*)) 
    _GetProcAddress("glUniform2iv");
    glUniform3iv = (void (*) (GLint,GLsizei,const GLint*)) 
    _GetProcAddress("glUniform3iv");
    glUniform4iv = (void (*) (GLint,GLsizei,const GLint*)) 
    _GetProcAddress("glUniform4iv");
    glUniform1uiv = (void (*) (GLint,GLsizei,const GLuint*)) 
    _GetProcAddress("glUniform1uiv");
    glUniform2uiv = (void (*) (GLint,GLsizei,const GLuint*)) 
    _GetProcAddress("glUniform2uiv");
    glUniform3uiv = (void (*) (GLint,GLsizei,const GLuint*)) 
    _GetProcAddress("glUniform3uiv");
    glUniform4uiv = (void (*) (GLint,GLsizei,const GLuint*)) 
    _GetProcAddress("glUniform4uiv");  
    glCreateShader = (glCreateShader_type)_GetProcAddress("glCreateShader");
    glCompileShader = (void (*) (GLuint))_GetProcAddress("glCompileShader");
    glDeleteShader = (void (*) (GLuint))_GetProcAddress("glDeleteShader");
    glGetShaderiv = (void (*) (GLuint,GLenum,GLint*))_GetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (void (*) (GLuint,GLsizei,GLsizei*,GLchar*)) 
    _GetProcAddress("glGetShaderInfoLog");
    glCreateProgram =(GLuint (*) (void)) _GetProcAddress("glCreateProgram");
    glShaderSource = (void (*) (GLuint,GLsizei,const GLchar * const *,const GLint*)) 
    _GetProcAddress("glShaderSource");
    glGetProgramiv = (void (*) (GLuint program,GLenum,GLint *))
    _GetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (void(*) (GLuint,GLsizei,GLsizei*,GLchar*) )
    _GetProcAddress("glGetProgramInfoLog");
    glAttachShader = (void (*) (GLuint,GLuint))_GetProcAddress("glAttachShader");
    glLinkProgram = (void (*) (GLuint program))_GetProcAddress("glLinkProgram");
    glBindVertexArray = (void (*)(GLuint))_GetProcAddress("glBindVertexArray");
    glDeleteVertexArrays = (void(*)(GLsizei, GLuint*))
    _GetProcAddress("glDeleteVertexArrays");
    glMapBuffer =(void*(*)(GLenum,GLenum)) _GetProcAddress("glMapBuffer");
    glUnmapBuffer = (void(*)(GLenum)) _GetProcAddress("glUnmapBuffer");
    glBindBufferRange = (void(*)(GLenum,GLuint,GLuint,GLintptr,GLsizeiptr)) 
    _GetProcAddress("glBindBufferRange");
    glDeleteBuffers = (void(*) (GLsizei n,  const GLuint * buffers)) 
    _GetProcAddress("glDeleteBuffers");
    glGenBuffers = (void (*) (GLsizei n,  GLuint * buffers))
    _GetProcAddress("glGenBuffers");
    glGenVertexArrays = (void (*) (GLsizei n,  GLuint * buffers))
    _GetProcAddress("glGenVertexArrays");
    glBindBuffer = (void (*) (GLenum ,GLuint))
    _GetProcAddress("glBindBuffer");
    glBufferData = (void (*) (GLenum,GLsizeiptr,const GLvoid *,GLenum))
    _GetProcAddress("glBufferData");
    glVertexAttribPointer = (void (*) (GLuint,GLint,GLenum,GLboolean,GLsizei,const GLvoid*))
    _GetProcAddress("glVertexAttribPointer");
    glVertexAttribIPointer = (void (*) (GLuint,GLint,GLenum,GLsizei,const GLvoid*))
    _GetProcAddress("glVertexAttribIPointer");
    glVertexAttribLPointer = (void (*) (GLuint,GLint,GLenum,GLsizei,const GLvoid*))
    _GetProcAddress("glVertexAttribLPointer");
    glEnableVertexAttribArray = (void (*) (GLuint)) 
    _GetProcAddress("glEnableVertexAttribArray");
    glDisableVertexAttribArray = (void (*) (GLuint)) 
    _GetProcAddress("glDisableVertexAttribArray");
    glEnableVertexArrayAttrib = (void (*) (GLuint,GLuint))
    _GetProcAddress("glEnableVertexArrayAttrib");
    glDisableVertexArrayAttrib = (void (*) (GLuint,GLuint))
    _GetProcAddress("glDisableVertexArrayAttrib");   
}

#endif