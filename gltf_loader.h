#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

unsigned int render_gltf_model_to_opengl_texture(const char * gltf_path, uint32_t texture_w,
                                                 uint32_t texture_h, lv_color_t color);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*GLTF_LOADER_H*/
