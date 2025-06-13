#include "demo.h"

//#define EXPERIMENTAL_GROUNDCAST

float TIME_SCALE = 1.0f;

unsigned int _current_tab = 0;

lv_obj_t * tab_pages[MAX_TABS];

const char* MY_WINDOW_TITLE = "glTF Viewer [ LVGL.io ]";

float spin_counter_degrees = 0.f;
float spin_rate = 0.f;
float anim_rate = 1.f;
int camera = -1;
int anim = -1;

bool use_scenecam = false;
bool anim_enabled = true;
bool animate_spin = true;
bool desktop_mode = false;

ShaderCache_struct _shaderCache;
pShaderCache shaderCache;
gl_environment_textures _environment;

lv_gltfdata_t * demo_gltfdata;
lv_gltfview_t * demo_gltfview;
lv_obj_t * gltfview_3dtex;

lv_indev_t * mouse;
lv_glfw_window_t * window;
lv_display_t * display_texture;
lv_glfw_texture_t * window_texture;

//GLFWwindow * glfw_window;

pOverride ov_boom;
pOverride ov_stick;
pOverride ov_bucket;
pOverride ov_swing;
pOverride ov_cursor;

double min(double a, double b) { return (a < b) ? a : b; }

void setup_shadercache(const char * hdr_filepath, int degrees_x10 ) {
    _shaderCache = ShaderCache(src_includes, sizeof(src_includes)/sizeof(key_value), src_vertex(), src_frag() );
    shaderCache = &_shaderCache;
    _environment = lv_gltfview_ibl_sampler_setup(NULL, hdr_filepath, degrees_x10  );
    _shaderCache.lastEnv = &_environment;
}

void reload(const char * _filename, const char * _hdr_filename) {

    printf("Loading ");
    printf(_filename);
    printf("...\n");

    const long unsigned int _MAX_FILENAME_SIZE = 64;
    char * FILENAME_BUFF = lv_malloc(_MAX_FILENAME_SIZE);
    char buffer[255];
    lv_gltf_get_isolated_filename(_filename, FILENAME_BUFF, _MAX_FILENAME_SIZE);
    lv_label_set_text(titleText, FILENAME_BUFF);
    sprintf(buffer, "Loading: %s | %s", FILENAME_BUFF, MY_WINDOW_TITLE);
    lv_glfw_window_set_title(window, buffer);
    lv_free(FILENAME_BUFF);
    
    lv_obj_clear_flag(grp_loading, LV_OBJ_FLAG_HIDDEN);

    demo_gltfdata = lv_malloc(get_gltf_datastruct_datasize() );
    
    lv_timer_handler();
    lv_task_handler();
    setup_shadercache(_hdr_filename, 1800);

    lv_gltfview_load(_filename, demo_gltfdata, demo_gltfview, shaderCache);
}

