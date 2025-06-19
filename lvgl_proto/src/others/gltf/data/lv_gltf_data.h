#ifndef LV_GLTFDATA_H
#define LV_GLTFDATA_H


#ifdef __cplusplus
extern "C" {
#endif

#include "lv_gltf_data.hpp"
#include "sup/include/lv_gltf_data_datatypes.h"
#include "../../../../../lvgl_proto/src/others/opengl_shader_cache/lv_opengl_shader_cache.h"

/**
 * @brief Create a summary of the mesh data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_data_make_mesh_summary(lv_gltf_data_t * data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the material data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_data_make_material_summary(lv_gltf_data_t * data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the scenes in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_data_make_scenes_summary(lv_gltf_data_t * data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the images in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_data_make_images_summary(lv_gltf_data_t * data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the animations in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_data_make_animations_summary(lv_gltf_data_t * data, char *dest_buffer, uint32_t dest_buffer_size);





/**
 * @brief Load the gltf file at the specified filepath
 *
 * @param gltf_path The gltf filename
 * @param ret_data Pointer to the data container that will be populated.
 * @param viewer Pointer to the viewer object this file will be displayed within.
 * @param shaders Pointer to the shader cache object this file uses.
 */
void lv_gltf_data_load(const char * gltf_path, lv_gltf_data_t * ret_data, lv_opengl_shader_cache_t * shaders);

/**
 * @brief Set the callback function that is called when loading increments.
 *
 * @param load_progress_callback The callback function with these parameters (const char*, const char* , float, float, float, float) and no return value.
 */
bool lv_gltf_view_set_loadphase_callback(void (*load_progress_callback)(const char*, const char* , float, float, float, float));

gltf_probe_info * lv_gltf_view_get_probe(lv_gltf_data_t * _data);

double  lv_gltf_data_get_radius(lv_gltf_data_t * D);
float*  lv_gltf_data_get_center(lv_gltf_data_t *  D);
void    lv_gltf_data_destroy(lv_gltf_data_t * _data);
int64_t lv_gltf_data_get_int_radiusX1000(lv_gltf_data_t * _data);
void    lv_gltf_data_copy_bounds_info(lv_gltf_data_t * to, lv_gltf_data_t * from);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTFVIEW_H*/
