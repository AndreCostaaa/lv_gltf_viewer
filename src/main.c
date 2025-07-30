#include <GL/glew.h> /* For window size restrictions */
#include <GLFW/glfw3.h> /* For window size / title */
#include <lvgl.h>
#include <src/core/lv_obj_pos.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>

#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 768
#define WINDOW_TITLE  "LVGL 3D Demo"

int main(void)
{
	/* initialize lvgl */
	lv_init();

	/* GLFW setup */
	lv_glfw_window_t *window = lv_glfw_window_create(WINDOW_WIDTH, WINDOW_HEIGHT, true);
	lv_display_t *texture = lv_opengles_texture_create(WINDOW_WIDTH, WINDOW_HEIGHT);
	unsigned int texture_id = lv_opengles_texture_get_texture_id(texture);
	lv_glfw_window_add_texture(window, texture_id, WINDOW_WIDTH, WINDOW_HEIGHT);

	/* GLTF setup */
	lv_obj_t *viewer = lv_gltf_create(lv_screen_active());
	lv_gltf_load_model_from_file(viewer, "gltfs/venom_comp.glb");
	lv_obj_set_size(viewer, WINDOW_WIDTH, WINDOW_HEIGHT);

	while (1) {
		uint32_t time_until_next = lv_timer_handler();
		if (time_until_next == LV_NO_TIMER_READY) {
			time_until_next = LV_DEF_REFR_PERIOD;
		}
		lv_delay_ms(time_until_next);
	}
	return 0;
}
