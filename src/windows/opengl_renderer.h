#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

// opengl definitions
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB               0x2093
#define WGL_CONTEXT_FLAGS_ARB                     0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB                 0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB    0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#define GL_VERTEX_SHADER                          0x8B31
#define GL_FRAGMENT_SHADER                        0x8B30
#define GL_COMPILE_STATUS                         0x8B81
#define GL_LINK_STATUS                            0x8B82
#define GL_INFO_LOG_LENGTH                        0x8B84

#define GL_ARRAY_BUFFER                           0x8892
#define GL_ELEMENT_ARRAY_BUFFER                   0x8893
#define GL_STATIC_DRAW                            0x88E4
#define GL_DYNAMIC_DRAW                           0x88E8

#define GL_TEXTURE0                               0x84C0

#define GL_MIRRORED_REPEAT                        0x8370


// wglfunctions
typedef char * WINAPI wgl_get_extensions_string_arb(HDC deviceContext);
wgl_get_extensions_string_arb *wglGetExtensionsStringARB;

typedef HGLRC WINAPI wgl_create_context_attribs_arb(
    HDC hdc, 
    HGLRC hShareContext, 
    const int *attribList
);
wgl_create_context_attribs_arb* wglCreateContextAttribsARB;

typedef BOOL WINAPI wgl_choose_pixel_format_arb(
    HDC hdc, 
    const int *piAttribIList, 
    const FLOAT *pfAttribFList,
    UINT nMaxFormats, 
    int *piFormats, 
    UINT *nNumFormats
);
wgl_choose_pixel_format_arb *wglChoosePixelFormatARB;

typedef BOOL WINAPI wgl_swap_interval_ext(int interval);
wgl_swap_interval_ext *wglSwapIntervalEXT;


// opengl functions
typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;


// shader loading
typedef GLuint APIENTRY gl_create_shader(GLenum shaderType);
gl_create_shader *glCreateShader;

typedef void APIENTRY gl_shader_source(
    GLuint shader, 
    GLsizei count, 
    const GLchar **string, 
    const GLint *length
);
gl_shader_source *glShaderSource;

typedef void APIENTRY gl_compile_shader(GLuint shader);
gl_compile_shader *glCompileShader;

typedef GLuint APIENTRY gl_create_program(void);
gl_create_program *glCreateProgram;

typedef void APIENTRY gl_attach_shader(GLuint program, GLuint shader);
gl_attach_shader *glAttachShader;

typedef void APIENTRY gl_detach_shader(GLuint program, GLuint shader);
gl_detach_shader *glDetachShader;

typedef void APIENTRY gl_link_program(GLuint program);
gl_link_program *glLinkProgram;

typedef void APIENTRY gl_get_shader_iv(GLuint shader, GLenum pname, GLint *params);
gl_get_shader_iv *glGetShaderiv;

typedef void APIENTRY gl_get_program_iv(GLuint object, GLenum type, int *param);
gl_get_program_iv *glGetProgramiv;

typedef void APIENTRY gl_get_shader_info_log(GLuint object, int maxLen, int *len, char *log);
gl_get_shader_info_log *glGetShaderInfoLog;

typedef void APIENTRY gl_get_program_info_log(GLuint object, int maxLen, int *len, char *log);
gl_get_program_info_log *glGetProgramInfoLog;

// vaos and buffers
typedef GLint APIENTRY gl_get_attrib_location(GLuint program, const GLchar *name);
gl_get_attrib_location *glGetAttribLocation;

typedef void APIENTRY gl_gen_vertex_arrays(GLsizei n, GLuint *arrays);
gl_gen_vertex_arrays *glGenVertexArrays;

typedef void APIENTRY gl_bind_vertex_array(GLuint array);
gl_bind_vertex_array *glBindVertexArray;

typedef void APIENTRY gl_gen_buffers(GLsizei n, GLuint *buffers);
gl_gen_buffers *glGenBuffers;

typedef void APIENTRY gl_bind_buffer(GLenum target, GLuint buffer);
gl_bind_buffer *glBindBuffer;

typedef void APIENTRY gl_buffer_data(
    GLenum target, 
    GLsizeiptr size, 
    const GLvoid *data, 
    GLenum usage
);
gl_buffer_data *glBufferData;

