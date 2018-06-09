#include <ym_gfx_gl.h>

PFNGLGENBUFFERSPROC              glGenBuffers;
PFNGLBINDBUFFERPROC              glBindBuffer;
PFNGLBUFFERDATAPROC              glBufferData;
PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
PFNGLCREATESHADERPROC            glCreateShader;
PFNGLSHADERSOURCEPROC            glShaderSource;
PFNGLCOMPILESHADERPROC           glCompileShader;
PFNGLCREATEPROGRAMPROC           glCreateProgram;
PFNGLATTACHSHADERPROC            glAttachShader;
PFNGLLINKPROGRAMPROC             glLinkProgram;
PFNGLUSEPROGRAMPROC              glUseProgram;
PFNGLUNIFORM1FPROC               glUniform1f;
PFNGLUNIFORM2FPROC               glUniform2f;
PFNGLUNIFORM3FPROC               glUniform3f;
PFNGLUNIFORM4FPROC               glUniform4f;
PFNGLGETSHADERIVPROC             glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
PFNGLGETPROGRAMIVPROC            glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
PFNGLUNIFORM1IPROC               glUniform1i;
PFNGLUNIFORM1UIPROC              glUniform1ui;


PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
PFNGLDELETESHADERPROC            glDeleteShader;
PFNGLDELETEPROGRAMPROC           glDeleteProgram;

// Functions already declared on linux
#ifdef WIN32
PFNGLACTIVETEXTUREPROC           glActiveTexture;
#endif

#ifdef WIN32
static
ym_errc
get_gl_func(const char* name, void** ptr)
{
    (*ptr) = wglGetProcAddress(name);
    if (!(*ptr) || (*ptr) == (void*)-1 ||
        (*ptr) == (void*)0x1 || (*ptr) == (void*)0x2 || (*ptr) == (void*)0x3)
    {
        HMODULE mod = LoadLibrary("opengl32.dll");
        (*ptr) = GetProcAddress(mod, name);
    }

    return (*ptr) != NULL
        ? ym_errc_success
        : ym_errc_system_error;
}
#else
static
ym_errc
get_gl_func(const char* name, void** ptr)
{
    (*ptr) = glXGetProcAddress((GLubyte*)name);
    return (*ptr) != NULL
        ? ym_errc_success
        : ym_errc_system_error;
}
#endif

// Take in memory region here, that can be used by gl funcs that requires memory.
YM_NO_DISCARD
ym_errc
ym_gfx_gl_load_procs()
{
    ym_errc errc = ym_errc_success;

    errc |= get_gl_func("glGenBuffers", (void**)&glGenBuffers);
    errc |= get_gl_func("glBindBuffer", (void**)&glBindBuffer);
    errc |= get_gl_func("glBufferData", (void**)&glBufferData);
    errc |= get_gl_func("glGenVertexArrays", (void**)&glGenVertexArrays);
    errc |= get_gl_func("glBindVertexArray", (void**)&glBindVertexArray);
    errc |= get_gl_func("glEnableVertexAttribArray", (void**)&glEnableVertexAttribArray);
    errc |= get_gl_func("glVertexAttribPointer", (void**)&glVertexAttribPointer);
    errc |= get_gl_func("glCreateShader", (void**)&glCreateShader);
    errc |= get_gl_func("glShaderSource", (void**)&glShaderSource);
    errc |= get_gl_func("glCompileShader", (void**)&glCompileShader);
    errc |= get_gl_func("glCreateProgram", (void**)&glCreateProgram);
    errc |= get_gl_func("glAttachShader", (void**)&glAttachShader);
    errc |= get_gl_func("glLinkProgram", (void**)&glLinkProgram);
    errc |= get_gl_func("glUseProgram", (void**)&glUseProgram);
    errc |= get_gl_func("glUniform1f", (void**)&glUniform1f);
    errc |= get_gl_func("glUniform2f", (void**)&glUniform2f);
    errc |= get_gl_func("glUniform3f", (void**)&glUniform3f);
    errc |= get_gl_func("glUniform4f", (void**)&glUniform4f);
    errc |= get_gl_func("glGetShaderiv", (void**)&glGetShaderiv);
    errc |= get_gl_func("glGetShaderInfoLog", (void**)&glGetShaderInfoLog);
    errc |= get_gl_func("glGetProgramiv", (void**)&glGetProgramiv);
    errc |= get_gl_func("glGetProgramInfoLog", (void**)&glGetProgramInfoLog);
    errc |= get_gl_func("glGetUniformLocation", (void**)&glGetUniformLocation);
    errc |= get_gl_func("glUniformMatrix4fv", (void**)&glUniformMatrix4fv);
    errc |= get_gl_func("glUniform1i", (void**)&glUniform1i);
    errc |= get_gl_func("glUniform1ui", (void**)&glUniform1ui);

    errc |= get_gl_func("glDeleteBuffers", (void**)&glDeleteBuffers);
    errc |= get_gl_func("glDeleteVertexArrays", (void**)&glDeleteVertexArrays);
    errc |= get_gl_func("glDeleteShader", (void**)&glDeleteShader);
    errc |= get_gl_func("glDeleteProgram", (void**)&glDeleteProgram);


    // Functions already declared on linux
    #ifdef WIN32
    errc |= get_gl_func("glActiveTexture", (void**)&glActiveTexture);
    #endif

    return errc;
}
