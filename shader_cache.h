#ifndef SHADER_CACHE_H
#define SHADER_CACHE_H
#include "gl_shader_includes.h" /* key_value */

#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////////////////////////////////////////////////////
typedef struct ShaderCache_struct ShaderCache_struct, *pShaderCache;
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

typedef struct ShaderCache_struct {
    // Methods:
        // selectShader ( myCache, identifier, defines[key_val] array, defines[key_val] array count ) 
            // --> (possibly compiles a new shader) 
            long unsigned int (*selectShader)(pShaderCache, const char*, key_value*, unsigned int );
            // --> returns the unique shader ID hash
        
            // getShaderProgram ( myCache, unique vertex shader hash, unique fragment shader hash )
            pProgram (*getShaderProgram)(pShaderCache, unsigned long int, unsigned long int);
            // --> returns a Program struct holding the shader information
            
            // setTextureCacheItem ( myCache, unique texture id hash, opengl texture id )
            // --> (sets internal cache of loaded textures at element hash to texture id ) 
            void (*setTextureCacheItem)(pShaderCache, long unsigned int, unsigned int);
            // --> returns nothing
            
            // getCachedTexture ( myCache, unique texture id hash )
            unsigned int (*getCachedTexture)(pShaderCache, long unsigned int);
            // --> returns the texture id of requested cache resource or GL_NONE if not found

    key_value* kvsources;
    unsigned int kvcount;
    gl_environment_textures* lastEnv;  // The last displayed environment, it gets reused if not null and loaded.
    void* map_sources;      //  -> (std::map<std::string, std::string>*)
    void* map_shaders;      //  -> (std::map<unsigned long int, GLuint>*)
    void* map_programs;     //  -> (std::map<std::string, Program_struct>*)
    void* map_textures;     //  -> (std::map<unsigned long int, GLuint>*)
} ShaderCache_struct, *pShaderCache;
ShaderCache_struct ShaderCache(key_value* _sources, unsigned int _count, char* _vertSrc, char* _fragSrc);
void destroy_ShaderCache(pShaderCache This);

////////////////////////////////////////////////////////////////////////////////////////

unsigned long int c_stringHash(const char* c_str, unsigned long int seed);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*SHADER_CACHE*/