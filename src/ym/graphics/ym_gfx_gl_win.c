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

static
void*
get_gl_func(const char* name)
{
    void* p = wglGetProcAddress(name);
    if (!p || p == (void*)-1 ||
        p == (void*)0x1 || p == (void*)0x2 || p == (void*)0x3)
    {
        HMODULE mod = LoadLibrary("opengl32.dll");
        p = GetProcAddress(mod, name);
    }

    return p;
}

YM_NO_DISCARD
ym_errc
ym_gfx_init_gl()
{
    glGenBuffers = get_gl_func("glGenBuffers");
    glBindBuffer = get_gl_func("glBindBuffer");
    glBufferData = get_gl_func("glBufferData");
    glGenVertexArrays = get_gl_func("glGenVertexArrays");
    glBindVertexArray = get_gl_func("glBindVertexArray");
    glEnableVertexAttribArray = get_gl_func("glEnableVertexAttribArray");
    glVertexAttribPointer = get_gl_func("glVertexAttribPointer");
    glCreateShader = get_gl_func("glCreateShader");
    glShaderSource = get_gl_func("glShaderSource");
    glCompileShader = get_gl_func("glCompileShader");
    glCreateProgram = get_gl_func("glCreateProgram");
    glAttachShader = get_gl_func("glAttachShader");
    glLinkProgram = get_gl_func("glLinkProgram");
    glUseProgram = get_gl_func("glUseProgram");

    return ym_errc_success;
}
