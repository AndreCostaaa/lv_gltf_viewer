#ifndef IBL_SAMPLER_H
#define IBL_SAMPLER_H

#ifndef VERBOSITY_MIN
    #define VERBOSITY_MIN 1
    #define VERBOSITY_MED 2
    #define VERBOSITY_MAX 3
#endif
#define IBL_SAMPLER_VERBOSITY VERBOSITY_MAX


#include "shader_cache.h"

#ifdef __cplusplus
class iblSampler
{
    public:
    
    unsigned int textureSize;
    unsigned int ggxSampleCount;
    unsigned int lambertianSampleCount;
    unsigned int sheenSamplCount;
    float lodBias;
    unsigned int lowestMipLevel;
    unsigned int lutResolution;

    float scaleValue;

    unsigned int mipmapCount;

    unsigned int lambertianTextureID;
    unsigned int ggxTextureID;
    unsigned int sheenTextureID;

    unsigned int ggxLutTextureID;
    unsigned int charlieLutTextureID;

    unsigned int inputTextureID;
    unsigned int cubemapTextureID;
    unsigned int framebuffer;
    unsigned int mipmapLevels;

    ShaderCache_struct _shaderCache;
    pShaderCache shaderCache;

    iblSampler(void);

    struct t_texture {
        unsigned int internalFormat;
        unsigned int format;
        unsigned int type;
        char* data; };

    struct t_image {
        unsigned int width;
        unsigned int height;
        float* dataFloat;
        unsigned long int dataFloatLength; };

    unsigned int internalFormat(void);
    unsigned int textureTargetType(void);
    t_texture prepareTextureData(t_image* image);
    unsigned int loadTextureHDR(t_image* image);
    unsigned int createCubemapTexture(bool withMipmaps);
    unsigned int createLutTexture( void );
    void doinit(t_image* panoramaImage, const char* env_filename);
    void panoramaToCubeMap(void);
    void applyFilter( unsigned int distribution, float roughness, unsigned int targetMipLevel, unsigned int targetTexture, unsigned int sampleCount, float lodBias, const char* _strProgress, float _baseProgress);
    void cubeMapToLambertian(void );
    void cubeMapToGGX(void );
    void cubeMapToSheen(void );
    void sampleLut(unsigned int distribution, unsigned int targetTexture, unsigned int currentTextureSize);
    void sampleGGXLut( void );
    void sampleCharlieLut( void );
    void filterAll( void (*callback)(const char *, float, float));
    void destroy_iblSampler( void );
};

extern "C" {
#endif


//#ifdef STB_IMAGE_IMPLEMENTATION
//#undef STB_IMAGE_IMPLEMENTATION
//#include "stb_image/stb_image.h"
//#define STB_IMAGE_IMPLEMENTATION
//#else
//#include "stb_image/stb_image.h"
//#endif /* STB_IMAGE_IMPLEMENTATION */

void setIBLLoadPhaseCallback(void (*_load_progress_callback)(const char*, const char* , float, float, float, float));
gl_environment_textures setup_environment(gl_environment_textures* _lastEnv, const char* _env_filename );

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*IBL_SAMPLER_H*/
