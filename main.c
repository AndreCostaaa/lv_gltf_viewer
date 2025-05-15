#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/examples/lv_examples.h"

#include "gltf_loader.h"

#include "gl_renwin.h"
#include "shader_cache.h"
#include "gl_shader_includes.h"
#include "gl_shaderdef_v1.h"

#include <unistd.h>     /* usleep */
#include <signal.h>     /* to trap ctrl-break */
#include <GL/glew.h>    /* For window size restrictions */
#include <GLFW/glfw3.h> /* For window size / title */
//#include <stdio.h>      /* printf */
//#include <math.h>       /* pow */
//#include <stdlib.h>     /* exit */

#include "lvgl/src/drivers/glfw/lv_opengles_debug.h" /* GL_CALL */

//#define SOFTWARE_ONLY

#define BIG_TEXTURE_WIDTH 256 * 3
#define BIG_TEXTURE_HEIGHT 192 * 3

#define WINDOW_WIDTH BIG_TEXTURE_WIDTH
#define WINDOW_HEIGHT BIG_TEXTURE_HEIGHT

#define LVGL_BLUE 0x2196f3
#define LVGL_COOLGRAY 0xe4f1fb
#define LVGL_COOLGRAY_DARKER 0xbbd9f1

bool animate_spin = true;
float yaw_degree_offset = 0.0f;
float pitch_degrees = 0.0f;
float distance = 1.0f;
float spin_rate = 1.0f;
float elevation = 0.0f;

lv_obj_t * grp_loading;
lv_obj_t * spin_checkbox;
lv_obj_t * spin_slider;
lv_glfw_window_t * window;
GLFWwindow * glfw_window;
lv_obj_t * progbar1;
lv_obj_t * progbar2;
lv_obj_t * progText1;

bool setIBLLoadPhaseCallback(void (*_load_progress_callback)(const char*, const char* , float, float, float, float));
gl_environment_textures setup_environment(gl_environment_textures* _lastEnv, const char* _env_filename );
static void load_progress_callback(const char* phase_title, const char* sub_phase_title, float phase_progress, float phase_progress_max, float sub_phase_progress, float sub_phase_progress_max);

static void yaw_slider_event_cb(lv_event_t * e)
{
    if (animate_spin) {
        animate_spin = false;
        lv_obj_remove_state(spin_checkbox, LV_STATE_CHECKED);
        lv_obj_add_flag(spin_slider, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_t * slider = lv_event_get_target_obj(e);
    yaw_degree_offset = ((float)lv_slider_get_value(slider) / 100.0f ) * 360.f;
}

static void pitch_slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    pitch_degrees = (((float)lv_slider_get_value(slider) / 100.0f ) - 0.5f) * 180.f;
}

static void spin_slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    spin_rate = (((float)lv_slider_get_value(slider) / 100.0f ) - 0.5f) * 4.f;
}

static void distance_slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    distance = (1.f - ((float)lv_slider_get_value(slider) / 100.0f )) * 2.0f;
    if (distance < 1.0f) {
        distance = pow(distance, 0.5);
    } else {
        distance = pow(distance, 2.0);
    }
}

static void elev_slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    elevation = (((float)lv_slider_get_value(slider) / 100.0f )-0.5f) * 0.5f;
}

static void spin_checkbox_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    animate_spin = (lv_obj_get_state(spin_checkbox) & LV_STATE_CHECKED);
    if (animate_spin) {
        lv_obj_clear_flag(spin_slider, LV_OBJ_FLAG_HIDDEN);
    } else{
        lv_obj_add_flag(spin_slider, LV_OBJ_FLAG_HIDDEN);
    }
}

