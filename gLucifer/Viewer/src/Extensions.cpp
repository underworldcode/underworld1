#include "Include.h"

//Define pointers to required gl 2.0 functions
#if defined _WIN32
#define GetProcAddress(arg) wglGetProcAddress((LPCSTR)arg)
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
#endif

#ifdef EXTENSION_POINTERS
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
PFNGLPOINTPARAMETERFVPROC glPointParameterfv;
PFNGLPOINTPARAMETERFPROC glPointParameterf;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLISBUFFERPROC glIsBuffer;
PFNGLMAPBUFFERPROC glMapBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;

PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLISSHADERPROC glIsShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;

PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM2FVPROC glUniform1fv;
PFNGLUNIFORM2FVPROC glUniform2fv;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLISPROGRAMPROC glIsProgram;
#endif

#if not defined _WIN32
getProcAddressFN GetProcAddress = NULL;
#endif 

void OpenGL_Extensions_Init()
{
#ifdef EXTENSION_POINTERS
   //Getting pointers to gl 2.0 functions
#if defined _WIN32
   glActiveTexture = (PFNGLACTIVETEXTUREPROC) GetProcAddress("glActiveTexture");
   glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)GetProcAddress("glDrawRangeElements");
#endif
   glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) GetProcAddress("glBlendFuncSeparate");
   glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) GetProcAddress("glPointParameterfv");
   glPointParameterf = (PFNGLPOINTPARAMETERFPROC) GetProcAddress("glPointParameterf");
   glBindBuffer = (PFNGLBINDBUFFERPROC) GetProcAddress("glBindBuffer");
   glGenBuffers = (PFNGLGENBUFFERSPROC) GetProcAddress("glGenBuffers");
   glIsBuffer = (PFNGLISBUFFERPROC) GetProcAddress("glIsBuffer");
   glMapBuffer = (PFNGLMAPBUFFERPROC) GetProcAddress("glMapBuffer");
   glBufferData = (PFNGLBUFFERDATAPROC) GetProcAddress("glBufferData");
   glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) GetProcAddress("glUnmapBuffer");
   glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) GetProcAddress("glDeleteBuffers");

   //Shader stuff, ARB?
   glCreateShader = (PFNGLCREATESHADERPROC) GetProcAddress("glCreateShader");
   glDeleteShader = (PFNGLDELETESHADERPROC) GetProcAddress("glDeleteShader");
   glShaderSource = (PFNGLSHADERSOURCEPROC) GetProcAddress("glShaderSource");
   glCompileShader = (PFNGLCOMPILESHADERPROC) GetProcAddress("glCompileShader");
   glAttachShader = (PFNGLATTACHSHADERPROC) GetProcAddress("glAttachShader");
   glIsShader = (PFNGLISSHADERPROC) GetProcAddress("glIsShader");
   glGetShaderiv = (PFNGLGETSHADERIVPROC) GetProcAddress("glGetShaderiv");
   glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) GetProcAddress("glGetShaderInfoLog");
   glCreateProgram = (PFNGLCREATEPROGRAMPROC) GetProcAddress("glCreateProgram");
   glDeleteProgram = (PFNGLDELETEPROGRAMPROC) GetProcAddress("glDeleteProgram");
   glLinkProgram = (PFNGLLINKPROGRAMPROC) GetProcAddress("glLinkProgram");
   glGetProgramiv = (PFNGLGETPROGRAMIVPROC) GetProcAddress("glGetProgramiv");
   glUseProgram = (PFNGLUSEPROGRAMPROC) GetProcAddress("glUseProgram");
   glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) GetProcAddress("glGetProgramInfoLog");

   glIsProgram = (PFNGLISPROGRAMPROC) GetProcAddress("glIsProgram");
   glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) GetProcAddress("glGetUniformLocation");
   glUniform1f = (PFNGLUNIFORM1FPROC) GetProcAddress("glUniform1f");
   glUniform1i = (PFNGLUNIFORM1IPROC) GetProcAddress("glUniform1i");
   glUniform2f = (PFNGLUNIFORM2FPROC) GetProcAddress("glUniform2f");
   glUniform1fv = (PFNGLUNIFORM1FVPROC) GetProcAddress("glUniform1fv");
   glUniform2fv = (PFNGLUNIFORM2FVPROC) GetProcAddress("glUniform2fv");
   glUniform3fv = (PFNGLUNIFORM3FVPROC) GetProcAddress("glUniform3fv");
   glUniform4fv = (PFNGLUNIFORM4FVPROC) GetProcAddress("glUniform4fv");
   glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) GetProcAddress("glUniformMatrix4fv");

   glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) GetProcAddress("glGetAttribLocation");
   glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) GetProcAddress("glEnableVertexAttribArray");
   glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) GetProcAddress("glVertexAttribPointer");
   glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) GetProcAddress("glDisableVertexAttribArray");

   glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) GetProcAddress("glGenRenderbuffersEXT");
   glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) GetProcAddress("glBindRenderbufferEXT");
   glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) GetProcAddress("glRenderbufferStorageEXT");
   glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) GetProcAddress("glGenFramebuffersEXT");
   glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) GetProcAddress("glFramebufferRenderbufferEXT");
   glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) GetProcAddress("glFramebufferTexture2DEXT");
   glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) GetProcAddress("glCheckFramebufferStatusEXT");
   glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) GetProcAddress("glBindFramebufferEXT");
   glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) GetProcAddress("glDeleteRenderbuffersEXT");
   glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) GetProcAddress("glDeleteFramebuffersEXT");
#endif
}
