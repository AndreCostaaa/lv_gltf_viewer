#include "lv_gltf_view.h"
#include "../data/lv_gltf_data_internal.h"
#include "lv_gltf_view_internal.h"

gl_viewer_desc_t* lv_gltf_view_get_desc(lv_gltf_view_t * view);

float absf(float v) { return v > 0 ? v : -v; }

void        lv_gltf_view_mark_dirty(lv_gltf_view_t * view)      {lv_gltf_view_get_desc(view)->dirty = true;}
uint32_t    lv_gltf_view_get_aa_mode(lv_gltf_view_t * view)     { return lv_gltf_view_get_desc(view)->aa_mode; }
uint32_t    lv_gltf_view_get_bg_mode(lv_gltf_view_t * view)     { return lv_gltf_view_get_desc(view)->bg_mode; }
uint32_t    lv_gltf_view_get_width (lv_gltf_view_t * view )     { return lv_gltf_view_get_desc(view)->width; }
uint32_t    lv_gltf_view_get_height (lv_gltf_view_t * view )    { return lv_gltf_view_get_desc(view)->height; }
float       lv_gltf_view_get_pitch (lv_gltf_view_t * view)      { return lv_gltf_view_get_desc(view)->pitch; }
float       lv_gltf_view_get_yaw (lv_gltf_view_t * view)        { return lv_gltf_view_get_desc(view)->yaw; }
float       lv_gltf_view_get_distance (lv_gltf_view_t * view)   { return lv_gltf_view_get_desc(view)->distance; }
float       lv_gltf_view_get_fov (lv_gltf_view_t * view)        { return lv_gltf_view_get_desc(view)->fov; }
float       lv_gltf_view_get_focal_x (lv_gltf_view_t * view)    { return lv_gltf_view_get_desc(view)->focal_x; }
float       lv_gltf_view_get_focal_y (lv_gltf_view_t * view)    { return lv_gltf_view_get_desc(view)->focal_y; }
float       lv_gltf_view_get_focal_z (lv_gltf_view_t * view)    { return lv_gltf_view_get_desc(view)->focal_z; }
float       lv_gltf_view_get_spin_degree_offset(lv_gltf_view_t * view){ return lv_gltf_view_get_desc(view)->spin_degree_offset; }

void lv_gltf_view_set_pitch (lv_gltf_view_t * view, int pitch_degrees_x100 ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    float _newval = pitch_degrees_x100 * 0.01f; 
    if (absf(desc->pitch - _newval) > 0.0001f ) {
        desc->pitch = _newval;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_yaw (lv_gltf_view_t * view, int yaw_degrees_x100 ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    float _newval = yaw_degrees_x100 * 0.01f; 
    if (absf(desc->yaw - _newval) > 0.0001f ) {
        desc->yaw = _newval;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_distance (lv_gltf_view_t * view, int distance_units_x1000 ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    float _newval = distance_units_x1000 * 0.001f; 
    if (absf(desc->distance - _newval) > 0.001f ) {
        desc->distance = _newval;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_focal_x (lv_gltf_view_t * view, float focal_x ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(desc->focal_x - focal_x) > 0.0001f ) {
        desc->focal_x = focal_x;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_focal_y (lv_gltf_view_t * view, float focal_y ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(desc->focal_y - focal_y) > 0.0001f ) {
        desc->focal_y = focal_y;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_focal_z (lv_gltf_view_t * view, float focal_z ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(desc->focal_z - focal_z) > 0.00001f ) {
        desc->focal_z = focal_z;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_fov (lv_gltf_view_t * view, float vertical_fov ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(desc->fov - vertical_fov) > 0.001f ) {
        desc->fov = vertical_fov;
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_pitch (lv_gltf_view_t * view, float pitch_inc_degrees ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(pitch_inc_degrees) > 0.0001f ) {
        desc->pitch += pitch_inc_degrees;
        if (desc->pitch > 89.0f) {
            desc->pitch = 89.0f;
        } else if (desc->pitch < -89.0f) {
            desc->pitch = -89.0f;
        }
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_yaw (lv_gltf_view_t * view, float yaw_inc_degrees ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(yaw_inc_degrees) > 0.0001f ) {
        desc->yaw += yaw_inc_degrees;
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_distance (lv_gltf_view_t * view, float distance_inc_units ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(distance_inc_units) != 0.f ) {
        desc->distance += distance_inc_units;
        if (desc->distance < 0.01f) { desc->distance = 0.01f; }
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_focal_x (lv_gltf_view_t * view, float focal_x_inc ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(focal_x_inc) > 0.0001f ) {
        desc->focal_x += focal_x_inc;
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_focal_y (lv_gltf_view_t * view, float focal_y_inc ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(focal_y_inc) > 0.0001f ) {
        desc->focal_y += focal_y_inc;
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_focal_z (lv_gltf_view_t * view, float focal_z_inc ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(focal_z_inc) > 0.0001f ) {
        desc->focal_z += focal_z_inc;
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_spin_degree_offset(lv_gltf_view_t * view, float spin_degree_inc ){
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(spin_degree_inc) > 0.0001f ) {
        desc->spin_degree_offset += spin_degree_inc;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_camera (lv_gltf_view_t * view, int camera_number ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (desc->camera != camera_number ) {
        desc->camera = camera_number;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_timestep (lv_gltf_view_t * view, float timestep ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    desc->timestep = timestep;
}

void lv_gltf_view_set_width (lv_gltf_view_t * view, uint32_t new_width ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (desc->width  != (int32_t)new_width) {
        desc->width = (int32_t)new_width;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_height (lv_gltf_view_t * view, uint32_t new_height ) {
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (desc->height  != (int32_t)new_height) {
        desc->height = (int32_t)new_height;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_anim(lv_gltf_view_t * view, uint32_t anim_num ){
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (desc->anim  != (int32_t)anim_num) {
        desc->anim = (int32_t)anim_num;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_bg_mode(lv_gltf_view_t * view, uint32_t bg_mode ){
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (desc->bg_mode != (int32_t)bg_mode) {
        desc->bg_mode = (int32_t)bg_mode;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_aa_mode(lv_gltf_view_t * view, uint32_t aa_mode ){
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (desc->aa_mode != (int32_t)aa_mode) {
        desc->aa_mode = (int32_t)aa_mode;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_blur_bg(lv_gltf_view_t * view, float blur_bg_amount ){
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(desc->blur_bg - blur_bg_amount) > 0.0001f ) {
        desc->blur_bg = blur_bg_amount;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_env_pow(lv_gltf_view_t * view, float env_pow ){
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(desc->env_pow - env_pow) > 0.0001f ) {
        desc->env_pow = env_pow;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_exposure(lv_gltf_view_t * view, float exposure ){
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(desc->exposure - exposure) > 0.0001f ) {
        desc->exposure = exposure;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_spin_degree_offset(lv_gltf_view_t * view, float spin_degree_offset ){
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if (absf(desc->spin_degree_offset - spin_degree_offset) > 0.0001f ) {
        desc->spin_degree_offset = spin_degree_offset;
        desc->dirty = true;
    }
}

bool lv_gltf_view_check_frame_was_cached(lv_gltf_view_t * view) {
    return lv_gltf_view_get_desc(view)->frame_was_cached;
}

bool lv_gltf_view_check_frame_was_antialiased(lv_gltf_view_t * view) {
    return lv_gltf_view_get_desc(view)->frame_was_antialiased;
}
