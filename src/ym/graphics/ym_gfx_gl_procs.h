#ifdef WIN32
// Needed for WINAPI macro
#include <Windows.h>
#include <GL/gl.h>

// Not from windows gl, but from header files in directory.
#include <glext.h>
#include <wglext.h>
#else
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>
#endif

extern PFNGLGENBUFFERSPROC              glGenBuffers;
extern PFNGLBINDBUFFERPROC              glBindBuffer;
extern PFNGLBUFFERDATAPROC              glBufferData;
extern PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
extern PFNGLCREATESHADERPROC            glCreateShader;
extern PFNGLSHADERSOURCEPROC            glShaderSource;
extern PFNGLCOMPILESHADERPROC           glCompileShader;
extern PFNGLCREATEPROGRAMPROC           glCreateProgram;
extern PFNGLATTACHSHADERPROC            glAttachShader;
extern PFNGLLINKPROGRAMPROC             glLinkProgram;
extern PFNGLUSEPROGRAMPROC              glUseProgram;
extern PFNGLUNIFORM1FPROC               glUniform1f;
extern PFNGLUNIFORM2FPROC               glUniform2f;
extern PFNGLUNIFORM3FPROC               glUniform3f;
extern PFNGLUNIFORM4FPROC               glUniform4f;
extern PFNGLGETSHADERIVPROC             glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
extern PFNGLGETPROGRAMIVPROC            glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
extern PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
extern PFNGLUNIFORM1IPROC               glUniform1i;
extern PFNGLUNIFORM1UIPROC              glUniform1ui;

extern PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
extern PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
extern PFNGLDELETESHADERPROC            glDeleteShader;
extern PFNGLDELETEPROGRAMPROC           glDeleteProgram;


// Functions already declared on linux
#ifdef WIN32
extern PFNGLACTIVETEXTUREPROC           glActiveTexture;
#endif
