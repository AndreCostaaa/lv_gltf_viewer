#ifndef LV_GLTFVIEW_H
#define LV_GLTFVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "__include/datatypes.h"
#include "__include/ibl_sampler.h"
#include "__include/shader_includes.h"
#include "__include/shader_v1.h"

////////////////////////////////////////////////////////////////////////////////////////
typedef struct ShaderCache_struct ShaderCache_struct, *pShaderCache;
typedef struct Program_struct Program_struct, *pProgram;
////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Create a summary of the mesh data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_make_mesh_summary(pGltf_data_t data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the material data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_make_material_summary(pGltf_data_t data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the scenes in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_make_scenes_summary(pGltf_data_t data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the images in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_make_images_summary(pGltf_data_t data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the animations in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_make_animations_summary(pGltf_data_t data, char *dest_buffer, uint32_t dest_buffer_size);





/**
 * @brief Load the gltf file at the specified filepath
 *
 * @param gltf_path The gltf filename
 * @param ret_data Pointer to the data container that will be populated.
 * @param viewer Pointer to the viewer object this file will be displayed within.
 * @param shaders Pointer to the shader cache object this file uses.
 */
void lv_gltfview_load(const char * gltf_path, pGltf_data_t ret_data, pViewer viewer, pShaderCache shaders);

/**
 * @brief Set the callback function that is called when loading increments.
 *
 * @param load_progress_callback The callback function with these parameters (const char*, const char* , float, float, float, float) and no return value.
 */
bool lv_gltfview_set_loadphase_callback(void (*load_progress_callback)(const char*, const char* , float, float, float, float));







/**
 * @brief Get the isolated filename from a full path.
 *
 * @param filename The full filename.
 * @param out_buffer Buffer to store the isolated filename.
 * @param max_out_length Maximum length of the output buffer.
 */
void lv_gltf_get_isolated_filename(const char* filename, char* out_buffer, uint32_t max_out_length);

/**
 * @brief Raycast to find the ground position based on mouse coordinates.
 *
 * @param viewer Pointer to the viewer.
 * @param mouse_x The X coordinate of the mouse.
 * @param mouse_y The Y coordinate of the mouse.
 * @param win_width The width of the window.
 * @param win_height The height of the window.
 * @param ground_height The height of the ground.
 * @param out_pos Pointer to store the output position.
 * @return true if the raycast was successful, false otherwise.
 */
bool lv_gltfview_raycast_ground_position(pViewer viewer, int32_t mouse_x, int32_t mouse_y, int32_t win_width, int32_t win_height, double ground_height, float* out_pos);

/**
 * @brief Copy the viewer descriptor from one state to another.
 *
 * @param from_state Pointer to the source viewer descriptor.
 * @param to_state Pointer to the destination viewer descriptor.
 */
void lv_gltf_copy_viewer_desc(gl_viewer_desc_t* from_state, gl_viewer_desc_t* to_state);

/**
 * @brief Compare two viewer descriptors for equality.
 *
 * @param from_state Pointer to the first viewer descriptor.
 * @param to_state Pointer to the second viewer descriptor.
 * @return true if the descriptors are equal, false otherwise.
 */
bool lv_gltf_compare_viewer_desc(gl_viewer_desc_t* from_state, gl_viewer_desc_t* to_state);




gl_viewer_desc_t* lv_gltfview_get_desc           (pViewer V);

unsigned int get_gltf_datastruct_datasize(void);
unsigned int get_viewer_datasize(void);
unsigned int get_primitive_datasize(void);

gl_environment_textures lv_gltfview_ibl_sampler_setup(gl_environment_textures* _lastEnv, const char* _env_filename, int _env_rotation_degreesX10 );
void lv_gltfview_ibl_set_loadphase_callback(void (*_load_progress_callback)(const char*, const char* , float, float, float, float));

uint32_t            lv_gltfview_render( pShaderCache shaders, lv_gltfview_t * view, pGltf_data_t gltf_data );
void                lv_gltfview_destroy(lv_gltfview_t * _viewer, pGltf_data_t _data, pShaderCache _shaders);
gltf_probe_info *   lv_gltfview_get_probe(pGltf_data_t _data);

