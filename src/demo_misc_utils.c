#include "data/lv_gltf_bind.h"
#include "demo.h"
#include "lib/lv_gltf/view/sup/include/shader_includes.h"
#include <drivers/glfw/lv_opengles_debug.h>
#include "lib/lv_gltf/view/sup/include/shader_v1.h"

lv_gl_shader_manager_env_textures_t _environment;
static uint32_t cached_clear_tex = 0;

double d_min(double a, double b)
{
    return (a < b) ? a : b;
}
uint32_t ui_max(uint32_t a, uint32_t b)
{
    return (a > b) ? a : b;
}
float lerp_towards(float start, float end, float t, float min_change)
{
    float newval = start + (end - start) * t;
    if(start < end) {
        if((end - start) < min_change) return end;
        if((newval - start) < min_change) {
            newval = start + min_change;
        }
        return newval;
    }
    else if(start > end) {
        if((start - end) < min_change) return end;
        if((start - newval) < min_change) {
            newval = start - min_change;
        }
        return newval;
    }
    else {
        return end;
    }
}

void setup_shadercache(const char * hdr_filepath, int degrees_x10)
{
    shader_cache = lv_gl_shader_manager_create(src_includes, sizeof(src_includes) / sizeof(*src_includes),
                                                  src_vertex(), src_frag());
    lv_timer_handler();
    lv_task_handler();
    lv_refr_now(NULL);
    _environment = lv_gltf_view_ibl_sampler_setup(NULL, hdr_filepath, degrees_x10);
    shader_cache->last_env = &_environment;
}

void demo_set_overrides(void)
{
    if(needs_system_gltfdata) ov_cursor = lv_gltf_bind_add_by_path(system_gltfdata, "/cursor", LV_GLTF_BIND_PROP_POSITION,
                                                                               LV_GLTF_BIND_CHANNEL_1 | LV_GLTF_BIND_CHANNEL_2  | LV_GLTF_BIND_CHANNEL_3, LV_GLTF_BIND_DIR_WRITE);
    if((ov_cursor != NULL)) demo_ui_add_override_controls(tab_pages[TAB_VIEW]);
}

void demo_refocus(lv_gltf_view_t * gltfview, bool first_call)
{
    demo_ui_fill_in_InfoTab(demo_gltfdata);
    lv_gltf_view_set_camera(gltfview, use_scenecam ? camera : -1);
    lv_gltf_view_reset_between_models(gltfview);
    lv_gltf_view_recenter_view_on_model(demo_gltfview, demo_gltfdata);
    lv_gltf_view_set_anim(gltfview, anim_enabled ? anim : -1);

    if(!first_call) {
        lv_gltf_view_set_distance(gltfview, 1000);
        lv_gltf_view_set_yaw(gltfview, 4200);
        lv_gltf_view_set_pitch(gltfview, -2000);
    }

    goal_pitch = lv_gltf_view_get_pitch(gltfview);
    goal_yaw = lv_gltf_view_get_yaw(gltfview);
    goal_distance = lv_gltf_view_get_distance(gltfview);
    goal_focal_x = lv_gltf_view_get_focal_x(gltfview);
    goal_focal_y = lv_gltf_view_get_focal_y(gltfview);
    goal_focal_z = lv_gltf_view_get_focal_z(gltfview);
    spin_counter_degrees = 0.f;
    if(enable_intro_zoom) lv_gltf_view_set_distance(gltfview, (int)((goal_distance * 1.25f + 0.1f) * 1000.f));
    demo_ui_apply_camera_button_visibility(demo_gltfdata);
    demo_ui_apply_anim_button_visibility(demo_gltfdata);
    demo_ui_apply_spin_rate_value(spin_rate);
    demo_ui_apply_spin_enabled_value(animate_spin);
    demo_ui_reposition_all();
    demo_set_overrides();
}

uint32_t demo_make_small_clear_texture(void)
{
    if(cached_clear_tex > 0) return cached_clear_tex;
    GL_CALL(glCreateTextures(GL_TEXTURE_2D, 1, &cached_clear_tex));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, cached_clear_tex));
    unsigned char clearBytes[4] = {255, 0, 255, 0}; // RGBA format
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0,  GL_RGBA, GL_UNSIGNED_BYTE,  clearBytes));
    // Set texture parameters (optional but recommended)
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    return cached_clear_tex;
}
