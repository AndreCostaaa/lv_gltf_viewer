#include "display/lv_display.h"
#include "misc/lv_timer.h"
#include "view/lv_gltf_view.h"
#include "view/sup/include/shader_includes.h"
#include "lib/lv_gl_shader/lv_gl_shader_manager.h"
#include "view/sup/include/shader_v1.h"
#include "widgets/3dtexture/lv_3dtexture.h"
#include <GL/glew.h> /* For window size restrictions */
#include <GLFW/glfw3.h> /* For window size / title */
#include <lib/lv_gl_shader/lv_gl_shader_manager_internal.h>
#include <lvgl.h>
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

	/* create a window and initialize OpenGL */
	lv_glfw_window_t *window =
		lv_glfw_window_create(WINDOW_WIDTH, WINDOW_HEIGHT, true);

	/* create a display that flushes to a texture */
	lv_display_t *texture =
		lv_opengles_texture_create(WINDOW_WIDTH, WINDOW_HEIGHT);

	/* add the texture to the window */
	unsigned int texture_id = lv_opengles_texture_get_texture_id(texture);
	lv_glfw_window_add_texture(window, texture_id, WINDOW_WIDTH,
				   WINDOW_HEIGHT);

	/* */
	lv_gl_shader_manager_t *shader_manager = lv_gl_shader_manager_create(
		src_includes, sizeof(src_includes) / sizeof(*src_includes),
		src_vertex(), src_frag());

	lv_gltf_view_t *demo_gltfview =
		(lv_gltf_view_t *)lv_malloc(get_viewer_datasize());
	init_viewer_struct(demo_gltfview);
	lv_gltf_view_set_width(demo_gltfview, WINDOW_WIDTH);
	lv_gltf_view_set_height(demo_gltfview, WINDOW_HEIGHT);

	lv_gl_shader_manager_env_textures_t env =
		lv_gltf_view_ibl_sampler_setup(NULL, NULL, 0);
	shader_manager->last_env = &env;

	lv_gltf_data_t *demo_gltfdata = lv_gltf_data_load_from_file(
		"gltfs/logo1.glb", shader_manager);
	lv_gltf_view_set_fov(demo_gltfview, 45);
	lv_3dtexture_id_t gltf_texture = lv_gltf_view_render(
		shader_manager, demo_gltfview, demo_gltfdata, true, 0, 0, 0, 0);

	lv_obj_t *gltfview_3dtex = lv_3dtexture_create(lv_screen_active());
	lv_3dtexture_set_src(gltfview_3dtex, gltf_texture);
	lv_3dtexture_set_flip(gltfview_3dtex, false, false);
	lv_obj_set_size(gltfview_3dtex, WINDOW_WIDTH, WINDOW_HEIGHT);
	lv_obj_align(gltfview_3dtex, LV_ALIGN_RIGHT_MID, 0, 0);

	while (1) {
		uint32_t time_until_next = lv_timer_handler();
		if (time_until_next == LV_NO_TIMER_READY) {
			time_until_next = LV_DEF_REFR_PERIOD;
		}
		lv_delay_ms(time_until_next);
	}

	// Cleanup
	lv_gltf_data_destroy(demo_gltfdata);
	lv_gltf_view_destroy(demo_gltfview);
	lv_gl_shader_manager_destroy(shader_manager);

	return 0;
}
