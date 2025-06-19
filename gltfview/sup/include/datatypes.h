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

struct lv_gltf_data_struct;
struct lv_gltf_view_struct;

typedef struct lv_gltf_data_struct lv_gltf_data_t;
typedef struct lv_gltf_view_struct lv_gltf_view_t;

typedef lv_gltf_data_t * pGltf_data_t;
typedef lv_gltf_view_t * pViewer;

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
//typedef gltf_probe_info * pProbeInfo;

typedef struct {
    float pitch;
    float yaw;
    float distance;
    int32_t width;              // The user specified width and height of this output
    int32_t height;
    int32_t render_width;       // If anti-aliasing is not applied this frame, these are the same as width/height, if antialiasing
    int32_t render_height;      // is enabled, these are width/height * antialias upscale power (currently 2.0)
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
// typedef gl_viewer_desc_t * pViewerDesc;

typedef struct {
    OverrideProp prop;
    uint32_t dataMask;
    float data1;
    float data2;
    float data3;
    float data4;
} lv_gltf_override_t;

typedef lv_gltf_override_t * pOverride;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTFDATATYPES_H*/