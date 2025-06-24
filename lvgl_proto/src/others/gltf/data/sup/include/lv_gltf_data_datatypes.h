#ifndef LV_GLTFDATA_DATATYPES_H
#define LV_GLTFDATA_DATATYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
#define FVEC2 fastgltf::math::fvec2
#define FVEC3 fastgltf::math::fvec3
#define FVEC4 fastgltf::math::fvec4
#define FMAT3 fastgltf::math::fmat3x3
#define FMAT4 fastgltf::math::fmat4x4
#define ASSET fastgltf::Asset
extern "C" {
#endif

struct lv_gltf_data_struct;
typedef struct lv_gltf_data_struct lv_gltf_data_t;
typedef lv_gltf_data_t * pGltf_data_t;

struct lv_gltf_override_struct;
typedef struct lv_gltf_override_struct lv_gltf_override_t;

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


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTFDATA_DATATYPES_H*/
