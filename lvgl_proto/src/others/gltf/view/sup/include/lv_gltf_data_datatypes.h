#ifndef LV_GLTFDATA_DATATYPES_H
#define LV_GLTFDATA_DATATYPES_H

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

struct lv_gltf_data_struct;

typedef struct lv_gltf_data_struct lv_gltf_data_t;

typedef lv_gltf_data_t * pGltf_data_t;

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

#endif /*LV_GLTFDATA_DATATYPES_H*/