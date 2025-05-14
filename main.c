#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/examples/lv_examples.h"

#include "gltf_loader.h"

#include <unistd.h>     /* usleep */
#include <signal.h>     /* to trap ctrl-break */
#include <GL/glew.h>    /* For window size restrictions */
#include <GLFW/glfw3.h> /* For window size / title */

#define BIG_TEXTURE_WIDTH 256 * 3
#define BIG_TEXTURE_HEIGHT 192 * 3

#define WINDOW_WIDTH BIG_TEXTURE_WIDTH
#define WINDOW_HEIGHT BIG_TEXTURE_HEIGHT

#define LVGL_BLUE 0x2196f3
#define LVGL_COOLGRAY 0xe4f1fb
#define LVGL_COOLGRAY_DARKER 0xbbd9f1

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

    while(true) {
        uint32_t ms_delay = lv_timer_handler();
        usleep(ms_delay * 1000);
    }
}