int main(int argc, char *argv[]) {
    demo_gltfview = lv_malloc(get_viewer_datasize() );
    init_viewer_struct(demo_gltfview);
    int lastMouseX = 0, lastMouseY = 0;
    int frameCount = 0;
    bool softwareOnly = false;

    anim_rate = 1.0f;
    char gltfFilePath[MAX_PATH_LENGTH] = {0};
    char hdrFilePath[MAX_PATH_LENGTH] = "media/hdr/directional.jpg";

    // Set the defaults
    lv_gltfview_set_env_pow(demo_gltfview, 1.8f );
    lv_gltfview_set_exposure(demo_gltfview, 0.8f );
    lv_gltfview_set_distance(demo_gltfview, 1000);
    lv_gltfview_set_yaw(demo_gltfview, 420 );
    lv_gltfview_set_pitch(demo_gltfview, -200 );
    lv_gltfview_set_blur_bg(demo_gltfview, 0.25f );
    lv_gltfview_set_aa_mode(demo_gltfview, ANTIALIAS_NOT_MOVING );
    lv_gltfview_set_bg_mode(demo_gltfview, BG_CLEAR);
    lv_gltfview_set_width(demo_gltfview, BIG_TEXTURE_WIDTH);
    lv_gltfview_set_height(demo_gltfview, BIG_TEXTURE_HEIGHT);

    if ( demo_cli_apply_commandline_options(demo_gltfview, gltfFilePath, hdrFilePath, &frameCount, &softwareOnly, &anim_rate, argc, argv) ) {
        // Output the parsed parameters
        printf("glTF File Path: %s\n", gltfFilePath[0] ? gltfFilePath : "None provided");
        printf("HDR File Path: %s\n", hdrFilePath[0] ? hdrFilePath : "None provided");
        printf("Antialiasing Mode: %d\n", lv_gltfview_get_aa_mode(demo_gltfview));
        printf("Background Mode: %d\n",  lv_gltfview_get_bg_mode(demo_gltfview));
        printf("Pitch: %.2f\n", lv_gltfview_get_pitch(demo_gltfview));
        printf("Yaw: %.2f\n", lv_gltfview_get_yaw(demo_gltfview));
        printf("Frame Count: %d\n", frameCount);
        printf("Spin Rate (degrees per sec): %.3f\n", spin_rate);
        printf("Software Only: %s\n", softwareOnly ? "true" : "false");
        printf("Desktop Mode: %s\n", desktop_mode ? "true" : "false");

        int maxFrames = frameCount;
        bool stubmode = desktop_mode | (frameCount > 0);
        lv_init();

        if (softwareOnly) setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);

        uint32_t max_window_width = ui_get_max_window_width();
        uint32_t max_window_height = ui_get_max_window_height();

        glfwInit();
        // Get the primary monitor
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            // Get the work area of the monitor
            int xpos, ypos, width, height;
            glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &width, &height);
            max_window_width = width;
            max_window_height = height;
            // Print the maximum window size
            printf("Maximum window size: %dx%d\n", width, height);
        }


        /* create a window and initialize OpenGL */
        window = lv_glfw_window_create_ex( stubmode ? STUB_WINDOW_WIDTH : max_window_width, stubmode ? STUB_WINDOW_HEIGHT : max_window_height, true, false, false, MY_WINDOW_TITLE, "gltf-view-new");
        demo_os_integrate_setup_glfw_window(window, stubmode);

        /* create a display that flushes to a texture */
        display_texture = lv_opengles_texture_create(max_window_width, max_window_height);
        lv_display_set_default(display_texture);

        demo_ui_make_underlayer();

        /* add the texture to the window */
        window_texture = lv_glfw_window_add_texture(window, lv_opengles_texture_get_texture_id(display_texture), max_window_width, max_window_height);
        
        /* get the mouse indev of the window texture */
        mouse = lv_glfw_texture_get_mouse_indev(window_texture);

        lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE );

        titleText = lv_label_create(lv_screen_active());
        lv_obj_align(titleText, LV_ALIGN_TOP_LEFT, 70, 10);
        lv_obj_set_style_text_opa(titleText, LV_OPA_50, LV_PART_MAIN);
        lv_obj_set_style_text_color(titleText, lv_color_hex(LVGL_BLUE), LV_PART_MAIN);
        lv_label_set_text(titleText, "3D Models");

        demo_ui_loading_info_objects();
        lv_gltfview_ibl_set_loadphase_callback(demo_ui_load_progress_callback);
        lv_gltfview_set_loadphase_callback(demo_ui_load_progress_callback);

        gltfview_3dtex = lv_3dtexture_create(tab_pages[TAB_VIEW]);
        lv_obj_set_size(gltfview_3dtex, max_window_width, max_window_height);   
        lv_obj_add_flag(gltfview_3dtex, LV_OBJ_FLAG_HIDDEN);
        lv_obj_align(gltfview_3dtex, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_obj_clear_flag(gltfview_3dtex, LV_OBJ_FLAG_CLICKABLE  );
        lv_3dtexture_set_src_flip(gltfview_3dtex, false, false);

        demo_ui_pitch_yaw_distance_sliders(tab_pages[TAB_VIEW]);
        demo_ui_camera_select(tab_pages[TAB_VIEW]);
        demo_ui_animation_select(tab_pages[TAB_VIEW]);

        lv_timer_handler();
        lv_task_handler();
        reload(gltfFilePath, hdrFilePath);

        ov_boom = lv_gltfview_add_override_by_id(demo_gltfdata, "/root_base/base_platform/cab_pivot/proximal_armlink", OP_ROTATION, OMC_CHAN2);
        ov_stick = lv_gltfview_add_override_by_id(demo_gltfdata, "/root_base/base_platform/cab_pivot/proximal_armlink/distal_armlink", OP_ROTATION, OMC_CHAN2);
        ov_bucket = lv_gltfview_add_override_by_id(demo_gltfdata, "/root_base/base_platform/cab_pivot/proximal_armlink/distal_armlink/bucket", OP_ROTATION, OMC_CHAN2);  // Not currently valid even with the right model loaded
        ov_swing = lv_gltfview_add_override_by_id(demo_gltfdata, "/root_base/base_platform/cab_pivot", OP_ROTATION, OMC_CHAN1 | OMC_CHAN2 | OMC_CHAN3);
        ov_cursor = lv_gltfview_add_override_by_id(demo_gltfdata, "/cursor", OP_POSITION, OMC_CHAN1 | OMC_CHAN2  | OMC_CHAN3);

        if ((ov_boom != NULL) && (ov_stick != NULL) && (ov_swing != NULL) && (ov_cursor != NULL) ) demo_ui_add_override_controls(tab_pages[TAB_VIEW]);
        
        demo_ui_apply_camera_button_visibility(demo_gltfdata);
        demo_ui_apply_anim_button_visibility(demo_gltfdata);
        demo_ui_fill_in_InfoTab(demo_gltfdata);

        if (lv_gltfview_get_probe(demo_gltfdata)->cameraCount > 0) {
            use_scenecam = true;
            camera = 0;
        }

        if (lv_gltfview_get_probe(demo_gltfdata)->animationCount > 0) anim = 0;
        lv_gltfview_set_camera(demo_gltfview, use_scenecam ? camera : -1);
        lv_gltfview_set_focal_x(demo_gltfview, 0.f);// Because this position is automatically determined the first time drawing an object, for now we explicitly do not set it here.
        lv_gltfview_set_focal_y(demo_gltfview, 0.f);
        lv_gltfview_set_focal_z(demo_gltfview, 0.f);
        lv_gltfview_set_anim(demo_gltfview, anim_enabled ? anim : -1);
        lv_refr_now(NULL);

        lv_3dtexture_id_t gltf_texture = lv_gltfview_render( shaderCache, demo_gltfview, demo_gltfdata );
        lv_3dtexture_set_src(gltfview_3dtex, gltf_texture);

        lv_obj_add_flag(grp_loading, LV_OBJ_FLAG_HIDDEN);
        if (!stubmode) {
            lv_obj_clear_flag(gltfview_3dtex, LV_OBJ_FLAG_HIDDEN);
            lv_obj_invalidate(gltfview_3dtex);
        }
        glfwPollEvents();
        lv_refr_now(NULL);

        {
            char buffer[255];
            const unsigned int _MAX_FILENAME_SIZE = 64;
            char * FILENAME_BUFF = lv_malloc(_MAX_FILENAME_SIZE);
            lv_gltf_get_isolated_filename(gltfFilePath, FILENAME_BUFF, _MAX_FILENAME_SIZE);
            sprintf(buffer, "%s | %s", FILENAME_BUFF, MY_WINDOW_TITLE);
            lv_glfw_window_set_title(window, buffer);
            lv_free(FILENAME_BUFF);
        }

        const int WINDOW_CONTROL_MARGIN = 80; 

        struct timeval start;
        lv_point_t _mousepoint;

        float MEASURED_CLOCKS_PER_SEC = 165000.0f;
        float ROLLING_FPS = 0.0f;
        long unsigned int frames_this_second = 0;
        long unsigned int frames_rendered_this_second = 0;
        long unsigned int ticks_this_second = 0;
        long unsigned int ticks = 0;
        unsigned long int usec_span = 0;

        #ifdef EXPERIMENTAL_GROUNDCAST 
        float _groundpos[3] = {0.f, 0.f, 0.f};
        #endif /* EXPERIMENTAL_GROUNDCAST */

        time_t last_poll = time(0);
        clock_t last_clock = clock();
        double subjectRadius = lv_gltf_get_int_radiusX1000(demo_gltfdata) / 1000.f;
        double movePow = min(subjectRadius, pow(subjectRadius, 0.5));
        //char * pixels = lv_malloc(BIG_TEXTURE_HEIGHT * BIG_TEXTURE_WIDTH * 4);

        {   
            bool _timing_break_flag = false;
            while (!_timing_break_flag) {
                time_t this_poll = time(0);
                _timing_break_flag = true;
                lv_refr_now(NULL);
                last_poll = this_poll;
            } 
        }

        gettimeofday(&start, NULL);
        lv_indev_get_point(mouse, &_mousepoint);

        while(!demo_os_integrate_window_should_close()) {
            uint32_t ms_delay = lv_timer_handler();
            
            lv_task_handler();
            usleep(ms_delay * 1000);

            float sec_span;
            if (frameCount > 0) {
                sec_span = 1.f / 30.f;
            } else {
                struct timeval stop;
                gettimeofday(&stop, NULL);
                usec_span = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
                sec_span = (float)usec_span / 1000000.0f;
                start = stop;
            }

            if (animate_spin) {
                spin_counter_degrees += (spin_rate * sec_span);
                lv_gltfview_set_spin_degree_offset(demo_gltfview, spin_counter_degrees);
            }


            lv_indev_get_point(mouse, &_mousepoint);
            
            int WINDOW_WIDTH_MINUS_MARGIN = ui_get_window_width()-WINDOW_CONTROL_MARGIN;
            int WINDOW_HEIGHT_MINUS_MARGIN = ui_get_window_height()-WINDOW_CONTROL_MARGIN;
            bool mouse_in_window = ((_mousepoint.x >= WINDOW_CONTROL_MARGIN) && (_mousepoint.x <= (WINDOW_WIDTH_MINUS_MARGIN)) && (_mousepoint.y >= WINDOW_CONTROL_MARGIN) && (_mousepoint.y <= (WINDOW_HEIGHT_MINUS_MARGIN)) );

            if (mouse_in_window) {

                #ifdef EXPERIMENTAL_GROUNDCAST
                bool _res = lv_gltfview_raycast_ground_position(demo_gltfview, _mousepoint.x, _mousepoint.y, ui_get_window_width(), ui_get_window_height(),  1.0, _groundpos);
                if (_res && (ov_cursor != NULL)) {
                    ov_cursor->data1 = _groundpos[0];
                    ov_cursor->data2 = _groundpos[1];
                    ov_cursor->data3 = _groundpos[2];
                }
                #endif /* EXPERIMENTAL_GROUNDCAST */

                lv_indev_state_t mouse_state = lv_indev_get_state(mouse);
                if ((mouse_state & 0x0F) == LV_INDEV_STATE_PR) demo_nav_process_drag(movePow, (mouse_state & 0xF0), _mousepoint.x, _mousepoint.y, lastMouseX, lastMouseY);
            }

            lastMouseX = _mousepoint.x;
            lastMouseY = _mousepoint.y;
            
            lv_gltfview_set_camera(demo_gltfview, use_scenecam ? camera : -1);
            lv_gltfview_set_anim(demo_gltfview, anim_enabled ? anim : -1);

            time_t this_poll = time(0);
            clock_t this_clock = clock();
            ticks = (long unsigned int)(this_clock - last_clock);
            last_clock = this_clock;
            ticks_this_second += ticks;
            frames_this_second += 1;
            bool seconds_changed = difftime(this_poll, last_poll) > 0;
            if (seconds_changed) {
                MEASURED_CLOCKS_PER_SEC = ((MEASURED_CLOCKS_PER_SEC * 3.0f) + ((float)ticks_this_second)) / 4.0f;
                ROLLING_FPS = ((ROLLING_FPS * 3.0f) + ((float)frames_this_second)) / 4.0f;
                if (frames_rendered_this_second > 0) {
                    #ifndef NDEBUG
                        printf("[DEBUG BUILD] ");
                    #endif
                    printf("Frames Drawn: %ld | Average FPS: %2.1f | Ticks per sec: %2.1f\n", frames_rendered_this_second, ROLLING_FPS, MEASURED_CLOCKS_PER_SEC);
                }
                ticks_this_second = 0;
                frames_this_second = 0;
                frames_rendered_this_second = 0;
            }

            lv_gltfview_set_timestep(demo_gltfview, anim_enabled ? sec_span * anim_rate : 0.f );
            last_poll = this_poll;
            gltf_texture = lv_gltfview_render( shaderCache, demo_gltfview, demo_gltfdata );
            if (!lv_gltfview_check_frame_was_cached(demo_gltfview)) {
                frames_rendered_this_second += 1;
                if (!stubmode) {
                    lv_3dtexture_set_src(gltfview_3dtex, gltf_texture);
                    lv_obj_invalidate(gltfview_3dtex);
                }
                glfwPollEvents();
                lv_refr_now(NULL);

                bool file_alpha = lv_gltfview_get_bg_mode(demo_gltfview) != BG_ENVIRONMENT;

                if (desktop_mode){
                    lv_gltfview_utils_save_png(demo_gltfview, DESKTOP_OUTPUT_FILEPATH, file_alpha, 0);
                    system(DESKTOP_APPLY_COMMAND);
                } else {
                    if (frameCount > 0) {
                        char _buffer[100];
                        snprintf(_buffer, sizeof(_buffer), "/home/pi/Desktop/lv_gltf_viewer/render_frames/frame%03d.png", (maxFrames - frameCount));
                        lv_gltfview_utils_save_png(demo_gltfview, _buffer, file_alpha, 10);
                    }
                }
            } else {
                usleep(30000);
            }
            glfwPollEvents();

            if (frameCount > 0) {
                frameCount -= 1;
                if (frameCount == 0) {
                    demo_os_integrate_signal_window_close();
                }
            }
        }
        
        lv_obj_clear_flag(grp_loading, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(gltfview_3dtex, LV_OBJ_FLAG_HIDDEN);
        demo_ui_load_progress_callback("Closing Application", "", 0.f, 0.f, 0.f, 0.f);
        usleep(20 * 1000);
        
        lv_obj_invalidate(grp_loading);
        lv_refr_now(NULL);
        lv_timer_handler();
        lv_task_handler();
        
        lv_gltfview_destroy(demo_gltfview, demo_gltfdata, shaderCache);
        //lv_free(pixels);
    }
    lv_free(demo_gltfview);
    exit(0);
}