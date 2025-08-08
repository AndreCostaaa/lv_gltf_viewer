#include <lvgl.h>
#include <demos/lv_demos.h>
#include <examples/lv_examples.h>

#define WINDOW_WIDTH	    1280
#define WINDOW_HEIGHT	    1050

int main(void)
{
	/* initialize lvgl */
	lv_init();

	/* GLFW setup */
	lv_glfw_window_t *window = lv_glfw_window_create(WINDOW_WIDTH, WINDOW_HEIGHT, true);
	lv_display_t *display = lv_opengles_texture_create(WINDOW_WIDTH, WINDOW_HEIGHT);
	unsigned int texture_id = lv_opengles_texture_get_texture_id(display);
	lv_glfw_window_add_texture(window, texture_id, WINDOW_WIDTH, WINDOW_HEIGHT);


	/*lv_demo_gltf("A:gltfs/webp_diffuse_transmission_plant.glb");*/
	/*lv_example_gltf_1();*/
	lv_example_gltf_2();

	while (1) {
		uint32_t time_until_next = lv_timer_handler();
		if (time_until_next == LV_NO_TIMER_READY) {
			time_until_next = LV_DEF_REFR_PERIOD;
		}
		lv_delay_ms(time_until_next);
	}
	return 0;
}
