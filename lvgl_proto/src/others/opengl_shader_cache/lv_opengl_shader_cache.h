#ifndef SHADER_CACHE_H
#define SHADER_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LV_SHADER_CACHE_KEYVAL
#define LV_SHADER_CACHE_KEYVAL
typedef struct {
    const char* key;
    const char* value;
} lv_shader_key_value_t;
#endif /* LV_SHADER_CACHE_KEYVAL */

////////////////////////////////////////////////////////////////////////////////////////
typedef struct lv_opengl_shader_cache_t lv_opengl_shader_cache_t, *pShaderCache;
typedef struct Program_struct Program_struct, *pProgram;
////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
    bool loaded;
    unsigned int diffuse;
    unsigned int specular;
    unsigned int sheen;
    unsigned int ggxLut;
    unsigned int charlieLut;
    unsigned int mipCount;
    float iblIntensityScale;
    float angle;
    
} gl_environment_textures;

////////////////////////////////////////////////////////////////////////////////////////

typedef struct Program_struct {
    // Methods:
        // updateUniform1f ( myProgram, "u_MyShaderFloatProp", 1.23 )
            void (*updateUniform1f)(pProgram, const char*, float);
            // --> returns nothing
        // updateUniform1i ( myProgram, "u_MyShaderIntProp", 123 )
            void (*updateUniform1i)(pProgram, const char*, int);
            // --> returns nothing
    unsigned int program;
    char hash[128];
    void* map_uniforms;     //  -> (std::map<std::string, GLuint>*)
    void* map_attributes;   //  -> (std::map<std::string, GLuint>*)
} Program_struct, *pProgram;
Program_struct Program(unsigned int _program, const char* _hash);
void destroy_Program(pProgram This);

////////////////////////////////////////////////////////////////////////////////////////

typedef struct lv_opengl_shader_cache_t {
    // Methods:
        // selectShader ( myCache, identifier, defines[key_val] array, defines[key_val] array count ) 
            // --> (possibly compiles a new shader) 
            long unsigned int (*selectShader)(lv_opengl_shader_cache_t *, const char*, lv_shader_key_value_t*, unsigned int );
            // --> returns the unique shader ID hash
        
            // getShaderProgram ( myCache, unique vertex shader hash, unique fragment shader hash )
            pProgram (*getShaderProgram)(lv_opengl_shader_cache_t *, unsigned long int, unsigned long int);
            // --> returns a Program struct holding the shader information
            
            // setTextureCacheItem ( myCache, unique texture id hash, opengl texture id )
            // --> (sets internal cache of loaded textures at element hash to texture id ) 
            void (*setTextureCacheItem)(lv_opengl_shader_cache_t *, long unsigned int, unsigned int);
            // --> returns nothing
            
            // getCachedTexture ( myCache, unique texture id hash )
            unsigned int (*getCachedTexture)(lv_opengl_shader_cache_t *, long unsigned int);
            // --> returns the texture id of requested cache resource or GL_NONE if not found

    lv_shader_key_value_t* kvsources;
    unsigned int kvcount;
    unsigned int bg_indexBuf;
    unsigned int bg_vertexBuf;
    unsigned int bg_program;
    unsigned int bg_vao;    
    gl_environment_textures* lastEnv;  // The last displayed environment, it gets reused if not null and loaded.
    void* map_sources;      //  -> (std::map<std::string, std::string>*)
    void* map_shaders;      //  -> (std::map<unsigned long int, GLuint>*)
    void* map_programs;     //  -> (std::map<std::string, Program_struct>*)
    void* map_textures;     //  -> (std::map<unsigned long int, GLuint>*)
} lv_opengl_shader_cache_t, *pShaderCache;
lv_opengl_shader_cache_t lv_opengl_shader_cache_create(lv_shader_key_value_t* _sources, unsigned int _count, char* _vertSrc, char* _fragSrc);
void lv_opengl_shader_cache_destroy(lv_opengl_shader_cache_t * This);

////////////////////////////////////////////////////////////////////////////////////////

unsigned long int c_stringHash(const char* c_str, unsigned long int seed);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*SHADER_CACHE*/