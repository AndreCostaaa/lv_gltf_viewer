#ifndef LV_GLTFVIEW_H
#define LV_GLTFVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sup/include/datatypes.h"
#include "sup/include/ibl_sampler.h"
#include "sup/include/shader_includes.h"
#include "sup/include/shader_v1.h"

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
void lv_gltfdata_make_mesh_summary(lv_gltfdata_t * data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the material data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltfdata_make_material_summary(lv_gltfdata_t * data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the scenes in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltfdata_make_scenes_summary(lv_gltfdata_t * data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the images in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltfdata_make_images_summary(lv_gltfdata_t * data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the animations in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltfdata_make_animations_summary(lv_gltfdata_t * data, char *dest_buffer, uint32_t dest_buffer_size);





/**
 * @brief Load the gltf file at the specified filepath
 *
 * @param gltf_path The gltf filename
 * @param ret_data Pointer to the data container that will be populated.
 * @param viewer Pointer to the viewer object this file will be displayed within.
 * @param shaders Pointer to the shader cache object this file uses.
 */
void lv_gltfview_load(const char * gltf_path, lv_gltfdata_t * ret_data, pShaderCache shaders);

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
bool lv_gltfview_raycast_ground_position(lv_gltfview_t * viewer, int32_t mouse_x, int32_t mouse_y, int32_t win_width, int32_t win_height, double ground_height, float* out_pos);

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

void lv_gltfview_utils_save_texture_to_png( lv_gltfview_t * viewer, uint32_t tex_id, const char * filename, bool alpha_enabled, uint32_t compression_level, uint32_t mipmapnum, uint32_t width, uint32_t height );

void lv_gltfview_utils_save_png( lv_gltfview_t * viewer, const char * filename, bool alpha_enabled, uint32_t compression_level );
void lv_gltfview_utils_save_pixelbuffer_to_png( lv_gltfview_t * viewer,  char * pixels, const char * filename, bool alpha_enabled, uint32_t compression_level, uint32_t width, uint32_t height );
void lv_gltfview_utils_get_capture_buffer( char * pixels, lv_gltfview_t * viewer, uint32_t tex_id, bool alpha_enabled, uint32_t mipmapnum, uint32_t width, uint32_t height );

gl_viewer_desc_t* lv_gltfview_get_desc           (lv_gltfview_t * V);

unsigned int get_gltf_datastruct_datasize(void);
unsigned int get_viewer_datasize(void);
unsigned int get_primitive_datasize(void);

gl_environment_textures lv_gltfview_ibl_sampler_setup(gl_environment_textures* _lastEnv, const char* _env_filename, int _env_rotation_degreesX10 );
void lv_gltfview_ibl_set_loadphase_callback(void (*_load_progress_callback)(const char*, const char* , float, float, float, float));

uint32_t            lv_gltfview_render( pShaderCache shaders, lv_gltfview_t * view, lv_gltfdata_t * gltf_data, bool prepare_bg, uint32_t crop_left,  uint32_t crop_right,  uint32_t crop_top,  uint32_t crop_bottom );
//uint32_t            lv_gltfview_render( pShaderCache shaders, lv_gltfview_t * view, lv_gltfdata_t * gltf_data, bool prepare_bg );
void                lv_gltfdata_destroy(lv_gltfdata_t * _data);
void                lv_gltfview_destroy(lv_gltfview_t * _viewer);
void                lv_gltfview_shadercache_destroy(pShaderCache _shaders);
gltf_probe_info *   lv_gltfview_get_probe(lv_gltfdata_t * _data);
void lv_gltfdata_copy_bounds_info(lv_gltfdata_t * to, lv_gltfdata_t * from);
void lv_gltfdata_link_view_to( lv_gltfdata_t * link_target,  lv_gltfdata_t * link_source);

pOverride lv_gltfview_add_override_by_index(lv_gltfdata_t * _data, uint64_t nodeIndex, OverrideProp whichProp, uint32_t dataMask);
pOverride lv_gltfview_add_override_by_ip(lv_gltfdata_t * _data, const char * nodeIp, OverrideProp whichProp, uint32_t dataMask);
pOverride lv_gltfview_add_override_by_id(lv_gltfdata_t * _data, const char * nodeId, OverrideProp whichProp, uint32_t dataMask);

void lv_gltf_get_isolated_filename(const char* filename, char* out_buffer, uint32_t max_out_length);
bool lv_gltfview_set_loadphase_callback(void (*load_progress_callback)(const char*, const char* , float, float, float, float));
int64_t lv_gltf_get_int_radiusX1000 (lv_gltfdata_t * _data);
bool lv_gltfview_raycast_ground_position(lv_gltfview_t * view, int32_t mouse_x, int32_t mouse_y, int32_t win_width, int32_t win_height, double ground_height, float* out_pos);

void init_viewer_struct(lv_gltfview_t * _ViewerMem);

void lv_gltfview_set_width (lv_gltfview_t * view, uint32_t new_width );
void lv_gltfview_set_height (lv_gltfview_t * view, uint32_t new_height );

void lv_gltfview_mark_dirty(lv_gltfview_t * view);

/**
 * @brief Get the viewing pitch angle (up/down). This is only valid when a scene camera is not enabled.
 *
 * @param view Pointer to the lv_gltfview.
 * @return The pitch angle in degrees.
 */
float lv_gltfview_get_pitch (lv_gltfview_t * view);

/**
 * @brief Get the viewing yaw angle (left/right). This is only valid when a scene camera is not enabled.
 *
 * @param view Pointer to the lv_gltfview.
 * @return The yaw angle in degrees.
 */
float lv_gltfview_get_yaw (lv_gltfview_t * view);

/**
 * @brief Get the viewing distance (in/out). This is only valid when a scene camera is not enabled.
 *
 * @param view Pointer to the lv_gltfview.
 * @return The viewing distance (in model bounding volume units, default 1.0).
 */
float lv_gltfview_get_distance (lv_gltfview_t * view);

/**
 * @brief Get the focal position x component. This is only valid when a scene camera is not enabled.
 *
 * @param view Pointer to the lv_gltfview.
 * @return The focal position x component in scene units.
 */
float lv_gltfview_get_focal_x (lv_gltfview_t * view);

/**
 * @brief Get the focal position y component. This is only valid when a scene camera is not enabled.
 *
 * @param view Pointer to the lv_gltfview.
 * @return The focal position y component in scene units.
 */
float lv_gltfview_get_focal_y (lv_gltfview_t * view);

/**
 * @brief Get the focal position x component. This is only valid when a scene camera is not enabled.
 *
 * @param view Pointer to the lv_gltfview.
 * @return The focal position x component in scene units.
 */
float lv_gltfview_get_focal_z (lv_gltfview_t * view);

/**
 * @brief Get the yaw angle offset (for platter spin). This is only valid when a scene camera is not enabled.
 *
 * @param view Pointer to the lv_gltfview.
 * @return The yaw angle offset in degrees.
 */
float lv_gltfview_get_spin_degree_offset(lv_gltfview_t * view);

/**
 * @brief Get the anti-aliasing mode enum value as unsigned int.  0 = Off, 1 = Always On, 2 = Auto (Off during movement, otherwise on)
 *
 * @param view Pointer to the lv_gltfview.
 * @return The current anti-aliasing mode (defined in AntialiasingMode enum), as unsigned int.
 */
uint32_t lv_gltfview_get_aa_mode(lv_gltfview_t * view);

/**
 * @brief Get the background mode enum value as unsigned int. 0 = Clear, 1 = Solid Color, 2 = Environment
 *
 * @param view Pointer to the lv_gltfview.
 * @return The current background mode (defined in BackgroundMode enum), as unsigned int.
 */
uint32_t lv_gltfview_get_bg_mode(lv_gltfview_t * view);



uint32_t lv_gltfview_get_width (lv_gltfview_t * view );
uint32_t lv_gltfview_get_height (lv_gltfview_t * view );




/**
 * @brief Set the viewing angle pitch in degrees x 10 (3600 per full turn)
 *
 * @param view Pointer to the lv_gltfview.
 * @param pitch_degrees_x10 The view pitch in degrees times ten, as signed integer.
 */
void lv_gltfview_set_pitch (lv_gltfview_t * view, int pitch_degrees_x10 );

/**
 * @brief Set the viewing angle yaw in degrees x 10 (3600 per full turn)
 *
 * @param view Pointer to the lv_gltfview.
 * @param yaw_degrees_x10 The yaw pitch in degrees times ten, as signed integer.
 */
void lv_gltfview_set_yaw (lv_gltfview_t * view, int yaw_degrees_x10 );

/**
 * @brief Set the viewing distance in model bounding volume units x 1000 (1000 per standard distance which is 250% the bounding volume radius)
 *
 * @param view Pointer to the lv_gltfview.
 * @param distance_x1000 The viewing distance in model bounding volume units x 1000 as signed integer (note: negative numbers are valid but will probably not be useful, putting the model out of view).
 */
void lv_gltfview_set_distance (lv_gltfview_t * view, int distance_x1000 );

/**
 * @brief Set the viewing focal position x component
 *
 * @param view Pointer to the lv_gltfview.
 * @param focal_x The viewing position x component as float 
 */
void lv_gltfview_set_focal_x (lv_gltfview_t * view, float focal_x);

/**
 * @brief Set the viewing focal position y component
 *
 * @param view Pointer to the lv_gltfview.
 * @param focal_y The viewing position y component as float 
 */
void lv_gltfview_set_focal_y (lv_gltfview_t * view, float focal_y);

/**
 * @brief Set the viewing focal position z component
 *
 * @param view Pointer to the lv_gltfview.
 * @param focal_z The viewing position z component as float 
 */
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
//void lv_gltfview_set_recenter_flag(lv_gltfview_t * view, bool should_recenter );
//void lv_gltfview_set_recenter_flag(lv_gltfview_t * view, lv_gltfdata_t * gltf_data,  bool should_recenter );
void lv_gltfview_recenter_view_on_model( lv_gltfview_t * viewer, pGltf_data_t gltf_data);
void lv_gltfview_reset_between_models( lv_gltfview_t * viewer );


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTFVIEW_H*/
