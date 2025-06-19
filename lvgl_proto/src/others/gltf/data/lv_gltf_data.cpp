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

#ifndef STB_HAS_BEEN_INCLUDED
#define STB_HAS_BEEN_INCLUDED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#pragma GCC diagnostic pop
#endif

#include "lv_gltf_data.h"
#include "lv_gltf_data_internal.h"
#include "sup/lv_gltf_data_datatypes.cpp"
#include "sup/injest.cpp"
#include "sup/reports.cpp"

#define __GLFW_SAMPLES 0x0002100D

void lv_gltf_data_destroy(pGltf_data_t _data){
    __free_data_struct(_data);
}