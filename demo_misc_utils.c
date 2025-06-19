#include "demo.h"

ShaderCache_struct _shaderCache;
gl_environment_textures _environment;
static uint32_t cached_clear_tex = 0;

double d_min(double a, double b) { return (a < b) ? a : b; }
uint32_t ui_max(uint32_t a, uint32_t b) { return (a > b) ? a : b; }
float lerp(float start, float end, float t) { return start + (end - start) * t; }

void setup_shadercache(const char * hdr_filepath, int degrees_x10 ) {
    _shaderCache = ShaderCache(src_includes, sizeof(src_includes)/sizeof(key_value), src_vertex(), src_frag() );
    shaderCache = &_shaderCache;
    lv_timer_handler();
    lv_task_handler();
    lv_refr_now(NULL);
    _environment = lv_gltfview_ibl_sampler_setup(NULL, hdr_filepath, degrees_x10  );
    _shaderCache.lastEnv = &_environment;
}

void demo_set_overrides(void) {
    ov_boom = lv_gltfview_add_override_by_id(demo_gltfdata,     "/root_base/base_platform/cab_pivot/proximal_armlink", OP_ROTATION, OMC_CHAN2);
    ov_stick = lv_gltfview_add_override_by_id(demo_gltfdata,    "/root_base/base_platform/cab_pivot/proximal_armlink/distal_armlink", OP_ROTATION, OMC_CHAN2);
    ov_bucket = lv_gltfview_add_override_by_id(demo_gltfdata,   "/root_base/base_platform/cab_pivot/proximal_armlink/distal_armlink/bucket", OP_ROTATION, OMC_CHAN2);  // Not currently valid even with the right model loaded
    ov_swing = lv_gltfview_add_override_by_id(demo_gltfdata,    "/root_base/base_platform/cab_pivot", OP_ROTATION, OMC_CHAN1 | OMC_CHAN2 | OMC_CHAN3);
    if (needs_system_gltfdata) ov_cursor = lv_gltfview_add_override_by_id(system_gltfdata, "/cursor", OP_POSITION, OMC_CHAN1 | OMC_CHAN2  | OMC_CHAN3);
    if ((ov_boom != NULL) && (ov_stick != NULL) && (ov_swing != NULL) && (ov_cursor != NULL) ) demo_ui_add_override_controls(tab_pages[TAB_VIEW]);
}

void demo_refocus(lv_gltfview_t * gltfview, bool first_call) {
    demo_ui_fill_in_InfoTab(demo_gltfdata);
    lv_gltfview_set_camera(gltfview, use_scenecam ? camera : -1);
    lv_gltfview_reset_between_models(gltfview);
    lv_gltfview_recenter_view_on_model(demo_gltfview, demo_gltfdata);
    lv_gltfview_set_anim(gltfview, anim_enabled ? anim : -1);

    if (!first_call) {
        lv_gltfview_set_distance(gltfview, 1000);
        lv_gltfview_set_yaw(gltfview, 420 );
        lv_gltfview_set_pitch(gltfview, -200 );
    }

    goal_pitch = lv_gltfview_get_pitch(gltfview);
    goal_yaw = lv_gltfview_get_yaw(gltfview);
    goal_distance = lv_gltfview_get_distance(gltfview);
    goal_focal_x = lv_gltfview_get_focal_x(gltfview);
    goal_focal_y = lv_gltfview_get_focal_y(gltfview);
    goal_focal_z = lv_gltfview_get_focal_z(gltfview);
    spin_counter_degrees = 0.f;
    if (enable_intro_zoom) lv_gltfview_set_distance(gltfview,  (int)((goal_distance*1.25f+0.1f)*1000.f) );
    demo_ui_apply_camera_button_visibility(demo_gltfdata);
    demo_ui_apply_anim_button_visibility(demo_gltfdata);
    demo_ui_apply_spin_rate_value(spin_rate);
    demo_ui_apply_spin_enabled_value(animate_spin);
    demo_ui_reposition_all();
    demo_set_overrides();
}

uint32_t demo_make_small_clear_texture() {
    if (cached_clear_tex > 0) return cached_clear_tex;
    GL_CALL(glCreateTextures(GL_TEXTURE_2D, 1, &cached_clear_tex));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, cached_clear_tex));
    unsigned char clearBytes[4] = {255, 0, 255, 255}; // RGBA format
    GL_CALL(glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0,  GL_RGBA8, GL_UNSIGNED_BYTE,  clearBytes ));
    // Set texture parameters (optional but recommended)
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    return cached_clear_tex;
}
