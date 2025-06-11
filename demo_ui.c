
#include "lvgl/lvgl.h"
#include "demo.h"
#include <math.h>

lv_obj_t * grp_loading;
lv_obj_t * spin_checkbox;
lv_obj_t * spin_slider;
lv_glfw_window_t * window;
//GLFWwindow * glfw_window;
lv_obj_t * progbar1;
lv_obj_t * progbar2;
lv_obj_t * progText1;
lv_obj_t * tabview;
lv_obj_t * viewTab;
lv_obj_t * loadTab;
lv_obj_t * titleText;
lv_obj_t * anim_checkbox;

lv_obj_t * infoTab;
    lv_obj_t * lbl_filename;
    lv_obj_t * lbl_mesh_summary;
    lv_obj_t * lbl_treesummary;

lv_style_t style_knob;
lv_style_t style_bg_vert;
lv_style_t style_bg_horiz;

bool __styles_ready = false;

#define MAX_CAM_BUTTONS 16
lv_obj_t * use_scenecam_checkbox;
lv_obj_t * cam_buttons[MAX_CAM_BUTTONS];
#define MAX_ANIM_BUTTONS 16
lv_obj_t * anim_buttons[MAX_ANIM_BUTTONS];

void __make_styles() {
    __styles_ready = true;
    //static lv_style_t style_knob;
    lv_style_init(&style_knob);
    lv_style_set_bg_opa(&style_knob, LV_OPA_COVER);
    lv_style_set_bg_color(&style_knob, lv_color_hex(LVGL_COOLGRAY));
    lv_style_set_bg_grad_color(&style_knob, lv_color_hex(LVGL_BLUE));
    lv_style_set_bg_grad_dir(&style_knob, LV_GRAD_DIR_VER);
    lv_style_set_border_color(&style_knob, lv_color_hex(LVGL_BLUE) );
    lv_style_set_border_opa(&style_knob, LV_OPA_50 );
    lv_style_set_border_width(&style_knob, 2);
    lv_style_set_bg_img_src(&style_knob, &sprites1_32x32x7);

    //static lv_style_t style_bg_vert;
    lv_style_init(&style_bg_vert);
    lv_style_set_bg_opa(&style_bg_vert, LV_OPA_60);
    lv_style_set_bg_color(&style_bg_vert, lv_color_hex(0x9ca2a7));
    lv_style_set_bg_grad_color(&style_bg_vert, lv_color_hex(LVGL_COOLGRAY));
    lv_style_set_bg_grad_dir(&style_bg_vert, LV_GRAD_DIR_HOR);
    lv_style_set_border_color(&style_bg_vert, lv_color_hex(LVGL_BLUE) );
    lv_style_set_border_opa(&style_bg_vert, LV_OPA_50 );
    lv_style_set_border_width(&style_bg_vert, 2);
    lv_style_set_bg_grad_stop(&style_bg_vert, 64);
    lv_style_set_bg_main_stop(&style_bg_vert, 16);

    //static lv_style_t style_bg_horiz;
    lv_style_init(&style_bg_horiz);
    lv_style_set_bg_opa(&style_bg_horiz, LV_OPA_60);
    lv_style_set_bg_color(&style_bg_horiz, lv_color_hex(0x9ca2a7));
    lv_style_set_bg_grad_color(&style_bg_horiz, lv_color_hex(LVGL_COOLGRAY));
    lv_style_set_bg_grad_dir(&style_bg_horiz, LV_GRAD_DIR_VER);
    lv_style_set_border_color(&style_bg_horiz, lv_color_hex(LVGL_BLUE) );
    lv_style_set_border_opa(&style_bg_horiz, LV_OPA_50 );
    lv_style_set_border_width(&style_bg_horiz, 2);
    lv_style_set_bg_grad_stop(&style_bg_horiz, 64);
    lv_style_set_bg_main_stop(&style_bg_horiz, 16);

}

lv_obj_t * __make_sprite(unsigned int _spriteNum, lv_obj_t * _parent){
    const int SPRITE_SIZE = 32;
    lv_obj_t * img1 = lv_image_create(_parent);
    lv_image_set_src(img1, &sprites1_32x32x7);
    lv_obj_set_size(img1, SPRITE_SIZE, SPRITE_SIZE);
    lv_img_set_offset_y(img1,  (SPRITE_SIZE * MAX_SPRITES) - ( SPRITE_SIZE * ( _spriteNum  + 4) ) );
    return img1;
}

