#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstddef>

#include "lvgl/lvgl.h"
#include "lvgl/src/drivers/glfw/lv_opengles_debug.h" /* GL_CALL */

#ifdef LVGL_ENABLE_WEBP_IMAGES
    #if LVGL_ENABLE_WEBP_IMAGES
        #if LVGL_ENABLE_WEBP_IMAGES == 1
            #include "deps/webp_min/src/webp/decode.h"
        #endif
        #if LVGL_ENABLE_WEBP_IMAGES == 2
            #include "webp/decode.h"
        #endif
        int32_t WebPGetInfo(const uint8_t* data, size_t data_size, int32_t* width, int32_t* height);
        VP8StatusCode WebPGetFeatures(const uint8_t* data,
                                    size_t data_size,
                                    WebPBitstreamFeatures* features);
    #endif /* LVGL_ENABLE_WEBP_IMAGES == true (or 1)*/
#endif /* LVGL_ENABLE_WEBP_IMAGES */

#define FASTGLTF_ENABLE_DEPRECATED_EXT 1
#undef FASTGLTF_DIFFUSE_TRANSMISSION_SUPPORT    // Talking withe fastgltf devs about getting this in there, should be merged in soon.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-move"
#include "deps/fastgltf/include/fastgltf/core.hpp"
#include "deps/fastgltf/include/fastgltf/types.hpp"
#include "deps/fastgltf/include/fastgltf/tools.hpp"
#ifndef ONE_TIME_FASTGLTF_SRC_INCLUDE
#define ONE_TIME_FASTGLTF_SRC_INCLUDE
#include "../data/deps/simdjson/simdjson.cpp"
#include "../data/deps/fastgltf/src/base64.cpp"
#include "../data/deps/fastgltf/src/fastgltf.cpp"
#include "../data/deps/fastgltf/src/io.cpp"
#endif /* ONE_TIME_FASTGLTF_SRC_INCLUDE */
#pragma GCC diagnostic pop


#ifndef STB_HAS_BEEN_INCLUDED
#define STB_HAS_BEEN_INCLUDED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#define STB_IMAGE_IMPLEMENTATION
#include "../data/deps/stb_image/stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image/stb_image_write.h"
#pragma GCC diagnostic pop
#endif

#include "lv_gltf_data.h"
#include "lv_gltf_data_internal.h"
#include "sup/datatypes.cpp"
#include "sup/injest.cpp"
#include "sup/reports.cpp"
#include "sup/utils.cpp"

#define __GLFW_SAMPLES 0x0002100D

void lv_gltf_data_destroy(pGltf_data_t _data){
    __free_data_struct(_data);
}