static void load_progress_callback(const char* phase_title, const char* sub_phase_title, float phase_progress, float phase_progress_max, float sub_phase_progress, float sub_phase_progress_max)
{
    LV_UNUSED(sub_phase_title);
    LV_UNUSED(sub_phase_progress);
    LV_UNUSED(sub_phase_progress_max);

    lv_label_set_text(progText1, phase_title);
    lv_obj_add_flag(progbar2, LV_OBJ_FLAG_HIDDEN);
    if (phase_progress_max != 0.f) {
        lv_obj_clear_flag(progbar1, LV_OBJ_FLAG_HIDDEN);
        lv_bar_set_value(progbar1, (int)((phase_progress / phase_progress_max) * 100.0f), LV_ANIM_OFF);
    } else {
        lv_obj_add_flag(progbar1, LV_OBJ_FLAG_HIDDEN);
    }
    lv_timer_handler();
    lv_obj_invalidate(grp_loading);
    lv_refr_now(NULL);
    usleep(1000);
}

static void ext_draw_size_event_cb(lv_event_t * e)
{
    lv_coord_t * cur_size = (lv_coord_t*)lv_event_get_param(e);
    *cur_size = LV_MAX(*cur_size, LV_HOR_RES);
}

void lv_loading_info_objects(void)
{
    grp_loading = lv_obj_create(lv_screen_active());
    lv_obj_set_size(grp_loading, 320, 60);

    lv_obj_center(grp_loading);
    lv_obj_align(grp_loading, LV_ALIGN_CENTER, 0, -15);
    lv_obj_set_style_bg_color(grp_loading, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(grp_loading, LV_OPA_80, LV_PART_MAIN);
    lv_obj_set_style_radius(grp_loading, 100, LV_PART_MAIN);
    lv_obj_set_style_border_width(grp_loading, 0, LV_PART_MAIN);
    lv_obj_clear_flag(grp_loading, LV_OBJ_FLAG_CLICKABLE  );
    lv_obj_add_flag(grp_loading, LV_OBJ_FLAG_OVERFLOW_VISIBLE );
    lv_obj_clear_flag(grp_loading, LV_OBJ_FLAG_SCROLLABLE );
    lv_obj_add_event_cb(grp_loading, ext_draw_size_event_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL);

    lv_obj_t * loading_bg = lv_obj_create(grp_loading);
    lv_obj_set_size(loading_bg, 320, 60);
    lv_obj_center(loading_bg);
    lv_obj_align(loading_bg, LV_ALIGN_CENTER, -1, -2);
    lv_obj_set_style_bg_color(loading_bg, lv_color_hex(0xd0dce6), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(loading_bg, LV_OPA_100, LV_PART_MAIN);
    lv_obj_set_style_radius(loading_bg, 100, LV_PART_MAIN);
    lv_obj_set_style_border_color(loading_bg, lv_color_hex(LVGL_BLUE), LV_PART_MAIN);
    lv_obj_set_style_border_width(loading_bg, 2, LV_PART_MAIN);
    lv_obj_clear_flag(loading_bg, LV_OBJ_FLAG_CLICKABLE  );
    lv_obj_set_style_bg_grad_color(loading_bg, lv_color_hex(LVGL_BLUE), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(loading_bg, LV_GRAD_DIR_VER, LV_PART_MAIN);

    lv_obj_clear_flag(loading_bg, LV_OBJ_FLAG_SCROLLABLE );

    lv_obj_t * spinner = lv_spinner_create(loading_bg);
    lv_spinner_set_anim_params(spinner, 4000.0, 30.0f);
    lv_obj_align(spinner, LV_ALIGN_LEFT_MID, -15, 0);
    lv_obj_set_size(spinner, 46, 46);
    lv_obj_set_style_radius(spinner, 2, LV_PART_ANY  );
    lv_obj_add_flag(spinner, LV_OBJ_FLAG_HIDDEN);
    
    progbar1 = lv_bar_create(loading_bg);
    lv_obj_set_size(progbar1, 240, 8);
    lv_obj_align(progbar1, LV_ALIGN_BOTTOM_RIGHT, -20, 8);
    lv_obj_set_style_bg_color(progbar1, lv_color_hex(0xd0dce6), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(progbar1, lv_color_hex(0x000000), LV_PART_MAIN);

    progbar2 = lv_bar_create(loading_bg);
    lv_obj_set_size(progbar2, 240, 4);
    lv_obj_align(progbar2, LV_ALIGN_BOTTOM_RIGHT, -20, 14);
    lv_obj_set_style_bg_color(progbar2, lv_color_hex(0xd0dce6), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(progbar2, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_add_flag(progbar2, LV_OBJ_FLAG_HIDDEN);
    
    progText1 = lv_label_create(loading_bg);
    lv_obj_align(progText1, LV_ALIGN_TOP_LEFT, 20, -10);
    lv_obj_set_style_text_color(progText1, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_text(progText1, "Loading...");
}

void lv_pitch_yaw_distance_sliders(void)
{
    {
        // Yaw (bottom)
        lv_obj_t * slider = lv_slider_create(lv_screen_active());
        lv_obj_set_size(slider, WINDOW_WIDTH - 140, 30);
        lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -15);
        lv_obj_add_event_cb(slider, yaw_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(slider, 2000, 0);
        lv_obj_set_style_bg_opa(slider, LV_OPA_0, LV_PART_INDICATOR );
        lv_obj_set_style_bg_color(slider, lv_color_hex(0xFFFFFF), LV_PART_MAIN );
        lv_obj_set_style_bg_opa(slider, LV_OPA_40, LV_PART_MAIN );
        lv_obj_set_style_border_color(slider, lv_color_hex(LVGL_BLUE), LV_PART_MAIN );
        lv_obj_set_style_border_opa(slider, LV_OPA_40, LV_PART_MAIN );
        lv_obj_set_style_border_width(slider, 2, LV_PART_MAIN );
        lv_slider_set_value(slider, 50.f, 0);
        lv_obj_set_style_border_color(slider, lv_color_hex(LVGL_BLUE), LV_PART_KNOB );
        lv_obj_set_style_border_width(slider, 2, LV_PART_KNOB );
        lv_obj_set_style_bg_color(slider, lv_color_hex(LVGL_COOLGRAY), LV_PART_KNOB);
        lv_obj_set_style_bg_grad_color(slider, lv_color_hex(LVGL_BLUE), LV_PART_KNOB);
        lv_obj_set_style_bg_grad_dir(slider, LV_GRAD_DIR_VER, LV_PART_KNOB);
    }
    {
        // Pitch (lower right)
        lv_obj_t * slider = lv_slider_create(lv_screen_active());
        lv_obj_set_size(slider, 30, ((WINDOW_HEIGHT - 90) / 2));
        lv_obj_align(slider, LV_ALIGN_BOTTOM_RIGHT, -15, -70);
        lv_obj_add_event_cb(slider, pitch_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(slider, 2000, 0);
        lv_obj_set_style_bg_opa(slider, LV_OPA_0, LV_PART_INDICATOR );
        lv_obj_set_style_bg_color(slider, lv_color_hex(0xFFFFFF), LV_PART_MAIN );
        lv_obj_set_style_bg_opa(slider, LV_OPA_40, LV_PART_MAIN );
        lv_obj_set_style_border_color(slider, lv_color_hex(LVGL_BLUE), LV_PART_MAIN );
        lv_obj_set_style_border_opa(slider, LV_OPA_40, LV_PART_MAIN );
        lv_obj_set_style_border_width(slider, 2, LV_PART_MAIN );
        lv_slider_set_value(slider, 50.f, 0);
        lv_obj_set_style_border_color(slider, lv_color_hex(LVGL_BLUE), LV_PART_KNOB );
        lv_obj_set_style_border_width(slider, 2, LV_PART_KNOB );
        lv_obj_set_style_bg_color(slider, lv_color_hex(LVGL_COOLGRAY), LV_PART_KNOB);
        lv_obj_set_style_bg_grad_color(slider, lv_color_hex(LVGL_BLUE), LV_PART_KNOB);
        lv_obj_set_style_bg_grad_dir(slider, LV_GRAD_DIR_VER, LV_PART_KNOB);
    }
    {
        // Distance (upper right)
        lv_obj_t * slider = lv_slider_create(lv_screen_active());
        lv_obj_set_size(slider, 30, ((WINDOW_HEIGHT - 90) / 2)-20);
        lv_obj_align(slider, LV_ALIGN_TOP_RIGHT, -15, 30);
        lv_obj_add_event_cb(slider, distance_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(slider, 2000, 0);
        lv_obj_set_style_bg_opa(slider, LV_OPA_40, LV_PART_INDICATOR );
        lv_obj_set_style_bg_opa(slider, LV_OPA_40, LV_PART_MAIN );
        lv_obj_set_style_width(slider, 10, LV_PART_INDICATOR );
        lv_obj_set_style_bg_color(slider, lv_color_hex(0xFFFFFF), LV_PART_MAIN );
        lv_obj_set_style_border_color(slider, lv_color_hex(LVGL_BLUE), LV_PART_MAIN );
        lv_obj_set_style_border_opa(slider, LV_OPA_40, LV_PART_MAIN );
        lv_obj_set_style_border_width(slider, 2, LV_PART_MAIN );
        lv_slider_set_value(slider, 50.f, 0);
        lv_obj_set_style_border_color(slider, lv_color_hex(LVGL_BLUE), LV_PART_KNOB );
        lv_obj_set_style_border_width(slider, 2, LV_PART_KNOB );
        lv_obj_set_style_bg_color(slider, lv_color_hex(LVGL_COOLGRAY), LV_PART_KNOB);
        lv_obj_set_style_bg_grad_color(slider, lv_color_hex(LVGL_BLUE), LV_PART_KNOB);
        lv_obj_set_style_bg_grad_dir(slider, LV_GRAD_DIR_VER, LV_PART_KNOB);
    }
    {
        // Spin enable/disable (upper right)
        spin_checkbox = lv_checkbox_create(lv_screen_active());
        lv_obj_set_size(spin_checkbox, 65, 40);
        lv_obj_align(spin_checkbox, LV_ALIGN_TOP_RIGHT, -62, 5);
        lv_obj_add_event_cb(spin_checkbox, spin_checkbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_set_style_anim_duration(spin_checkbox, 2000, 0);
        lv_obj_add_state(spin_checkbox, LV_STATE_CHECKED);
        lv_checkbox_set_text(spin_checkbox, "Spin");
        
        lv_obj_set_style_text_opa(spin_checkbox, LV_OPA_50, LV_PART_MAIN);
        lv_obj_set_style_text_color(spin_checkbox, lv_color_hex(LVGL_BLUE), LV_PART_MAIN);
    }    
    {
        // Spin rate (upper right)
        spin_slider = lv_slider_create(lv_screen_active());
        lv_obj_set_size(spin_slider, 200, 10);
        lv_obj_align(spin_slider, LV_ALIGN_TOP_RIGHT, -140, 10);
        lv_obj_add_event_cb(spin_slider, spin_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(spin_slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(spin_slider, 2000, 0);
        lv_obj_set_style_bg_opa(spin_slider, LV_OPA_0, LV_PART_INDICATOR );
        lv_slider_set_value(spin_slider, 75.f, 0);
    }
    {
        // Elevation nudge (lower left)
        lv_obj_t * elev_slider = lv_slider_create(lv_screen_active());
        lv_obj_set_size(elev_slider, 10, 200);
        lv_obj_align(elev_slider, LV_ALIGN_BOTTOM_LEFT, -3, -70);
        lv_obj_add_event_cb(elev_slider, elev_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(elev_slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(elev_slider, 2000, 0);
        //lv_obj_set_style_bg_opa(elev_slider, LV_OPA_0, LV_PART_INDICATOR );
        lv_slider_set_value(elev_slider, 50.f, 0);
    }      
}

void lv_background_objects(void) 
{
    lv_obj_t * background = lv_obj_create(lv_screen_active());
    lv_obj_set_size(background, WINDOW_WIDTH+20, WINDOW_HEIGHT+20);
    lv_obj_center(background);
    lv_obj_clear_flag(background, LV_OBJ_FLAG_CLICKABLE  );

    lv_obj_t * inner_background = lv_obj_create(lv_screen_active());
    lv_obj_set_size(inner_background, WINDOW_WIDTH-120, WINDOW_HEIGHT-90);
    lv_obj_center(inner_background);
    lv_obj_align(inner_background, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_style_bg_color(inner_background, lv_color_hex(0xf0faff), LV_PART_MAIN);
    lv_obj_set_style_border_color(inner_background, lv_color_hex(LVGL_BLUE), LV_PART_MAIN);
    lv_obj_set_style_border_width(inner_background, 1, LV_PART_MAIN);
    lv_obj_clear_flag(inner_background, LV_OBJ_FLAG_CLICKABLE  );
    lv_obj_set_style_bg_grad_color(inner_background, lv_color_hex(0xd0dce6), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(inner_background, LV_GRAD_DIR_VER, LV_PART_MAIN);
     
    LV_IMAGE_DECLARE(lvgl_icon_40px);
    lv_obj_t * img1 = lv_image_create(lv_screen_active());
    lv_image_set_src(img1, &lvgl_icon_40px);
    lv_obj_align(img1, LV_ALIGN_TOP_LEFT, 10, 9);
}

void handle_sigint(int sig)
{
    LV_UNUSED(sig);
    printf("\nShutting down app (from ctrl-c)...\n");
    glfwSetWindowShouldClose(glfw_window, GLFW_TRUE);
}

void window_close_callback(GLFWwindow* _window)
{
    LV_UNUSED(_window);
    printf("\nShutting down app (from window close)...\n");
    glfwSetWindowShouldClose(glfw_window, GLFW_TRUE);
}

int main(int argc, char *argv[])
{
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    lv_init();

    // To force software rendering, find the #define SOFTWARE_ONLY line 
    // that's commented out at start of this file and uncomment it
    #ifdef SOFTWARE_ONLY
    setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
    #endif

    /* create a window and initialize OpenGL */
    window = lv_glfw_window_create(WINDOW_WIDTH, WINDOW_HEIGHT, true);
    glfw_window = (GLFWwindow *)lv_glfw_window_get_glfw_window(window);
    glfwSetWindowTitle(glfw_window, "[ LVGL.io ]  glTF File Viewer ");
    glfwSetWindowCloseCallback(glfw_window, window_close_callback);
    glfwSetWindowSizeLimits(glfw_window, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetWindowAttrib(glfw_window, GLFW_RESIZABLE, false);
    glfwSetWindowAspectRatio(glfw_window, WINDOW_WIDTH, WINDOW_HEIGHT);

    /* create a display that flushes to a texture */
    lv_display_t * texture = lv_opengles_texture_create(WINDOW_WIDTH, WINDOW_HEIGHT);
    lv_display_set_default(texture);

    lv_background_objects();

    /* add the texture to the window */
    unsigned int display_texture = lv_opengles_texture_get_texture_id(texture);
    lv_glfw_texture_t * window_texture = lv_glfw_window_add_texture(window, display_texture, WINDOW_WIDTH, WINDOW_HEIGHT);
    //LV_UNUSED(window_texture); // Temporary while refactoring

    //lv_obj_clear_flag(window_texture, LV_OBJ_FLAG_CLICKABLE  );

    lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE );

    lv_obj_t * title = lv_label_create(lv_screen_active());
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 70, 10);
    lv_obj_set_style_text_opa(title, LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, lv_color_hex(LVGL_BLUE), LV_PART_MAIN);
    lv_label_set_text(title, "3D Models");

    char* filename = "gltfs/torusknot.gltf";
    if (argc > 1) {
        filename = argv[1];
        printf("Loading ");
        printf(filename);
        printf("...\n");
        const char* isofilename = getIsolatedFilename(filename);
        lv_label_set_text(title, isofilename);
    }



    
    lv_loading_info_objects();
    setIBLLoadPhaseCallback(load_progress_callback);
    setLoadPhaseCallback(load_progress_callback);
    lv_obj_clear_flag(grp_loading, LV_OBJ_FLAG_HIDDEN);

    lv_timer_handler();
    lv_task_handler();

    ShaderCache_struct _shaderCache = ShaderCache(src_includes, sizeof(src_includes)/sizeof(key_value), src_vertex(), src_frag() );
    pShaderCache shaderCache = &_shaderCache;
    gl_environment_textures _environment = setup_environment(NULL, "hdr/footprint_court.hdr" );
    _shaderCache.lastEnv = &_environment;

    lv_obj_t * tex = lv_3dtexture_create(lv_screen_active());
    lv_obj_set_size(tex, BIG_TEXTURE_WIDTH, BIG_TEXTURE_HEIGHT);   
    lv_obj_add_flag(tex, LV_OBJ_FLAG_HIDDEN);
    lv_obj_align(tex, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_clear_flag(tex, LV_OBJ_FLAG_CLICKABLE  );

    lv_pitch_yaw_distance_sliders();
  
    pGltf_data_t _model_data = lv_malloc(get_gltf_datastruct_datasize() );
    pViewer _viewer = lv_malloc(get_viewer_datasize() );

    load_gltf((const char*)filename, _model_data, _viewer, shaderCache);

    lv_obj_add_flag(grp_loading, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(tex, LV_OBJ_FLAG_HIDDEN);

    lv_3dtexture_id_t gltf_texture;
    float temp_inc = 0.f;
    gl_viewer_desc_t view_desc;
    view_desc.pitch = pitch_degrees;
    view_desc.yaw = temp_inc + yaw_degree_offset;
    view_desc.distance = distance;

    gltf_texture = new_render_gltf_model_to_opengl_texture(view_desc, elevation, shaderCache, _viewer, _model_data, BIG_TEXTURE_WIDTH, BIG_TEXTURE_HEIGHT, lv_color_hex(0xFFFFFF));
    lv_3dtexture_set_src(tex, gltf_texture);

    signal(SIGINT, handle_sigint);

    while(!glfwWindowShouldClose(glfw_window)) {
        uint32_t ms_delay = lv_timer_handler();
        lv_task_handler();
        usleep(ms_delay * 1000);
        if (animate_spin) {
            temp_inc += spin_rate;
        }
        view_desc.pitch = pitch_degrees;
        view_desc.yaw = temp_inc + yaw_degree_offset;
        view_desc.distance = distance;
        gltf_texture = new_render_gltf_model_to_opengl_texture(view_desc, elevation, shaderCache, _viewer, _model_data, BIG_TEXTURE_WIDTH, BIG_TEXTURE_HEIGHT, lv_color_hex(0xFFFFFF));
        lv_3dtexture_set_src(tex, gltf_texture);
        lv_obj_invalidate(tex);
        glfwPollEvents();

    }
    lv_obj_clear_flag(grp_loading, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(tex, LV_OBJ_FLAG_HIDDEN);
    load_progress_callback("Closing Application", "", 0.f, 0.f, 0.f, 0.f);
    usleep(20 * 1000);
    
    lv_obj_invalidate(grp_loading);
    lv_refr_now(NULL);
    lv_timer_handler();
    lv_task_handler();
    
    DestroyDataStructs(_viewer, _model_data, shaderCache);
    lv_free(_viewer);
    exit(0);
    //return 0;
}