void demo_ui_set_tab(unsigned int _tabnum) {
    _current_tab = _tabnum;
    lv_tabview_set_active(tabview, _current_tab + 1, LV_ANIM_OFF);
    for (unsigned int i=0; i<MAX_TABS; i++) {
        if (_current_tab == i) {
            lv_obj_clear_flag(tab_pages[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(tab_pages[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void yaw_slider_event_cb(lv_event_t * e){
    if (animate_spin) {
        animate_spin = false;
        lv_obj_remove_state(spin_checkbox, LV_STATE_CHECKED);
        lv_obj_add_flag(spin_slider, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_t * slider = lv_event_get_target_obj(e);
    lv_gltfview_set_yaw(demo_gltfview, (int)(((float)lv_slider_get_value(slider) / 1000.0f ) * 3600.f));
    //yaw_degree_offset = ((float)lv_slider_get_value(slider) / 1000.0f ) * 360.f;
}

static void pitch_slider_event_cb(lv_event_t * e){
    lv_obj_t * slider = lv_event_get_target_obj(e);
    lv_gltfview_set_pitch(demo_gltfview, (int)(((float)lv_slider_get_value(slider) / 100.0f ) * 1800.f));
    //pitch_degrees = ((float)lv_slider_get_value(slider) / 100.0f ) * 180.f;
}

static void spin_slider_event_cb(lv_event_t * e){
    lv_obj_t * slider = lv_event_get_target_obj(e);
    spin_rate = ((float)lv_slider_get_value(slider) / 100.0f ) * 4.f * 60.0f;
}

static void distance_slider_event_cb(lv_event_t * e){
    lv_obj_t * slider = lv_event_get_target_obj(e);
    float _distance = (1.f - ((float)lv_slider_get_value(slider) / 100.0f )) * 2.0f;
    if (_distance < 1.0f) {
        _distance = pow(_distance, 4.0);
    } else {
        _distance = pow(_distance, 2.0);
    }
    lv_gltfview_set_distance(demo_gltfview, (int)(_distance * 1000.0f));

}

/*
static void elev_slider_event_cb(lv_event_t * e){
    lv_obj_t * slider = lv_event_get_target_obj(e);
    elevation = ((float)lv_slider_get_value(slider) / 100.0f ) * 0.5f;
} */

static void spin_checkbox_event_cb(lv_event_t * e){
    LV_UNUSED(e);
    animate_spin = (lv_obj_get_state(spin_checkbox) & LV_STATE_CHECKED);
    if (animate_spin) {
        lv_obj_clear_flag(spin_slider, LV_OBJ_FLAG_HIDDEN);
    } else{
        lv_obj_add_flag(spin_slider, LV_OBJ_FLAG_HIDDEN);
    }
}

static void tab_clicked_event_cb(lv_event_t * e){
    lv_obj_t * button = lv_event_get_current_target(e);
    int32_t idx = lv_obj_get_index_by_type(button, &lv_button_class);
    demo_ui_set_tab(idx - 1);
}

static void use_scenecam_checkbox_event_cb(lv_event_t * e){
    LV_UNUSED(e);
    use_scenecam = (lv_obj_get_state(use_scenecam_checkbox) & LV_STATE_CHECKED);
    camselection_changed = true;
}

static void anim_checkbox_event_cb(lv_event_t * e){
    LV_UNUSED(e);
    anim_enabled = (lv_obj_get_state(anim_checkbox) & LV_STATE_CHECKED);
    animselection_changed = true;
}

// default value means no preference, it will use the first available scene camera if defined, otherwise the generated platter camera
void __select_camera_default_cb(lv_event_t * e) { LV_UNUSED(e); camera = -2; }
// this explictly selects the platter camera even if a scene camera exists
void __select_camera_0_cb(lv_event_t * e) { LV_UNUSED(e); camera = -1; }
// these select scene cameras #1->#16 (base 0 internally)
void __select_camera_1_cb(lv_event_t * e) { LV_UNUSED(e); camera = 0; }
void __select_camera_2_cb(lv_event_t * e) { LV_UNUSED(e); camera = 1; }
void __select_camera_3_cb(lv_event_t * e) { LV_UNUSED(e); camera = 2; }
void __select_camera_4_cb(lv_event_t * e) { LV_UNUSED(e); camera = 3; }
void __select_camera_5_cb(lv_event_t * e) { LV_UNUSED(e); camera = 4; }
void __select_camera_6_cb(lv_event_t * e) { LV_UNUSED(e); camera = 5; }
void __select_camera_7_cb(lv_event_t * e) { LV_UNUSED(e); camera = 6; }
void __select_camera_8_cb(lv_event_t * e) { LV_UNUSED(e); camera = 7; }
void __select_camera_9_cb(lv_event_t * e) { LV_UNUSED(e); camera = 8; }
void __select_camera_10_cb(lv_event_t * e) { LV_UNUSED(e); camera = 9; }
void __select_camera_11_cb(lv_event_t * e) { LV_UNUSED(e); camera = 10; }
void __select_camera_12_cb(lv_event_t * e) { LV_UNUSED(e); camera = 11; }
void __select_camera_13_cb(lv_event_t * e) { LV_UNUSED(e); camera = 12; }
void __select_camera_14_cb(lv_event_t * e) { LV_UNUSED(e); camera = 13; }
void __select_camera_15_cb(lv_event_t * e) { LV_UNUSED(e); camera = 14; }
void __select_camera_16_cb(lv_event_t * e) { LV_UNUSED(e); camera = 15; }


void __select_anim_1_cb(lv_event_t * e) { LV_UNUSED(e); anim = 0; }
void __select_anim_2_cb(lv_event_t * e) { LV_UNUSED(e); anim = 1; }
void __select_anim_3_cb(lv_event_t * e) { LV_UNUSED(e); anim = 2; }
void __select_anim_4_cb(lv_event_t * e) { LV_UNUSED(e); anim = 3; }
void __select_anim_5_cb(lv_event_t * e) { LV_UNUSED(e); anim = 4; }
void __select_anim_6_cb(lv_event_t * e) { LV_UNUSED(e); anim = 5; }
void __select_anim_7_cb(lv_event_t * e) { LV_UNUSED(e); anim = 6; }
void __select_anim_8_cb(lv_event_t * e) { LV_UNUSED(e); anim = 7; }
void __select_anim_9_cb(lv_event_t * e) { LV_UNUSED(e); anim = 8; }
void __select_anim_10_cb(lv_event_t * e) { LV_UNUSED(e); anim = 9; }
void __select_anim_11_cb(lv_event_t * e) { LV_UNUSED(e); anim = 10; }
void __select_anim_12_cb(lv_event_t * e) { LV_UNUSED(e); anim = 11; }
void __select_anim_13_cb(lv_event_t * e) { LV_UNUSED(e); anim = 12; }
void __select_anim_14_cb(lv_event_t * e) { LV_UNUSED(e); anim = 13; }
void __select_anim_15_cb(lv_event_t * e) { LV_UNUSED(e); anim = 14; }
void __select_anim_16_cb(lv_event_t * e) { LV_UNUSED(e); anim = 15; }


static void ov_swing_slider_event_cb(lv_event_t * e){
    const float MIN_SWING = -180.0f * PI_TO_RAD;
    const float MAX_SWING = 180.0f * PI_TO_RAD;
    lv_obj_t * slider = lv_event_get_target_obj(e);
    float normval = ((float)lv_slider_get_value(slider) / 1000.0f );
    ov_swing->data1 = ( normval * (MAX_SWING - MIN_SWING) ) + MIN_SWING;
    ov_swing->data2 = 0.f;
    ov_swing->data3 = 0.f;
}

static void ov_boom_slider_event_cb(lv_event_t * e){
    const float MAX_BOOM = 100.0f * PI_TO_RAD;
    const float MIN_BOOM = -25.0f * PI_TO_RAD;
    lv_obj_t * slider = lv_event_get_target_obj(e);
    float normval = ((float)lv_slider_get_value(slider) / 1000.0f ) ;
    ov_boom->data2 = ( normval * (MAX_BOOM - MIN_BOOM) ) + MIN_BOOM;
}

static void ov_stick_slider_event_cb(lv_event_t * e){
    const float MAX_STICK = 20.0f * PI_TO_RAD;
    const float MIN_STICK = 160.0f * PI_TO_RAD;
    lv_obj_t * slider = lv_event_get_target_obj(e);
    float normval = ((float)lv_slider_get_value(slider) / 1000.0f ) ;
    ov_stick->data2 = ( normval * (MAX_STICK - MIN_STICK) ) + MIN_STICK;
}
/*
static void ov_bucket_slider_event_cb(lv_event_t * e){
    const float MIN_BUCKET = -70.0f;
    const float MAX_BUCKET = 30.0f;
    lv_obj_t * slider = lv_event_get_target_obj(e);
    float normval = ((float)lv_slider_get_value(slider) / 1000.0f ) ;
    ov_bucket->data2 = ( normval * (MAX_BUCKET - MIN_BUCKET) ) + MIN_BUCKET;
}*/

static void ext_draw_size_event_cb(lv_event_t * e){
    lv_coord_t * cur_size = (lv_coord_t*)lv_event_get_param(e);
    *cur_size = LV_MAX(*cur_size, LV_HOR_RES);
}

void lv_loading_info_objects(void){
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

#define TBUF_SIZE 32768
void __fill_in_InfoTab( pGltf_data_t _data ) {
    int _cury = 40;
    const int LABEL_HEIGHT = 16;
    int _ll = 0;
    char * _c = calloc(16384 * 16, 1);
    strcat(_c, "Filename: ");
    strcat(_c, "asdf.asdf");
    strcat(_c, "\n"); _ll++;
    strcat(_c, "Directory: ");
    strcat(_c, "folder_path");
    strcat(_c, "\n"); _ll++;
    strcat(_c, "File Size: ");
    strcat(_c, "0000kB");
    strcat(_c, "\n"); _ll++;
    strcat(_c, "\n"); _ll++;
    lv_label_set_text(lbl_filename, _c);
    lv_obj_align(lbl_filename, LV_ALIGN_TOP_LEFT, 70, _cury);
    _cury+=(LABEL_HEIGHT * _ll);

    _c[0] = '\0'; _ll = 0;
    strcat(_c, "Vertices (File / Displayed): ");
    strcat(_c, "12345");
    strcat(_c, " / ");
    strcat(_c, "1234567");
    strcat(_c, "\n"); _ll++;
    strcat(_c, "Triangles (File / Displayed): ");
    strcat(_c, "2345");
    strcat(_c, " / ");
    strcat(_c, "234567");
    strcat(_c, "\n"); _ll++;
    strcat(_c, "\n"); _ll++;    
    lv_label_set_text(lbl_mesh_summary, _c);
    lv_obj_align(lbl_mesh_summary, LV_ALIGN_TOP_LEFT, 70, _cury);
    _cury+=(LABEL_HEIGHT * _ll); _c[0] = '\0'; _ll = 0;


    char * _tbuf;
    _tbuf  = (char *)malloc(TBUF_SIZE);

    lv_gltf_make_scenes_summary( _data, _tbuf, TBUF_SIZE );
    strcat(_c, _tbuf);
    lv_gltf_make_mesh_summary( _data, _tbuf, TBUF_SIZE );
    strcat(_c, _tbuf);
    lv_gltf_make_material_summary( _data, _tbuf, TBUF_SIZE );
    strcat(_c, _tbuf);
    lv_gltf_make_images_summary( _data, _tbuf, TBUF_SIZE );
    strcat(_c, _tbuf);
    strcat(_c, "Nodes: ");
    strcat(_c, "13");
    strcat(_c, "\n"); _ll++;
    //make_animations_summary( _data, _tbuf, TBUF_SIZE );
    //strcat(_c, _tbuf);
    //strcat(_c, "\n"); _ll++;
    lv_label_set_text(lbl_treesummary, _c);
    lv_obj_align(lbl_treesummary, LV_ALIGN_TOP_LEFT, 70, _cury);
    _cury+=(LABEL_HEIGHT * _ll); _c[0] = '\0'; _ll = 0;

    free(_c);
    free(_tbuf);
}

void __make_InfoTab(void) {
    lv_obj_t * background = tab_pages[TAB_INFO];
    lv_obj_t * white_bg = lv_obj_create(background);
    lv_obj_remove_style_all(white_bg);
    lv_obj_set_size(white_bg, WINDOW_WIDTH-60, WINDOW_HEIGHT);
    lv_obj_set_style_bg_color(white_bg, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(white_bg, LV_OPA_100, LV_PART_MAIN);
    lv_obj_align(white_bg, LV_ALIGN_TOP_RIGHT, 0, 0);

    lv_obj_t * inner_background = lv_obj_create(background);
    lv_obj_set_size(inner_background, WINDOW_WIDTH-74, WINDOW_HEIGHT-44);
    lv_obj_align(inner_background, LV_ALIGN_TOP_LEFT, 60, 30);
    lv_obj_set_style_bg_color(inner_background, lv_color_hex(0xf0faff), LV_PART_MAIN);
    //lv_obj_set_style_bg_color(inner_background, lv_color_hex(0x303a3f), LV_PART_MAIN);
    lv_obj_set_style_border_color(inner_background, lv_color_hex(LVGL_BLUE), LV_PART_MAIN);
    lv_obj_set_style_border_width(inner_background, 1, LV_PART_MAIN);
    //lv_obj_clear_flag(inner_background, LV_OBJ_FLAG_CLICKABLE  );
    //lv_obj_clear_flag(inner_background, LV_OBJ_FLAG_CLICKABLE  );
    lv_obj_set_style_bg_grad_color(inner_background, lv_color_hex(0xd0dce6), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(inner_background, LV_GRAD_DIR_VER, LV_PART_MAIN);
     
    //lv_obj_t * spinImg = __make_sprite(5, background);
    //lv_obj_align(spinImg, LV_ALIGN_BOTTOM_RIGHT, -15, -15);
    lbl_filename = lv_label_create(inner_background);
    lbl_mesh_summary = lv_label_create(inner_background);
    lbl_treesummary = lv_label_create(inner_background);

    //__fill_in_InfoTab()
}

void __make_ViewTab(void) {
    lv_obj_t * background = tab_pages[TAB_VIEW];
    lv_obj_t * white_bg = lv_obj_create(background);
    lv_obj_remove_style_all(white_bg);
    lv_obj_set_size(white_bg, WINDOW_WIDTH-60, WINDOW_HEIGHT);
    lv_obj_set_style_bg_color(white_bg, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(white_bg, LV_OPA_100, LV_PART_MAIN);
    lv_obj_align(white_bg, LV_ALIGN_TOP_RIGHT, 0, 0);

    lv_obj_t * inner_background = lv_obj_create(background);
    lv_obj_set_size(inner_background, WINDOW_WIDTH-114, WINDOW_HEIGHT-84);
    lv_obj_align(inner_background, LV_ALIGN_TOP_LEFT, 60, 30);
    lv_obj_set_style_bg_color(inner_background, lv_color_hex(0x303a3f), LV_PART_MAIN);
    //lv_obj_set_style_bg_color(inner_background, lv_color_hex(0xf0faff), LV_PART_MAIN);
    lv_obj_set_style_border_color(inner_background, lv_color_hex(LVGL_BLUE), LV_PART_MAIN);
    lv_obj_set_style_border_width(inner_background, 1, LV_PART_MAIN);
    lv_obj_clear_flag(inner_background, LV_OBJ_FLAG_CLICKABLE  );
    lv_obj_set_style_bg_grad_color(inner_background, lv_color_hex(0xd0dce6), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(inner_background, LV_GRAD_DIR_VER, LV_PART_MAIN);
     
    //lv_obj_t * spinImg = __make_sprite(5, background);
    //lv_obj_align(spinImg, LV_ALIGN_BOTTOM_RIGHT, -15, -15);

    

}

void __make_WindowBevelsAndIcon(void) {
    {
        lv_obj_t * tbev = lv_obj_create(lv_screen_active());
        lv_obj_remove_style_all(tbev);
        lv_obj_set_size(tbev, WINDOW_WIDTH, 2);
        lv_obj_set_style_bg_color(tbev, lv_color_hex(0x595f66), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(tbev, LV_OPA_80, LV_PART_MAIN);
        lv_obj_align(tbev, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_clear_flag(tbev, LV_OBJ_FLAG_CLICKABLE  );
    }
    {
        lv_obj_t * tbev = lv_obj_create(lv_screen_active());
        lv_obj_remove_style_all(tbev);
        lv_obj_set_size(tbev, WINDOW_WIDTH, 1);
        lv_obj_set_style_bg_color(tbev, lv_color_hex(0x595f66), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(tbev, LV_OPA_50, LV_PART_MAIN);
        lv_obj_align(tbev, LV_ALIGN_BOTTOM_MID , 0, -1);
        lv_obj_clear_flag(tbev, LV_OBJ_FLAG_CLICKABLE  );
    }
    {
        lv_obj_t * tbev = lv_obj_create(lv_screen_active());
        lv_obj_remove_style_all(tbev);
        lv_obj_set_size(tbev, WINDOW_WIDTH, 1);
        lv_obj_set_style_bg_color(tbev, lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(tbev, LV_OPA_80, LV_PART_MAIN);
        lv_obj_align(tbev, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_clear_flag(tbev, LV_OBJ_FLAG_CLICKABLE  );
    }
    {
        lv_obj_t * tbev = lv_obj_create(lv_screen_active());
        lv_obj_remove_style_all(tbev);
        lv_obj_set_size(tbev, 1, WINDOW_HEIGHT);
        lv_obj_set_style_bg_color(tbev, lv_color_hex(0x595f66), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(tbev, LV_OPA_50, LV_PART_MAIN);
        lv_obj_align(tbev, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
        lv_obj_clear_flag(tbev, LV_OBJ_FLAG_CLICKABLE  );
    }
    lv_obj_t * img1 = lv_image_create(lv_screen_active());
    lv_image_set_src(img1, &lvgl_icon_40px);
    lv_obj_align(img1, LV_ALIGN_TOP_LEFT, 10, 9);
}

unsigned int __add_blanktab_px(lv_obj_t * _tabview, unsigned int _px_length) {
    lv_obj_t * _tabs = lv_tabview_get_tab_bar(_tabview);
    lv_obj_t * _blankTabContents = lv_tabview_add_tab(_tabview, "");
    LV_UNUSED(_blankTabContents);
    uint32_t _count = lv_obj_get_child_count_by_type(_tabs, &lv_button_class);

    uint32_t i = 0;
    lv_obj_t * button = lv_obj_get_child_by_type(_tabs, i, &lv_button_class);
    while(button) {
        if (i == _count-1){
            lv_obj_t * _newTab = button;
            lv_obj_clear_flag(_newTab, LV_OBJ_FLAG_CLICKABLE  );
            lv_obj_set_flex_grow(_newTab, 0.1f);
            lv_obj_set_style_bg_opa(_newTab, LV_OPA_0, LV_PART_ANY);
            lv_obj_set_size(button, lv_pct(100), lv_dpx(_px_length));
        }
        i++;
        button = lv_obj_get_child_by_type(_tabs, (int32_t)i, &lv_button_class);
    }
    return _px_length;
}

void __customize_last_tab(lv_obj_t * _tabview, lv_obj_t * _tabContents) {
    if (_tabContents != NULL) {
        lv_obj_set_style_bg_color(_tabContents, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    }
    lv_obj_t * _tabs = lv_tabview_get_tab_bar(_tabview);
    uint32_t _count = lv_obj_get_child_count_by_type(_tabs, &lv_button_class);
    uint32_t i = 0;
    lv_obj_t * button = lv_obj_get_child_by_type(_tabs, i, &lv_button_class);
    while(button) {
        if (i == _count-1){
            lv_obj_t * _label = lv_obj_get_child_by_type(button, 0, &lv_label_class);
            lv_obj_align(_label, LV_ALIGN_BOTTOM_MID, 0, -10);
            lv_obj_t * _img = __make_sprite(i-1, _label);
            lv_obj_align(_img, LV_ALIGN_TOP_MID, 0, 21);
            lv_obj_add_event_cb(button, tab_clicked_event_cb, LV_EVENT_CLICKED, NULL);
        }
        i++;
        button = lv_obj_get_child_by_type(_tabs, (int32_t)i, &lv_button_class);
    }

}

void lv_main_tabview(void) {
    tabview = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
    lv_tabview_set_tab_bar_size(tabview, 60);
    __add_blanktab_px(tabview, 80); //, -1, -1);
    viewTab = lv_tabview_add_tab(tabview, "View");
    __customize_last_tab(tabview, viewTab);
    loadTab = lv_tabview_add_tab(tabview, "Load");
    __customize_last_tab(tabview, loadTab);
    infoTab = lv_tabview_add_tab(tabview, "Info");
    __customize_last_tab(tabview, infoTab);
    __add_blanktab_px(tabview, 360);//, -1, -1);

    lv_tabview_set_active(tabview, 1, LV_ANIM_OFF);
    for (int i=0; i<MAX_TABS; i++) {
        tab_pages[i] = lv_obj_create(lv_screen_active());
        lv_obj_add_flag(tab_pages[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(tab_pages[i], LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(tab_pages[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_remove_style_all(tab_pages[i]);
        lv_obj_set_size(tab_pages[i], WINDOW_WIDTH, WINDOW_HEIGHT);
        lv_obj_center(tab_pages[i]);
        
   }
}

void lv_pitch_yaw_distance_sliders(lv_obj_t * _cont){
    if (!__styles_ready) {__make_styles();}

    {
        // Yaw (bottom)
        lv_obj_t * slider = lv_slider_create(_cont);
        lv_obj_set_size(slider, WINDOW_WIDTH - 140, 24);
        lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -16);
        lv_obj_add_event_cb(slider, yaw_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(slider, 2000, 0);
        lv_obj_set_style_bg_opa(slider, LV_OPA_40, LV_PART_INDICATOR );
        lv_slider_set_range(slider, -500, 500);
        lv_slider_set_mode(slider, LV_SLIDER_MODE_SYMMETRICAL);
        lv_slider_set_value(slider, 0.f, 0);
        lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);
        lv_obj_add_style(slider, &style_bg_horiz, LV_PART_MAIN);
    }
    {
        // Pitch (lower right)
        lv_obj_t * slider = lv_slider_create(_cont);
        lv_obj_set_size(slider, 24, ((WINDOW_HEIGHT - 90) / 2));
        lv_obj_align(slider, LV_ALIGN_BOTTOM_RIGHT, -15, -70);
        lv_obj_add_event_cb(slider, pitch_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(slider, 2000, 0);
        lv_obj_set_style_bg_opa(slider, LV_OPA_40, LV_PART_INDICATOR );
        lv_slider_set_range(slider, -50, 50);
        lv_slider_set_mode(slider, LV_SLIDER_MODE_SYMMETRICAL);
        lv_slider_set_value(slider, 0.f, 0);
        lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);        
        lv_obj_add_style(slider, &style_bg_vert, LV_PART_MAIN);
    }
    {
        // Distance (upper right)
        lv_obj_t * slider = lv_slider_create(_cont);
        lv_obj_set_size(slider, 24, ((WINDOW_HEIGHT - 90) / 2)-20);
        lv_obj_align(slider, LV_ALIGN_TOP_RIGHT, -15, 30);
        lv_obj_add_event_cb(slider, distance_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(slider, 2000, 0);
        lv_obj_set_style_bg_opa(slider, LV_OPA_40, LV_PART_INDICATOR );
        lv_obj_set_style_width(slider, 10, LV_PART_INDICATOR );
        lv_slider_set_value(slider, 50.f, 0);
        lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);
        lv_obj_add_style(slider, &style_bg_vert, LV_PART_MAIN);
    }
    {
        // Spin enable/disable (upper right)
        spin_checkbox = lv_checkbox_create(_cont);
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
        spin_slider = lv_slider_create(_cont);
        lv_obj_set_size(spin_slider, 200, 10);
        lv_obj_align(spin_slider, LV_ALIGN_TOP_RIGHT, -140, 10);
        lv_obj_add_event_cb(spin_slider, spin_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(spin_slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(spin_slider, 2000, 0);
        lv_obj_set_style_bg_opa(spin_slider, LV_OPA_40, LV_PART_INDICATOR );
        lv_obj_set_style_border_width(spin_slider, 1, LV_PART_MAIN );
        lv_obj_set_style_border_color(spin_slider, lv_color_hex(0x9ca2a7), LV_PART_MAIN );
        lv_obj_set_style_border_opa(spin_slider, LV_OPA_40, LV_PART_MAIN );
        lv_slider_set_range(spin_slider, -50, 50);
        lv_slider_set_value(spin_slider, 25.f, 0);
        lv_slider_set_mode(spin_slider, LV_SLIDER_MODE_SYMMETRICAL);
        lv_obj_add_style(spin_slider, &style_bg_horiz, LV_PART_MAIN);
    }
    /*{
        // Elevation nudge (lower left)
        lv_obj_t * elev_slider = lv_slider_create(_cont);
        lv_obj_set_size(elev_slider, 10, 200);
        lv_obj_align(elev_slider, LV_ALIGN_BOTTOM_LEFT, -3, -70);
        lv_obj_add_event_cb(elev_slider, elev_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(elev_slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(elev_slider, 2000, 0);
        lv_obj_set_style_border_width(elev_slider, 1, LV_PART_MAIN );
        lv_obj_set_style_border_opa(elev_slider, LV_OPA_40, LV_PART_MAIN );
        lv_slider_set_range(elev_slider, -50, 50);
        lv_slider_set_value(elev_slider, 0.f, 0);
        lv_slider_set_mode(elev_slider, LV_SLIDER_MODE_SYMMETRICAL);
        lv_obj_add_style(elev_slider, &style_bg_vert, LV_PART_MAIN);
    }*/     
}

void lv_camera_select(lv_obj_t * _cont){
    if (!__styles_ready) {__make_styles();}

    {
        // Scene camera enable/disable (upper left)
        use_scenecam_checkbox = lv_checkbox_create(_cont);
        lv_obj_set_size(use_scenecam_checkbox, 85, 40);
        lv_obj_align(use_scenecam_checkbox, LV_ALIGN_TOP_LEFT, 62, 36);
        lv_obj_add_event_cb(use_scenecam_checkbox, use_scenecam_checkbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_set_style_anim_duration(use_scenecam_checkbox, 2000, 0);
        lv_obj_add_state(use_scenecam_checkbox, LV_STATE_CHECKED);
        lv_checkbox_set_text(use_scenecam_checkbox, "Camera");
        lv_obj_set_style_text_opa(use_scenecam_checkbox, LV_OPA_50, LV_PART_MAIN);
        lv_obj_set_style_text_color(use_scenecam_checkbox, lv_color_hex(LVGL_BLUE), LV_PART_MAIN);
        lv_obj_add_flag(use_scenecam_checkbox, LV_OBJ_FLAG_HIDDEN);
    }

    unsigned int _rx = 150;
    unsigned int _xspacing = 34;
    char buffer[255];
    for (int i=0; i< MAX_CAM_BUTTONS; i++) {
        // Camera X Selector
        lv_obj_t * button = lv_button_create(_cont);
        lv_obj_set_size(button, 24, 24);
        lv_obj_align(button, LV_ALIGN_TOP_LEFT, _rx, 36);
        lv_obj_set_style_anim_duration(button, 2000, 0);
        lv_obj_t * label = lv_label_create(button);
        sprintf(buffer, "%d", i+1);
        lv_label_set_text(label, buffer);
        lv_obj_center(label);
        lv_obj_add_style(button, &style_bg_horiz, LV_PART_MAIN);
        lv_obj_add_flag(button, LV_OBJ_FLAG_HIDDEN);
        cam_buttons[i] = button;
        _rx += _xspacing;
    }
    lv_obj_add_event_cb(cam_buttons[0], __select_camera_1_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[1], __select_camera_2_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[2], __select_camera_3_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[3], __select_camera_4_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[4], __select_camera_5_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[5], __select_camera_6_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[6], __select_camera_7_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[7], __select_camera_8_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[8], __select_camera_9_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[9], __select_camera_10_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[10], __select_camera_11_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[11], __select_camera_12_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[12], __select_camera_13_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[13], __select_camera_14_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[14], __select_camera_15_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cam_buttons[15], __select_camera_16_cb, LV_EVENT_CLICKED, NULL);
}

void demo_ui_apply_camera_button_visibility(pGltf_data_t _data){
    camselection_changed = false;
    //gltf_probe_info * probe = (gltf_probe_info*)lv_gltfview_get_probe(_data);
    gltf_probe_info * probe = lv_gltfview_get_probe(_data);
    for (unsigned int i=0; i<MAX_CAM_BUTTONS; i++)
        if (use_scenecam && (i < probe->cameraCount)) lv_obj_clear_flag(cam_buttons[i], LV_OBJ_FLAG_HIDDEN);
            else lv_obj_add_flag(cam_buttons[i], LV_OBJ_FLAG_HIDDEN);
    if (probe->cameraCount > 0) lv_obj_clear_flag(use_scenecam_checkbox, LV_OBJ_FLAG_HIDDEN);
        else lv_obj_add_flag(use_scenecam_checkbox, LV_OBJ_FLAG_HIDDEN);
}

void __add_override_controls( lv_obj_t * _cont ) {
    if (!__styles_ready) {__make_styles();}
    {   // Boom (lower left)
        lv_obj_t * slider = lv_slider_create(_cont);
        lv_obj_set_size(slider, 24, ((WINDOW_HEIGHT - 90) / 2));
        lv_obj_align(slider, LV_ALIGN_BOTTOM_LEFT, 75, -70);
        lv_obj_add_event_cb(slider, ov_boom_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(slider, 2000, 0);
        lv_obj_set_style_bg_opa(slider, LV_OPA_40, LV_PART_INDICATOR );
        lv_slider_set_range(slider, 0, 1000);
        lv_slider_set_mode(slider, LV_SLIDER_MODE_SYMMETRICAL);
        lv_slider_set_value(slider, 0.f, 0);
        lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);        
        lv_obj_add_style(slider, &style_bg_vert, LV_PART_MAIN);
    }
    {   // Stick (lower left + 1)
        lv_obj_t * slider = lv_slider_create(_cont);
        lv_obj_set_size(slider, 24, ((WINDOW_HEIGHT - 90) / 2));
        lv_obj_align(slider, LV_ALIGN_BOTTOM_LEFT, 105, -70);
        lv_obj_add_event_cb(slider, ov_stick_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(slider, 2000, 0);
        lv_obj_set_style_bg_opa(slider, LV_OPA_40, LV_PART_INDICATOR );
        lv_slider_set_range(slider, 0, 1000);
        lv_slider_set_mode(slider, LV_SLIDER_MODE_SYMMETRICAL);
        lv_slider_set_value(slider, 0.f, 0);
        lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);        
        lv_obj_add_style(slider, &style_bg_vert, LV_PART_MAIN);
    }
    {   // Swing (bottom)
        lv_obj_t * slider = lv_slider_create(_cont);
        lv_obj_set_size(slider, WINDOW_WIDTH - 280, 24);
        lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 20, -76);
        lv_obj_add_event_cb(slider, ov_swing_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST  );
        lv_obj_set_style_anim_duration(slider, 2000, 0);
        lv_obj_set_style_bg_opa(slider, LV_OPA_40, LV_PART_INDICATOR );
        lv_slider_set_range(slider, 0, 1000);
        lv_slider_set_mode(slider, LV_SLIDER_MODE_SYMMETRICAL);
        lv_slider_set_value(slider, 0.f, 0);
        lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);
        lv_obj_add_style(slider, &style_bg_horiz, LV_PART_MAIN);
    }
}


void lv_animation_select(lv_obj_t * _cont){
    if (!__styles_ready) {__make_styles();}

    {
        // Scene camera enable/disable (upper left)
        anim_checkbox = lv_checkbox_create(_cont);
        lv_obj_set_size(anim_checkbox, 85, 40);
        lv_obj_align(anim_checkbox, LV_ALIGN_BOTTOM_LEFT, 62,-45);
        lv_obj_add_event_cb(anim_checkbox, anim_checkbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_set_style_anim_duration(anim_checkbox, 2000, 0);
        lv_obj_add_state(anim_checkbox, LV_STATE_CHECKED);
        lv_checkbox_set_text(anim_checkbox, "Anim");
        lv_obj_set_style_text_opa(anim_checkbox, LV_OPA_50, LV_PART_MAIN);
        lv_obj_set_style_text_color(anim_checkbox, lv_color_hex(LVGL_BLUE), LV_PART_MAIN);
        lv_obj_add_flag(anim_checkbox, LV_OBJ_FLAG_HIDDEN);
    }

    unsigned int _rx = 150;
    unsigned int _xspacing = 34;
    char buffer[255];
    for (int i=0; i< MAX_ANIM_BUTTONS; i++) {
        // Animation X Selector
        lv_obj_t * button = lv_button_create(_cont);
        lv_obj_set_size(button, 24, 24);
        lv_obj_align(button, LV_ALIGN_BOTTOM_LEFT, _rx, -65);
        lv_obj_set_style_anim_duration(button, 2000, 0);
        lv_obj_t * label = lv_label_create(button);
        sprintf(buffer, "%d", i+1);
        lv_label_set_text(label, buffer);
        lv_obj_center(label);
        lv_obj_add_style(button, &style_bg_horiz, LV_PART_MAIN);
        lv_obj_add_flag(button, LV_OBJ_FLAG_HIDDEN);
        anim_buttons[i] = button;
        _rx += _xspacing;
    }
    lv_obj_add_event_cb(anim_buttons[0], __select_anim_1_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[1], __select_anim_2_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[2], __select_anim_3_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[3], __select_anim_4_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[4], __select_anim_5_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[5], __select_anim_6_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[6], __select_anim_7_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[7], __select_anim_8_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[8], __select_anim_9_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[9], __select_anim_10_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[10], __select_anim_11_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[11], __select_anim_12_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[12], __select_anim_13_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[13], __select_anim_14_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[14], __select_anim_15_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim_buttons[15], __select_anim_16_cb, LV_EVENT_CLICKED, NULL);
    animselection_changed=true;
}

void demo_ui_apply_anim_button_visibility(pGltf_data_t _data){
    animselection_changed = false;
    //gltf_probe_info * probe = (gltf_probe_info*)lv_gltfview_get_probe(_data);
    gltf_probe_info * probe = lv_gltfview_get_probe(_data);
    for (unsigned int i=0; i<MAX_ANIM_BUTTONS; i++)
        if (anim_enabled && i < probe->animationCount) lv_obj_clear_flag(anim_buttons[i], LV_OBJ_FLAG_HIDDEN);
            else lv_obj_add_flag(anim_buttons[i], LV_OBJ_FLAG_HIDDEN);
    if (probe->animationCount > 0) lv_obj_clear_flag(anim_checkbox, LV_OBJ_FLAG_HIDDEN);
        else lv_obj_add_flag(anim_checkbox, LV_OBJ_FLAG_HIDDEN);
}

void demo_ui_make_underlayer( void ) {
    lv_main_tabview();
    demo_ui_set_tab(TAB_VIEW);
    __make_ViewTab();
    __make_InfoTab();
    __make_WindowBevelsAndIcon();
}