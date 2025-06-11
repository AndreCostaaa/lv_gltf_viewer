#ifndef LV_GLTFDATATYPES_H
#define LV_GLTFDATATYPES_H


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint32_t _GLENUM;
typedef uint32_t _GLUINT;
typedef int32_t  _GLINT;


#ifdef __cplusplus
#define FVEC2 fastgltf::math::fvec2
#define FVEC3 fastgltf::math::fvec3
#define FVEC4 fastgltf::math::fvec4
#define FMAT3 fastgltf::math::fmat3x3
#define FMAT4 fastgltf::math::fmat4x4
#define ASSET fastgltf::Asset
extern "C" {
#endif
struct lv_gltfdata_struct;
struct lv_gltfview_struct;

typedef struct lv_gltfdata_struct lv_gltfdata_t;
typedef struct lv_gltfview_struct lv_gltfview_t;

typedef lv_gltfdata_t * pGltf_data_t;
typedef lv_gltfview_t * pViewer;

typedef enum  {
    OP_VISIBILITY,
    OP_POSITION,
    OP_ROTATION,
    OP_SCALE,
    OP_BASE_COLOR,
    OP_ALPHA_FACTOR,
    OP_EMIS_COLOR
} OverrideProp;

typedef enum  {
    OMC_CHAN1 = 0x01,
    OMC_CHAN2 = 0x02,
    OMC_CHAN3 = 0x04,
    OMC_CHAN4 = 0x08
} OverrideMaskChannels;

typedef enum {
    ANTIALIAS_OFF=0,
    ANTIALIAS_CONSTANT=1,
    ANTIALIAS_NOT_MOVING=2,
    ANTIALIAS_UNKNOWN=999
} AntialiasingMode;

typedef enum {
    BG_CLEAR=0,
    BG_SOLID=1,
    BG_ENVIRONMENT=2,
    BG_UNKNOWN=999
} BackgroundMode;

typedef struct {
    OverrideProp prop;
    uint32_t dataMask;
    float data1;
    float data2;
    float data3;
    float data4;
} _Override;

typedef _Override * pOverride;

typedef struct {
    bool overrideBaseColor;
    uint32_t materialVariant;
    uint64_t sceneIndex;
} _ViewerOpts;

typedef struct {
    uint64_t opaqueRenderTexture;
    uint64_t opaqueFramebuffer;
    uint64_t opaqueFramebufferScratch;
    uint64_t opaqueDepthTexture;
    uint64_t opaqueFramebufferWidth;
    uint64_t opaqueFramebufferHeight;
    uint64_t windowHeight;
    uint64_t windowWidth;
    float fWindowHeight;
    float fWindowWidth;
    uint64_t vertex_count;
} _ViewerMetrics;

typedef struct {
    bool init_success;
    uint32_t texture;
    uint32_t renderbuffer;
    unsigned framebuffer;
} gl_renwin_state_t;

typedef struct  {
    _ViewerOpts     options;
    _ViewerMetrics  metrics;
    gl_renwin_state_t render_state;
    gl_renwin_state_t opaque_render_state;
    bool render_state_ready;
    bool renderOpaqueBuffer;
    bool load_success;
} _ViewerState;

typedef struct {
    bool ready;
    uint32_t program;
    uint32_t bg_program;
    uint32_t vert;
    uint32_t frag;
} gl_renwin_shaderset_t;

typedef struct {
    float pitch;
    float yaw;
    float distance;
    int32_t width;  // The user specified width and height of this output
    int32_t height;
    int32_t render_width;  // If anti-aliasing is not applied this frame, these are the same as width/height, if antialiasing
    int32_t render_height; // is enabled, these are width/height * antialias upscale power (currently 2.0)
    bool recenter_flag;
    float spin_degree_offset;   // This amount is added to the yaw and can be incremented overtime for a spinning platter effect

    float focal_x;
    float focal_y;
    float focal_z;
    bool frame_was_cached;
    bool frame_was_antialiased;
    bool dirty;
    int32_t camera;             // -1 for default (first scene camera if available or platter if not), 0 = platter, 1+ = Camera index in the order it appeared within the current scene render.  Any value higher than the scene's camera count will be limited to the scene's camera count.
    int32_t anim;               // -1 for no animations, 0+ = Animation index.  Any value higher than the scene's animation count will be limited to the scene's animation count.
    float timestep;             // How far to step the current animation in seconds
    int32_t error_frames;       // temporary counter of how many times the texture failed to update in the past second
    int32_t aa_mode;            // The anti-aliasing mode: 0 = None, 1 = Always, 2 = When Moving or Animated
    int32_t bg_mode;            // The background mode: 0 = Clear, 1 = Solid Color, 2 = The Environment
    float blur_bg;              // How much to blur the environment background, between 0.0 and 1.0
    float env_pow;              // Environmental brightness, 1.0 default
    float exposure;             // Image exposure level, 1.0 default
} gl_viewer_desc_t;

typedef struct {
    uint32_t imageCount;
    uint32_t textureCount;
    uint32_t materialCount;
    uint32_t cameraCount;
    uint32_t nodeCount;
    uint32_t meshCount;
    uint32_t sceneCount;
    uint32_t animationCount;

} gltf_probe_info;

typedef gltf_probe_info * pProbeInfo;

typedef struct {
    _GLINT camera;
    _GLINT viewProjectionMatrixUniform;
    _GLINT modelMatrixUniform;
    _GLINT viewMatrixUniform;
    _GLINT projectionMatrixUniform;

    _GLINT envIntensity;
    _GLINT envDiffuseSampler;
    _GLINT envSpecularSampler;
    _GLINT envSheenSampler;
    _GLINT envGgxLutSampler;
    _GLINT envCharlieLutSampler;
    _GLINT envMipCount;
    
    _GLINT exposure;
    _GLINT roughnessFactor;

    _GLINT baseColorFactor;
    _GLINT baseColorSampler;
    _GLINT baseColorUVSet;
    _GLINT baseColorUVTransform;

    _GLINT emissiveFactor;
    _GLINT emissiveSampler;
    _GLINT emissiveUVSet;
    _GLINT emissiveUVTransform;
    _GLINT emissiveStrength;

    _GLINT metallicFactor;
    _GLINT metallicRoughnessSampler;
    _GLINT metallicRoughnessUVSet;
    _GLINT metallicRoughnessUVTransform;

    _GLINT occlusionStrength;
    _GLINT occlusionSampler;
    _GLINT occlusionUVSet;
    _GLINT occlusionUVTransform;

    _GLINT normalScale;
    _GLINT normalSampler;
    _GLINT normalUVSet;
    _GLINT normalUVTransform;

    _GLINT clearcoatFactor;
    _GLINT clearcoatRoughnessFactor;
    _GLINT clearcoatSampler;
    _GLINT clearcoatUVSet;
    _GLINT clearcoatUVTransform;
    _GLINT clearcoatRoughnessSampler;
    _GLINT clearcoatRoughnessUVSet;
    _GLINT clearcoatRoughnessUVTransform;
    _GLINT clearcoatNormalScale;
    _GLINT clearcoatNormalSampler;    
    _GLINT clearcoatNormalUVSet;
    _GLINT clearcoatNormalUVTransform;

    _GLINT thickness;
    _GLINT thicknessSampler;
    _GLINT thicknessUVSet;
    _GLINT thicknessUVTransform;

    _GLINT diffuseTransmissionSampler;
    _GLINT diffuseTransmissionUVSet;
    _GLINT diffuseTransmissionUVTransform;    

    _GLINT diffuseTransmissionColorSampler;
    _GLINT diffuseTransmissionColorUVSet;
    _GLINT diffuseTransmissionColorUVTransform;

    _GLINT sheenColorFactor;
    _GLINT sheenRoughnessFactor;

    _GLINT specularColorFactor;
    _GLINT specularFactor;

    _GLINT diffuseTransmissionColorFactor;
    _GLINT diffuseTransmissionFactor;

    _GLINT ior;
    _GLINT alphaCutoff;

    _GLINT dispersion;
    _GLINT screenSize;
    _GLINT transmissionFactor;
    //_GLINT transmissionScale;
    _GLINT transmissionSampler;
    _GLINT transmissionUVSet;
    _GLINT transmissionUVTransform;
    _GLINT transmissionFramebufferSampler;
    _GLINT transmissionFramebufferSize;

    _GLINT attenuationDistance;
    _GLINT attenuationColor;
    
    _GLINT jointsSampler;

    _GLINT diffuseFactor;
    //_GLINT specularFactor;
    _GLINT glossinessFactor;

    _GLINT diffuseSampler;
    _GLINT diffuseUVSet;
    _GLINT diffuseUVTransform;
    _GLINT specularGlossinessSampler;
    _GLINT specularGlossinessUVSet;
    _GLINT specularGlossinessUVTransform;
    
} UniformLocs;

typedef struct {
    _GLUINT count;
    _GLUINT instanceCount;
    _GLUINT firstIndex;
    _GLINT baseVertex;
    _GLUINT baseInstance;
} IndirectDrawCommand;

typedef struct {
    IndirectDrawCommand draw;
    _GLENUM primitiveType;
    _GLENUM indexType;
    _GLUINT vertexArray;

    _GLUINT vertexBuffer;
    _GLUINT indexBuffer;

    _GLUINT materialUniformsIndex;
    _GLUINT albedoTexture;
    _GLUINT emissiveTexture;
    _GLUINT metalRoughTexture;
    _GLUINT occlusionTexture;
    _GLUINT normalTexture;
    _GLUINT diffuseTransmissionTexture;
    _GLUINT diffuseTransmissionColorTexture;
    _GLUINT transmissionTexture;
    _GLUINT transmissionTexcoordIndex;
    
    _GLINT baseColorTexcoordIndex;
    _GLINT emissiveTexcoordIndex;
    
    _GLINT metallicRoughnessTexcoordIndex;
    _GLINT occlusionTexcoordIndex;
    _GLINT normalTexcoordIndex;
    _GLINT diffuseTransmissionTexcoordIndex;
    _GLINT diffuseTransmissionColorTexcoordIndex;

    _GLINT clearcoatTexture;
    _GLINT clearcoatRoughnessTexture;
    _GLINT clearcoatNormalTexture;
    _GLINT clearcoatTexcoordIndex;
    _GLINT clearcoatRoughnessTexcoordIndex;
    _GLINT clearcoatNormalTexcoordIndex;

    _GLUINT thicknessTexture;
    _GLINT thicknessTexcoordIndex;

    _GLUINT diffuseTexture;
    _GLINT diffuseTexcoordIndex;

    _GLUINT specularGlossinessTexture;
    _GLINT specularGlossinessTexcoordIndex;

} Primitive;

typedef struct {
    _GLUINT texture;
} Texture;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTFDATATYPES_H*/