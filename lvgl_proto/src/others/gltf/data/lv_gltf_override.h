#ifndef LV_GLTFOVERRIDE_H
#define LV_GLTFOVERRIDE_H


#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
#include <string>
#include <iostream>

extern "C" {
#endif
struct lv_gltf_data_struct;
typedef struct lv_gltf_data_struct lv_gltf_data_t;

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

struct lv_gltf_override_struct {
    OverrideProp prop;
    uint32_t dataMask;
    float data1;
    float data2;
    float data3;
    float data4;
} ;

typedef struct lv_gltf_override_struct lv_gltf_override_t;

lv_gltf_override_t * lv_gltf_view_add_override_by_index(lv_gltf_data_t * _data, uint64_t nodeIndex, OverrideProp whichProp, uint32_t dataMask);
lv_gltf_override_t * lv_gltf_view_add_override_by_ip(lv_gltf_data_t * _data, const char * nodeIp, OverrideProp whichProp, uint32_t dataMask);
lv_gltf_override_t * lv_gltf_view_add_override_by_id(lv_gltf_data_t * _data, const char * nodeId, OverrideProp whichProp, uint32_t dataMask);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTFOVERRIDE_H*/
