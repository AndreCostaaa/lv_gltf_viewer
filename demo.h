#ifndef MAINUI_SHARED_H
#define MAINUI_SHARED_H
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/examples/lv_examples.h"

#include "gltfview/lv_gltfview.h"
//#include "gltfview/lv_gltfview_datatypes.h"
//#include "gltfview/lv_gltfview_internal_interface.hpp"
//#include "gltfview/lv_gltfview_reports.h"
//#include "gltfview/lv_gltfview_render.h"
//#include "gltfview/lv_gltfview_shader_cache.h"
//#include "gltfview/lv_gltfview_shader_includes.h"
//#include "gltfview/lv_gltfview_shader_v1.h"

#include <unistd.h>     /* usleep */
#include <GL/glew.h>    /* For window size restrictions */
//#define GLFW_INCLUDE_ES31 
#include <GLFW/glfw3.h> /* For window size / title */

#include <stdio.h>      /* printf */
#include <string.h>
#include <stdlib.h>     /* exit */
#include <time.h>       /* for animation timing */
#include <sys/time.h>
#include "lvgl/src/drivers/glfw/lv_opengles_debug.h" /* GL_CALL */

#define BIG_TEXTURE_WIDTH 256 * 3
#define BIG_TEXTURE_HEIGHT 192 * 3

#define WINDOW_WIDTH BIG_TEXTURE_WIDTH
#define WINDOW_HEIGHT BIG_TEXTURE_HEIGHT

#define LVGL_BLUE 0x2196f3
#define LVGL_COOLGRAY 0xe4f1fb
#define LVGL_COOLGRAY_DARKER 0xbbd9f1

#define TAB_VIEW 0
#define TAB_LOAD 1
#define TAB_INFO 2
#define MAX_TABS 3

#define MAX_SPRITES 7

#define MAX_PATH_LENGTH 256
#define MAX_OPTION_LENGTH 50

#define PI_TO_RAD 0.01745329238f

extern bool animate_spin;
extern float spin_rate;
extern float anim_rate;
extern int camera;
extern int anim;
extern unsigned int _current_tab;
extern bool use_scenecam;
extern bool anim_enabled;

extern lv_obj_t * grp_loading;
extern lv_obj_t * spin_checkbox;
extern lv_obj_t * spin_slider;
extern lv_obj_t * progText1;
extern lv_obj_t * tabview;
extern lv_glfw_window_t * window;
extern lv_obj_t * titleText;
extern lv_obj_t * tab_pages[MAX_TABS];
extern lv_obj_t * progbar1;
extern lv_obj_t * progbar2;
extern lv_obj_t * anim_checkbox;

extern pOverride ov_boom;
extern pOverride ov_stick;
extern pOverride ov_bucket;
extern pOverride ov_swing;
extern pOverride ov_cursor;

extern pGltf_data_t demo_gltfdata;
extern pViewer demo_gltfview;
extern GLFWwindow * glfw_window;

LV_IMAGE_DECLARE(lvgl_icon_40px);
LV_IMAGE_DECLARE(sprites1_32x32x7);

void demo_ui_loading_info_objects(void);
void demo_ui_pitch_yaw_distance_sliders(lv_obj_t * container);
void demo_ui_camera_select(lv_obj_t * container);
void demo_ui_animation_select(lv_obj_t * container);
void demo_ui_fill_in_InfoTab( pGltf_data_t _data );
void demo_ui_add_override_controls(lv_obj_t * container);
void demo_ui_apply_camera_button_visibility( pGltf_data_t _data);
void demo_ui_apply_anim_button_visibility( pGltf_data_t _data);
void demo_ui_set_tab(unsigned int _tabnum);
void demo_ui_make_underlayer(void);
void demo_ui_load_progress_callback(const char* phase_title, const char* sub_phase_title, float phase_progress, float phase_progress_max, float sub_phase_progress, float sub_phase_progress_max);
bool demo_cli_apply_commandline_options( pViewer viewer, char * gltfFile, char * hdrFile, int * frame_count, bool * software_only, float * _anim_rate, int argc, char *argv[] );
void demo_nav_process_drag(float movement_power, uint32_t mouse_state_ex, int mouse_x, int mouse_y, int last_mouse_x, int last_mouse_y);
void demo_os_integrate_setup_glfw_window(GLFWwindow * _window);

#endif // MAINUI_SHARED_H