pOverride lv_gltfview_add_override_by_index(pGltf_data_t _data, uint64_t nodeIndex, OverrideProp whichProp, uint32_t dataMask);
pOverride lv_gltfview_add_override_by_ip(pGltf_data_t _data, const char * nodeIp, OverrideProp whichProp, uint32_t dataMask);
pOverride lv_gltfview_add_override_by_id(pGltf_data_t _data, const char * nodeId, OverrideProp whichProp, uint32_t dataMask);

void lv_gltf_get_isolated_filename(const char* filename, char* out_buffer, uint32_t max_out_length);
bool lv_gltfview_set_loadphase_callback(void (*load_progress_callback)(const char*, const char* , float, float, float, float));
void lv_gltfview_load(const char * gltf_path, pGltf_data_t ret_data, lv_gltfview_t * view, pShaderCache shaders);
int64_t lv_gltf_get_int_radiusX1000 (pGltf_data_t _data);
bool lv_gltfview_raycast_ground_position(lv_gltfview_t * view, int32_t mouse_x, int32_t mouse_y, int32_t win_width, int32_t win_height, double ground_height, float* out_pos);

void init_viewer_struct(lv_gltfview_t * _ViewerMem);

void lv_gltfview_set_width (lv_gltfview_t * view, uint32_t new_width );
void lv_gltfview_set_height (lv_gltfview_t * view, uint32_t new_height );

float lv_gltfview_get_pitch (lv_gltfview_t * view);
float lv_gltfview_get_yaw (lv_gltfview_t * view);
float lv_gltfview_get_distance (lv_gltfview_t * view);
float lv_gltfview_get_focal_x (lv_gltfview_t * view);
float lv_gltfview_get_focal_y (lv_gltfview_t * view);
float lv_gltfview_get_focal_z (lv_gltfview_t * view);
float lv_gltfview_get_spin_degree_offset(lv_gltfview_t * view);
uint32_t lv_gltfview_get_aa_mode(lv_gltfview_t * view);
uint32_t lv_gltfview_get_bg_mode(lv_gltfview_t * view);

void lv_gltfview_set_pitch (lv_gltfview_t * view, int pitch_degrees_x10 );
void lv_gltfview_set_yaw (lv_gltfview_t * view, int yaw_degrees_x10 );
void lv_gltfview_set_distance (lv_gltfview_t * view, int distance_x1000 );
void lv_gltfview_set_focal_x (lv_gltfview_t * view, float focal_x);
void lv_gltfview_set_focal_y (lv_gltfview_t * view, float focal_y);
void lv_gltfview_set_focal_z (lv_gltfview_t * view, float focal_z);
void lv_gltfview_inc_pitch (lv_gltfview_t * view, float pitch_inc_degrees );
void lv_gltfview_inc_yaw (lv_gltfview_t * view, float yaw_inc_degrees );
void lv_gltfview_inc_distance (lv_gltfview_t * view, float distance_inc_units );
void lv_gltfview_inc_focal_x (lv_gltfview_t * view, float focal_x_inc );
void lv_gltfview_inc_focal_y (lv_gltfview_t * view, float focal_y_inc );
void lv_gltfview_inc_focal_z (lv_gltfview_t * view, float focal_z_inc );
void lv_gltfview_inc_spin_degree_offset(lv_gltfview_t * view, float spin_degree_inc );

void lv_gltfview_set_camera (lv_gltfview_t * view, int camera_number );
void lv_gltfview_set_anim(lv_gltfview_t * view, uint32_t anim_num );
void lv_gltfview_set_bg_mode(lv_gltfview_t * view, uint32_t bg_mode );
void lv_gltfview_set_aa_mode(lv_gltfview_t * view, uint32_t aa_mode );
void lv_gltfview_set_blur_bg(lv_gltfview_t * view, float blur_bg_amount );
void lv_gltfview_set_env_pow(lv_gltfview_t * view, float env_pow);
void lv_gltfview_set_exposure(lv_gltfview_t * view, float exposure);
void lv_gltfview_set_spin_degree_offset(lv_gltfview_t * view, float spin_degree_offset );

bool lv_gltfview_check_frame_was_cached(lv_gltfview_t * view);
bool lv_gltfview_check_frame_was_antialiased(lv_gltfview_t * view);

// TO-DO: This should be in gltf_data, not view
void lv_gltfview_set_timestep (lv_gltfview_t * view, float timestep );


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTFVIEW_H*/
