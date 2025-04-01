#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/examples/lv_examples.h"

#include "gltf_loader.h"

#include <unistd.h> /* usleep */

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define LIST_WIDTH 450
#define LIST_HEIGHT 325

#define TEXTURE_WIDTH 100
#define TEXTURE_HEIGHT 100

static lv_obj_t * lv_3dtexture_from_gltf_model(lv_obj_t * parent, const char * path, uint32_t w, uint32_t h,
                                         lv_color_t color, lv_opa_t opa)
{
    lv_obj_t * tex = lv_3dtexture_create(parent);
    lv_3dtexture_id_t gltf_texture = render_gltf_model_to_opengl_texture(path, w, h, color);
    lv_3dtexture_set_src(tex, gltf_texture);
    lv_obj_set_size(tex, w, h);
    lv_obj_set_style_opa(tex, opa, 0);
    return tex;
}

static lv_obj_t * create_list_item(lv_obj_t * parent, const char * path, lv_color_t color, lv_opa_t opa, const char * name,
                                   const char * category, const char * status)
{
    static int32_t grid_col_dsc[] = {LV_GRID_CONTENT, 5, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);

    lv_obj_t * tex = lv_3dtexture_from_gltf_model(cont, path, TEXTURE_WIDTH, TEXTURE_HEIGHT, color, opa);
    lv_obj_set_grid_cell(tex, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 2);

    lv_obj_t * label;
    label = lv_label_create(cont);
    lv_label_set_text_static(label, name);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_END, 0, 1);

    label = lv_label_create(cont);
    lv_label_set_text_static(label, category);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_obj_set_style_text_opa(label, LV_OPA_50, 0);

    label = lv_label_create(cont);
    lv_label_set_text_static(label, status);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_END, 3, 1, LV_GRID_ALIGN_END, 0, 1);

    return cont;
}

int main()
{
    lv_init();

    /* create a window and initialize OpenGL */
    lv_glfw_window_t * window = lv_glfw_window_create(WINDOW_WIDTH, WINDOW_HEIGHT, true);

    /* create a display that flushes to a texture */
    lv_display_t * texture = lv_opengles_texture_create(WINDOW_WIDTH, WINDOW_HEIGHT);
    lv_display_set_default(texture);

    /* add the texture to the window */
    unsigned int display_texture = lv_opengles_texture_get_texture_id(texture);
    lv_glfw_texture_t * window_texture = lv_glfw_window_add_texture(window, display_texture, WINDOW_WIDTH, WINDOW_HEIGHT);

    /* get the mouse indev of the window texture */
    lv_indev_t * mouse = lv_glfw_texture_get_mouse_indev(window_texture);

    /* add a cursor to the mouse indev */
    LV_IMAGE_DECLARE(mouse_cursor_icon);
    lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(mouse, cursor_obj);


    lv_obj_t * list = lv_obj_create(lv_screen_active());
    lv_obj_set_size(list, LIST_WIDTH, LIST_HEIGHT);
    lv_obj_center(list);

    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(list, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    lv_obj_t * title = lv_label_create(list);
    lv_label_set_text(title, "3D Models");

    create_list_item(list, "gltfs/teapot.gltf", lv_color_hex(0xffff00), LV_OPA_100, "big yellow teapot", "yellow", "active");
    create_list_item(list, "gltfs/torusknot.gltf", lv_color_hex(0xff007f), LV_OPA_100, "torus knot", "pink", "inactive");
    create_list_item(list, "gltfs/teapot.gltf", lv_color_hex(0x00ff00), LV_OPA_100, "witch's teapot", "green", "inactive");
    create_list_item(list, "gltfs/torusknot.gltf", lv_color_hex(0x7f7f7f), LV_OPA_20, "translucent torus knot", "translucent", "active");

    lv_obj_t * scroll_down_label = lv_label_create(lv_screen_active());
    lv_obj_align_to(scroll_down_label, list, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_label_set_text_static(scroll_down_label, "scroll down");

    while(true) {
        uint32_t ms_delay = lv_timer_handler();
        usleep(ms_delay * 1000);
    }
}
