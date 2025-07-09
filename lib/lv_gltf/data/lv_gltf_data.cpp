/**
 * @file lv_gltf_data.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data.h"
#include "misc/lv_assert.h"

#include "lv_gltf_data_internal.hpp"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

size_t lv_gltf_data_get_image_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.images.size();
}
size_t lv_gltf_data_get_texture_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.textures.size();
}
size_t lv_gltf_data_get_material_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.materials.size();
}
size_t lv_gltf_data_get_camera_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.cameras.size();
}
size_t lv_gltf_data_get_node_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.nodes.size();
}
size_t lv_gltf_data_get_mesh_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.meshes.size();
}
size_t lv_gltf_data_get_scene_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.scenes.size();
}
size_t lv_gltf_data_get_animation_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.animations.size();
}

lv_gltf_data_t *
lv_gltf_data_load_from_file(const char * file_path,
                            lv_gl_shader_manager_t * shader_manager)
{
    return lv_gltf_data_load_internal(file_path, 0, shader_manager);
}

lv_gltf_data_t *
lv_gltf_data_load_from_bytes(const uint8_t * data, size_t data_size,
                             lv_gl_shader_manager_t * shader_manager)
{
    return lv_gltf_data_load_internal(data, data_size, shader_manager);
}

fastgltf::Asset * lv_gltf_data_get_asset(lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return &data->asset;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
