#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lvgl/lvgl.h"
#include "lvgl/src/drivers/glfw/lv_opengles_debug.h" /* GL_CALL */

#include "webp/decode.h"
int32_t WebPGetInfo(const uint8_t* data, size_t data_size, int32_t* width, int32_t* height);
VP8StatusCode WebPGetFeatures(const uint8_t* data,
                              size_t data_size,
                              WebPBitstreamFeatures* features);

#define FASTGLTF_ENABLE_DEPRECATED_EXT 1
#undef FASTGLTF_DIFFUSE_TRANSMISSION_SUPPORT    // Talking withe fastgltf devs about getting this in there, should be merged in soon.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-move"
#include "lib/fastgltf/include/fastgltf/core.hpp"
#include "lib/fastgltf/include/fastgltf/types.hpp"
#include "lib/fastgltf/include/fastgltf/tools.hpp"
#pragma GCC diagnostic pop

//#include "lv_gltf_override.h"
#include "lv_gltf_data.h"
//#include "lv_gltf_data_internal.h"

lv_gltf_override_t * lv_gltf_view_add_override_by_index(lv_gltf_data_t * _data, uint64_t nodeIndex, OverrideProp whichProp, uint32_t dataMask){
    return NULL;
}

lv_gltf_override_t * lv_gltf_view_add_override_by_ip(lv_gltf_data_t * _data, const char * nodeIp, OverrideProp whichProp, uint32_t dataMask){
    return NULL;
}

lv_gltf_override_t * lv_gltf_view_add_override_by_id(lv_gltf_data_t * _data, const char * nodeId, OverrideProp whichProp, uint32_t dataMask){
    std::string sNodeId = std::string(nodeId);
    if ((*_data->node_by_path).find(sNodeId) != (*_data->node_by_path).end()) {
        const auto& _node = (*_data->node_by_path)[sNodeId];
        //std::cout << "Found Node within the __node_by_path collection under path:\n" << sNodeId << "\nNode Name is:\n" << _node->name << "\n";
        struct lv_gltf_override_struct _newOverride;// = lv_gltf_override_t();
        _newOverride.prop = whichProp;
        _newOverride.dataMask = dataMask;
        _newOverride.data1 = 0.f;
        _newOverride.data2 = 0.f;
        _newOverride.data3 = 0.f;
        _newOverride.data4 = 0.f;
        (*_data->overrides)[_node] = _newOverride;
        return &((*_data->overrides)[_node]);
    }
    return NULL;
}