typedef void APIENTRY gl_enable_vertex_attrib_array(GLuint index);
gl_enable_vertex_attrib_array *glEnableVertexAttribArray;

typedef void APIENTRY gl_disable_vertex_attrib_array(GLuint index);
gl_disable_vertex_attrib_array *glDisableVertexAttribArray;

typedef void APIENTRY gl_vertex_attrib_pointer(
    GLuint index, 
    GLint size, 
    GLenum type, 
    GLboolean normalized,
    GLsizei stride, 
    const GLvoid *pointer
);
gl_vertex_attrib_pointer *glVertexAttribPointer;

typedef void APIENTRY gl_use_program(GLuint program);
gl_use_program *glUseProgram;


// uniforms
typedef GLint APIENTRY gl_get_uniform_location(GLuint program, const char *name);
gl_get_uniform_location *glGetUniformLocation;

typedef void APIENTRY gl_uniform_1i(GLint location, GLint v0);
gl_uniform_1i *glUniform1i;

typedef void APIENTRY gl_uniform_1f(GLint location, GLfloat v0);
gl_uniform_1f *glUniform1f;

typedef void APIENTRY gl_uniform_matrix_4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
gl_uniform_matrix_4fv *glUniformMatrix4fv;


// texture
typedef void APIENTRY gl_active_texture(GLenum texture);
gl_active_texture *glActiveTexture;
typedef void APIENTRY gl_generate_mipmap(GLenum target);
gl_generate_mipmap *glGenerateMipmap;


// renderer things
typedef void debug_print_string(char *string);

struct renderer_memory {
    unsigned int memoryCapacity;
    void *memory;

    debug_print_string *debugPrintString;
};

enum shader_type {
    SHADER_TYPE_DEFAULT,
    SHADER_TYPE_ANIMATED_MODEL,
    SHADER_TYPE_LINES,
    SHADER_TYPE_SPRITE,
    SHADER_TYPE_BACKGROUND_VISUALIZATION,
    SHADER_TYPE_COUNT
};

struct shader_program {
    shader_type type;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;
};

struct openGL_mesh {
    int key;
    GLuint positionBuffer;
    GLuint texCoordBuffer;
    GLuint normalBuffer;
    GLuint indexBuffer;
    int numIndices;
};

// TODO(ebuchholz): unify animated and regular meshes?
struct openGL_animated_mesh {
    int key;
    GLuint positionBuffer;
    GLuint texCoordBuffer;
    GLuint normalBuffer;
    GLuint boneIndexBuffer;
    GLuint boneWeightBuffer;
    GLuint indexBuffer;
    int numIndices;
};

struct openGL_texture {
    int key;
    int width;
    int height;
    GLuint textureID;
};

#define MAX_OPENGL_MESHES 100
#define MAX_OPENGL_ANIMATED_MESHES 100
#define MAX_OPENGL_TEXTURES 100
#define MAX_SPRITES_PER_BATCH 1000

struct openGL_renderer {
    HGLRC context;
    HDC deviceContext;

    int numShaders;
    shader_program shaders[SHADER_TYPE_COUNT];

    int numMeshes;
    openGL_mesh meshes[MAX_OPENGL_MESHES];
    int numAnimatedMeshes;
    openGL_animated_mesh animatedMeshes[MAX_OPENGL_ANIMATED_MESHES];
    int numTextures;
    openGL_texture textures[MAX_OPENGL_TEXTURES];

    // TODO(ebuchholz): push/pop matrix style api, instead of setting these matrices
    matrix4x4 viewMatrix;
    matrix4x4 projMatrix;

    // one set of buffers for all sprites
    GLuint spritePositionBuffer;
    GLuint spriteTextureBuffer;
    GLuint spriteColorBuffer;
    GLuint spriteIndexBuffer;

    float spriteVertexPositions[MAX_SPRITES_PER_BATCH * 4 * 2];
    float spriteTextureCoords[MAX_SPRITES_PER_BATCH * 4 * 2];
    float spriteColors[MAX_SPRITES_PER_BATCH * 4 * 4]; 
    int spriteIndices[MAX_SPRITES_PER_BATCH * 3 * 2];

    GLuint fullScreenQuadBuffer;

    GLuint debugPositionBuffer;
};

#endif
