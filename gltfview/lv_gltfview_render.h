#ifndef GL_RENWIN_H
#define GL_RENWIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_gltfview_datatypes.h"
#include "lv_gltfview_shader_cache.h"

uint32_t            lv_gltfview_render( pShaderCache shaders, pViewer viewer, pGltf_data_t gltf_data );
void                lv_gltfview_destroy(pViewer _viewer, pGltf_data_t _data, pShaderCache _shaders);
gltf_probe_info *   lv_gltfview_get_probe(pGltf_data_t _data);

pOverride lv_gltfview_add_override_by_index(pGltf_data_t _data, uint64_t nodeIndex, OverrideProp whichProp, uint32_t dataMask);
pOverride lv_gltfview_add_override_by_ip(pGltf_data_t _data, const char * nodeIp, OverrideProp whichProp, uint32_t dataMask);
pOverride lv_gltfview_add_override_by_id(pGltf_data_t _data, const char * nodeId, OverrideProp whichProp, uint32_t dataMask);

void lv_gltf_get_isolated_filename(const char* filename, char* out_buffer, uint32_t max_out_length);
bool lv_gltfview_set_load_phase_callback(void (*load_progress_callback)(const char*, const char* , float, float, float, float));
void lv_gltfview_load(const char * gltf_path, pGltf_data_t ret_data, pViewer viewer, pShaderCache shaders);
int64_t lv_gltf_get_int_radiusX1000 (pGltf_data_t _data);
bool lv_gltfview_raycast_ground_position(pViewer viewer, int32_t mouse_x, int32_t mouse_y, int32_t win_width, int32_t win_height, double ground_height, float* out_pos);

void init_viewer_struct(pViewer _ViewerMem);

void lv_gltfview_set_width (pViewer viewer, uint32_t new_width );
void lv_gltfview_set_height (pViewer viewer, uint32_t new_height );

float lv_gltfview_get_pitch (pViewer viewer);
float lv_gltfview_get_yaw (pViewer viewer);
float lv_gltfview_get_distance (pViewer viewer);
float lv_gltfview_get_focal_x (pViewer viewer);
float lv_gltfview_get_focal_y (pViewer viewer);
float lv_gltfview_get_focal_z (pViewer viewer);
float lv_gltfview_get_spin_degree_offset(pViewer viewer);
uint32_t lv_gltfview_get_aa_mode(pViewer viewer);
uint32_t lv_gltfview_get_bg_mode(pViewer viewer);

void lv_gltfview_set_pitch (pViewer viewer, int pitch_degrees_x10 );
void lv_gltfview_set_yaw (pViewer viewer, int yaw_degrees_x10 );
void lv_gltfview_set_distance (pViewer viewer, int distance_x1000 );
void lv_gltfview_set_focal_x (pViewer viewer, float focal_x);
void lv_gltfview_set_focal_y (pViewer viewer, float focal_y);
void lv_gltfview_set_focal_z (pViewer viewer, float focal_z);
void lv_gltfview_inc_pitch (pViewer viewer, float pitch_inc_degrees );
void lv_gltfview_inc_yaw (pViewer viewer, float yaw_inc_degrees );
void lv_gltfview_inc_distance (pViewer viewer, float distance_inc_units );
void lv_gltfview_inc_focal_x (pViewer viewer, float focal_x_inc );
void lv_gltfview_inc_focal_y (pViewer viewer, float focal_y_inc );
void lv_gltfview_inc_focal_z (pViewer viewer, float focal_z_inc );
void lv_gltfview_inc_spin_degree_offset(pViewer viewer, float spin_degree_inc );

void lv_gltfview_set_camera (pViewer viewer, int camera_number );
void lv_gltfview_set_anim(pViewer viewer, uint32_t anim_num );
void lv_gltfview_set_bg_mode(pViewer viewer, uint32_t bg_mode );
void lv_gltfview_set_aa_mode(pViewer viewer, uint32_t aa_mode );
void lv_gltfview_set_blur_bg(pViewer viewer, float blur_bg_amount );
void lv_gltfview_set_env_pow(pViewer viewer, float env_pow);
void lv_gltfview_set_exposure(pViewer viewer, float exposure);
void lv_gltfview_set_spin_degree_offset(pViewer viewer, float spin_degree_offset );

bool lv_gltfview_check_frame_was_cached(pViewer viewer);
bool lv_gltfview_check_frame_was_antialiased(pViewer viewer);

// TO-DO: This should be in gltf_data, not viewer
void lv_gltfview_set_timestep (pViewer viewer, float timestep );


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*GL_RENWIN_H*/
