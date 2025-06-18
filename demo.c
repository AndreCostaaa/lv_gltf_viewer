#include "demo.h"

//#define EXPERIMENTAL_GROUNDCAST

float TIME_SCALE = 1.0f;

unsigned int _current_tab = 0;

lv_obj_t * tab_pages[MAX_TABS];

#define SYSTEM_ASSETS_FILENAME  "./gltfs/support_assets.glb"
#define TEMP_TEAPOT_FILENAME    "./gltfs/logo1.glb"

float spin_counter_degrees = 0.f;
float spin_rate = 0.f;
float anim_rate = 1.f;
int camera = -1;
int anim = -1;
float goal_pitch;
float goal_yaw;
float goal_distance;
float goal_focal_x;
float goal_focal_y;
float goal_focal_z;

bool use_scenecam = false;
bool anim_enabled = true;
bool animate_spin = true;
bool show_grid = true;
bool needs_system_gltfdata = false;
bool frame_grab_ui = false;
bool enable_intro_zoom = true;
bool reapply_layout_flag = true;
bool stub_mode = false;

#ifdef ENABLE_DESKTOP_MODE
float desktop_ratio = 0.5f;
bool desktop_mode = false;
#endif

ShaderCache_struct _shaderCache;
pShaderCache shaderCache = NULL;
gl_environment_textures _environment;

lv_gltfdata_t * system_gltfdata = NULL;
lv_gltfdata_t * demo_gltfdata = NULL;
lv_gltfview_t * demo_gltfview;
lv_obj_t * gltfview_3dtex;

lv_indev_t * mouse;
lv_glfw_window_t * window;
lv_display_t * display_texture;
lv_glfw_texture_t * window_texture;

pOverride ov_boom;
pOverride ov_stick;
pOverride ov_bucket;
pOverride ov_swing;
pOverride ov_cursor;
pOverride ov_cursor_scale;
pOverride ov_ground_scale;

double min(double a, double b) { return (a < b) ? a : b; }
uint32_t ui_max(uint32_t a, uint32_t b) { return (a > b) ? a : b; }
float lerp(float start, float end, float t) { return start + (end - start) * t; }

void setup_shadercache(const char * hdr_filepath, int degrees_x10 ) {
    _shaderCache = ShaderCache(src_includes, sizeof(src_includes)/sizeof(key_value), src_vertex(), src_frag() );
    shaderCache = &_shaderCache;
    lv_timer_handler();
    lv_task_handler();
    lv_refr_now(NULL);
    _environment = lv_gltfview_ibl_sampler_setup(NULL, hdr_filepath, degrees_x10  );
    _shaderCache.lastEnv = &_environment;
}

void demo_refocus(lv_gltfview_t * gltfview) {
    demo_ui_fill_in_InfoTab(demo_gltfdata);
    lv_gltfview_set_camera(gltfview, use_scenecam ? camera : -1);
    //lv_gltfview_set_focal_x(gltfview, 0.f);// Because this position is automatically determined the first time drawing an object, for now we explicitly do not set it here.
    //lv_gltfview_set_focal_y(gltfview, 0.f);
    //lv_gltfview_set_focal_z(gltfview, 0.f);
    lv_gltfview_reset_between_models(gltfview);
    lv_gltfview_recenter_view_on_model(demo_gltfview, demo_gltfdata);
    lv_gltfview_set_anim(gltfview, anim_enabled ? anim : -1);
    lv_gltfview_set_distance(gltfview, 1000);
    lv_gltfview_set_yaw(gltfview, 420 );
    lv_gltfview_set_pitch(gltfview, -200 );

    goal_pitch = lv_gltfview_get_pitch(gltfview);
    goal_yaw = lv_gltfview_get_yaw(gltfview);
    goal_distance = lv_gltfview_get_distance(gltfview);
    goal_focal_x = lv_gltfview_get_focal_x(gltfview);
    goal_focal_y = lv_gltfview_get_focal_y(gltfview);
    goal_focal_z = lv_gltfview_get_focal_z(gltfview);
    spin_counter_degrees = 0.f;
    if (enable_intro_zoom) lv_gltfview_set_distance(gltfview,  (int)((goal_distance*1.25f+0.1f)*1000.f) );
    demo_ui_apply_camera_button_visibility(demo_gltfdata);
    demo_ui_apply_anim_button_visibility(demo_gltfdata);
    demo_ui_apply_spin_rate_value(spin_rate);
    demo_ui_apply_spin_enabled_value(animate_spin);
    demo_ui_reposition_all();
}

void reload(char * _filename, const char * _hdr_filename) {
    printf("Loading %s...\n", _filename);

    if (!stub_mode) {
        lv_obj_clear_flag(grp_loading, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(gltfview_3dtex, LV_OBJ_FLAG_HIDDEN);
    }

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

    if (shaderCache == NULL) setup_shadercache(_hdr_filename, 1800);
    lv_gltfview_load(_filename, demo_gltfdata, shaderCache);

    if (lv_gltfview_get_probe(demo_gltfdata)->cameraCount == 0) {
        use_scenecam = false;
        camera = -1;            
    } else {
        use_scenecam = true;
        camera = 0;
    }
    if (lv_gltfview_get_probe(demo_gltfdata)->animationCount == 0) {
        anim_enabled = false;
        //anim = 0;
    } else {
        anim_enabled = true;
        anim = 0;
    }
    needs_system_gltfdata = show_grid;
    #ifdef EXPERIMENTAL_GROUNDCAST
        needs_system_gltfdata = true;
    #endif
    
    if (needs_system_gltfdata) {
        system_gltfdata = lv_malloc(get_gltf_datastruct_datasize() );
        lv_gltfview_load(SYSTEM_ASSETS_FILENAME, system_gltfdata, shaderCache);
        lv_gltfdata_link_view_to(system_gltfdata, demo_gltfdata);
        lv_gltfdata_copy_bounds_info(system_gltfdata, demo_gltfdata);
        float newradius = lv_gltf_get_int_radiusX1000(demo_gltfdata) / 1000.f;
        ov_ground_scale = lv_gltfview_add_override_by_id(system_gltfdata, "/grid", OP_SCALE, OMC_CHAN1 | OMC_CHAN2  | OMC_CHAN3);
        float unitscale = newradius * ((1.f / 2.f) * 3.f);
        float tscale = unitscale;
        if (!show_grid) {
            tscale = 0.f;
        }
        ov_ground_scale->data1 = tscale;
        ov_ground_scale->data2 = tscale;
        ov_ground_scale->data3 = tscale;

        tscale = unitscale / 8.f;
        ov_cursor_scale = lv_gltfview_add_override_by_id(system_gltfdata, "/cursor/visible", OP_SCALE, OMC_CHAN1 | OMC_CHAN2  | OMC_CHAN3);
        #ifndef EXPERIMENTAL_GROUNDCAST
        tscale = 0.f;
        #endif
        ov_cursor_scale->data1 = tscale;
        ov_cursor_scale->data2 = tscale;
        ov_cursor_scale->data3 = tscale;
    }
    lv_obj_add_flag(grp_loading, LV_OBJ_FLAG_HIDDEN);
    if (!stub_mode) lv_obj_clear_flag(gltfview_3dtex, LV_OBJ_FLAG_HIDDEN);
    demo_os_integrate_window_standard_title(_filename);
    demo_file_load_dialog_set_directory_from_filepath(_filename);
    demo_ui_set_tab(TAB_VIEW);
}

int main(int argc, char *argv[]) {
    demo_gltfview = lv_malloc(get_viewer_datasize() );
    init_viewer_struct(demo_gltfview);

    int lastMouseX = 0, lastMouseY = 0;
    int frameCount = 0;
    bool softwareOnly = false;
    bool startMaximized = false;
    char gltfFilePath[MAX_PATH_LENGTH] = {0};
    char hdrFilePath[MAX_PATH_LENGTH] = "media/hdr/directional.jpg";
    anim_rate = 1.0f;
    use_scenecam = true;
    camera = 0;

    if ( demo_cli_apply_commandline_options(demo_gltfview, gltfFilePath, hdrFilePath, &frameCount, &softwareOnly, &startMaximized, &stub_mode, &anim_rate, argc, argv) ) {

        if (softwareOnly) setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        lv_init();
        glfwInit();

        uint32_t max_window_width;
        uint32_t max_window_height;
        int maxFrames = frameCount;
        if (!demo_os_integrate_get_maximum_window_framebuffer_size(&max_window_width, &max_window_height)) startMaximized = false;

        #ifdef ENABLE_DESKTOP_MODE
        // Create the thread pool container
        pthread_t desktop_mode_worker_threads[MAX_THREADS];
        if (desktop_mode){
            for (int i = 0; i < MAX_THREADS; i++) pthread_create(&desktop_mode_worker_threads[i], NULL, demo_os_integrate_save_desktop_png_thread, NULL); // Start the thread pool
            startMaximized = false;
            lv_gltfview_set_width(demo_gltfview, ui_max( (INNER_BG_CROP_LEFT + INNER_BG_CROP_RIGHT) + 128, (int)((float)max_window_width * desktop_ratio) - (INNER_BG_CROP_LEFT + INNER_BG_CROP_RIGHT)));
            lv_gltfview_set_height(demo_gltfview, ui_max( (INNER_BG_CROP_TOP + INNER_BG_CROP_BOTTOM) + 128, (int)((float)max_window_height * desktop_ratio) - (INNER_BG_CROP_TOP + INNER_BG_CROP_BOTTOM)));
        } else 
        #endif 
        {
            if (startMaximized) {
                lv_gltfview_set_width(demo_gltfview, max_window_width - (INNER_BG_CROP_LEFT + INNER_BG_CROP_RIGHT));
                lv_gltfview_set_height(demo_gltfview, max_window_height - (INNER_BG_CROP_TOP + INNER_BG_CROP_BOTTOM));
            } else {
                lv_gltfview_set_width(demo_gltfview, (int)(max_window_width * 0.6f) - (INNER_BG_CROP_LEFT + INNER_BG_CROP_RIGHT));
                lv_gltfview_set_height(demo_gltfview, (int)(max_window_height * 0.8f) - (INNER_BG_CROP_TOP + INNER_BG_CROP_BOTTOM));
            }
        }

        /* create a window and initialize OpenGL */
        window = lv_glfw_window_create_ex( stub_mode ? STUB_WINDOW_WIDTH : max_window_width, stub_mode ? STUB_WINDOW_HEIGHT : max_window_height, true, false, false, MY_WINDOW_TITLE, "gltf-view-new");
        glfwHideWindow((GLFWwindow *)lv_glfw_window_get_glfw_window(window));
        
        /* create a display that flushes to a texture */
        display_texture = lv_opengles_texture_create(max_window_width, max_window_height);
        lv_display_set_default(display_texture);

        demo_os_integrate_setup_glfw_window(window, stub_mode, startMaximized);
        demo_ui_make_underlayer();

        /* add the texture to the window */
        window_texture = lv_glfw_window_add_texture(window, lv_opengles_texture_get_texture_id(display_texture), max_window_width, max_window_height);

        /* get the mouse indev of the window texture */
        mouse = lv_glfw_texture_get_mouse_indev(window_texture);
        gltfview_3dtex = lv_3dtexture_create(tab_pages[TAB_VIEW]);

        lv_3dtexture_id_t gltf_texture = 0;

        uint32_t clear_tex;
        GL_CALL(glCreateTextures(GL_TEXTURE_2D, 1, &clear_tex));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, clear_tex));
        unsigned char clearBytes[4] = {255, 0, 255, 255}; // RGBA format
        GL_CALL(glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0,  GL_RGBA8, GL_UNSIGNED_BYTE,  clearBytes ));
        // Set texture parameters (optional but recommended)
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        lv_3dtexture_set_src(gltfview_3dtex, clear_tex);

        lv_obj_set_size(gltfview_3dtex, max_window_width - (INNER_BG_CROP_LEFT + INNER_BG_CROP_RIGHT), max_window_height - (INNER_BG_CROP_TOP + INNER_BG_CROP_BOTTOM));   
        lv_obj_add_flag(gltfview_3dtex, LV_OBJ_FLAG_HIDDEN);
        lv_obj_align(gltfview_3dtex, LV_ALIGN_TOP_LEFT, INNER_BG_CROP_LEFT, INNER_BG_CROP_TOP);
        lv_obj_clear_flag(gltfview_3dtex, LV_OBJ_FLAG_CLICKABLE  );
        lv_3dtexture_set_src_flip(gltfview_3dtex, false, false);

        /* Create the loading info ui objects */
        demo_ui_loading_info_objects();

        /* Set the loading info update callbacks */
        lv_gltfview_ibl_set_loadphase_callback(demo_ui_load_progress_callback);
        lv_gltfview_set_loadphase_callback(demo_ui_load_progress_callback);

        demo_ui_pitch_yaw_distance_sliders(tab_pages[TAB_VIEW]);
        demo_ui_camera_select(tab_pages[TAB_VIEW]);
        demo_ui_animation_select(tab_pages[TAB_VIEW]);

        glfwShowWindow(glfw_window);
        if (startMaximized) glfwMaximizeWindow(glfw_window);

        reload(gltfFilePath, hdrFilePath);

        //lv_gltfview_set_recenter_flag(demo_gltfview, demo_gltfdata, true);
        lv_gltfview_recenter_view_on_model(demo_gltfview, demo_gltfdata);

        ov_boom = lv_gltfview_add_override_by_id(demo_gltfdata, "/root_base/base_platform/cab_pivot/proximal_armlink", OP_ROTATION, OMC_CHAN2);
        ov_stick = lv_gltfview_add_override_by_id(demo_gltfdata, "/root_base/base_platform/cab_pivot/proximal_armlink/distal_armlink", OP_ROTATION, OMC_CHAN2);
        ov_bucket = lv_gltfview_add_override_by_id(demo_gltfdata, "/root_base/base_platform/cab_pivot/proximal_armlink/distal_armlink/bucket", OP_ROTATION, OMC_CHAN2);  // Not currently valid even with the right model loaded
        ov_swing = lv_gltfview_add_override_by_id(demo_gltfdata, "/root_base/base_platform/cab_pivot", OP_ROTATION, OMC_CHAN1 | OMC_CHAN2 | OMC_CHAN3);
        if (needs_system_gltfdata) {
            ov_cursor = lv_gltfview_add_override_by_id(system_gltfdata, "/cursor", OP_POSITION, OMC_CHAN1 | OMC_CHAN2  | OMC_CHAN3);
        }
        if ((ov_boom != NULL) && (ov_stick != NULL) && (ov_swing != NULL) && (ov_cursor != NULL) ) demo_ui_add_override_controls(tab_pages[TAB_VIEW]);
        
        demo_ui_apply_camera_button_visibility(demo_gltfdata);
        demo_ui_apply_anim_button_visibility(demo_gltfdata);
        demo_ui_apply_spin_rate_value(spin_rate);
        demo_ui_apply_spin_enabled_value(animate_spin);

        demo_ui_fill_in_InfoTab(demo_gltfdata);

        if (lv_gltfview_get_probe(demo_gltfdata)->cameraCount == 0) {
            use_scenecam = false;
            camera = -1;            
        }

        if (lv_gltfview_get_probe(demo_gltfdata)->animationCount > 0) anim = 0;
        lv_gltfview_set_camera(demo_gltfview, use_scenecam ? camera : -1);
        lv_gltfview_set_focal_x(demo_gltfview, 0.f);// Because this position is automatically determined the first time drawing an object, for now we explicitly do not set it here.
        lv_gltfview_set_focal_y(demo_gltfview, 0.f);
        lv_gltfview_set_focal_z(demo_gltfview, 0.f);
        lv_gltfview_set_anim(demo_gltfview, anim_enabled ? anim : -1);

        lv_3dtexture_set_src(gltfview_3dtex, clear_tex);
        if (!stub_mode) {
            lv_obj_clear_flag(gltfview_3dtex, LV_OBJ_FLAG_HIDDEN);
            lv_obj_invalidate(gltfview_3dtex);
        }
        demo_os_integrate_window_standard_title(gltfFilePath);

        struct timeval start;
        lv_point_t _mousepoint;
        float MEASURED_CLOCKS_PER_SEC = 165000.0f;
        float ROLLING_FPS = -1.0f;
        long unsigned int frames_this_second = 0;
        long unsigned int frames_rendered_this_second = 0;
        long unsigned int ticks_this_second = 0;
        long unsigned int ticks = 0;
        unsigned long int usec_span = 0;
        unsigned long int usec_per_frame_optimal = 0;
        float seconds_this_second = 0.f;
        float total_seconds = 0.f;
        float cycle_seconds = fabs(spin_rate) > 0 ? 360.f /  fabs(spin_rate) : 0.f;
        float goal_fps = 15.0f;
        float goal_fps_span = 1.0f / goal_fps;
        time_t last_poll = time(0);
        clock_t last_clock = clock();
        #ifdef EXPERIMENTAL_GROUNDCAST 
        float _groundpos[3] = {0.f, 0.f, 0.f};
        #endif /* EXPERIMENTAL_GROUNDCAST */
        uint32_t cycle_frames = ui_max(1, (uint32_t)(cycle_seconds * goal_fps));
        //printf ("With a spin rate of %.2f degrees per second, and an FPS of %.2f, it would take %d frames to complete one cycle.\n", spin_rate, goal_fps, cycle_frames);

        bool _timing_break_flag = false;
        while (!_timing_break_flag) {
            time_t this_poll = time(0);
            #ifndef ENABLE_DESKTOP_MODE
            if (!desktop_mode) _timing_break_flag = true;
            #endif
            if (this_poll != last_poll) { _timing_break_flag = true; usleep(1000); }
            lv_refr_now(NULL);
            last_poll = this_poll;
        } 
        
        demo_ui_apply_camera_button_visibility(demo_gltfdata);
        demo_ui_apply_anim_button_visibility(demo_gltfdata);

        gettimeofday(&start, NULL);
        lv_indev_get_point(mouse, &_mousepoint);
        uint32_t totalframenum = 0;
        while(!demo_os_integrate_window_should_close()) {

            uint32_t ms_delay = lv_timer_handler();
            lv_task_handler();
            {
                struct pollfd fds[1];
                fds[0].fd = -1;     // No file descriptors to monitor
                fds[0].events = 0;  // No events to monitor
                poll(fds, 0, ms_delay);
            }
            float sec_span;
            
            if (frameCount > 0) {
                sec_span = 1.f / 30.f;
            } else {
                struct timeval stop;
                gettimeofday(&stop, NULL);
                usec_span = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
                start = stop;
                #ifdef ENABLE_DESKTOP_MODE
                    if (desktop_mode) {
                        long int thread_timing_delay = (usec_per_frame_optimal - usec_span)-1000;
                        long int fpslock_timing_delay = ((long unsigned int)(goal_fps_span * 1000000.f) - usec_span);
                        if (thread_timing_delay < fpslock_timing_delay) thread_timing_delay = fpslock_timing_delay;
                        if (thread_timing_delay > 1000) {
                            struct pollfd fds[1];
                            fds[0].fd = -1;     // No file descriptors to monitor
                            fds[0].events = 0;  // No events to monitor
                            // Wait for events with a timeout (e.g., 1000 ms)
                            poll(fds, 0, thread_timing_delay / 1000);
                            gettimeofday(&stop, NULL);
                            start = stop;
                        }  
                    }
                #endif
                sec_span = (float)usec_span / 1000000.0f;
            }
            seconds_this_second += sec_span;
            total_seconds += sec_span;
            
            //float windowed_seconds = cycle_seconds != 0.f ? total_seconds - ((int)(total_seconds / cycle_seconds) * cycle_seconds) : 0.f;
            uint32_t framenum = totalframenum % cycle_frames;

            float EASE_POWER = 0.5f;
            float EASE_CLOSE_ENOUGH = 0.001f;

            float tfx = lerp(lv_gltfview_get_focal_x(demo_gltfview), goal_focal_x, EASE_POWER / 8.f);
            if (fabsf(tfx - goal_focal_x) < (EASE_CLOSE_ENOUGH/100.f)) {tfx = goal_focal_x;}
            lv_gltfview_set_focal_x(demo_gltfview, tfx); 

            float tfy = lerp(lv_gltfview_get_focal_y(demo_gltfview), goal_focal_y, EASE_POWER / 8.f);
            if (fabsf(tfy - goal_focal_y) < (EASE_CLOSE_ENOUGH/100.f)) {tfy = goal_focal_y;}
            lv_gltfview_set_focal_y(demo_gltfview, tfy); 

            float tfz = lerp(lv_gltfview_get_focal_z(demo_gltfview), goal_focal_z, EASE_POWER / 8.f);
            if (fabsf(tfz - goal_focal_z) < (EASE_CLOSE_ENOUGH/100.f)) {tfz = goal_focal_z;}
            lv_gltfview_set_focal_z(demo_gltfview, tfz); 

            float tyaw = lerp(lv_gltfview_get_yaw(demo_gltfview), goal_yaw, EASE_POWER / 28.f);
            if (fabsf(tyaw - goal_yaw) < EASE_CLOSE_ENOUGH/100.f) {tyaw = goal_yaw;}
            bool looped = demo_ui_apply_yaw_value(tyaw );
            if (looped) {
                // Move the goal and lerped yaw back into the -180 <-> +180 degree range
                while (goal_yaw < -180.f) goal_yaw += 360.f;
                while (goal_yaw > 180.f) goal_yaw -= 360.f;
                while (tyaw < -180.f) tyaw += 360.f;
                while (tyaw > 180.f) tyaw -= 360.f;
            }
            lv_gltfview_set_yaw(demo_gltfview, (int)(tyaw * 10.f)); 

            float tpitch = lerp(lv_gltfview_get_pitch(demo_gltfview), goal_pitch, EASE_POWER / 28.f);
            if (fabsf(tpitch - goal_pitch) < EASE_CLOSE_ENOUGH/100.f) {tpitch = goal_pitch;}
            lv_gltfview_set_pitch(demo_gltfview, (int)(tpitch * 10.f)); demo_ui_apply_pitch_value(tpitch );

            float tdistance = lerp(lv_gltfview_get_distance(demo_gltfview), goal_distance, EASE_POWER / 8.f);
            if (fabsf(tdistance - goal_distance) < (EASE_CLOSE_ENOUGH/100.f)) {tdistance = goal_distance;}
            lv_gltfview_set_distance(demo_gltfview, (int)(tdistance * 1000.f)); demo_ui_apply_distance_value(tdistance );

            if (animate_spin) {
                #ifdef ENABLE_DESKTOP_MODE
                    spin_counter_degrees += (spin_rate * (desktop_mode?goal_fps_span:sec_span));
                #else
                    spin_counter_degrees += (spin_rate * sec_span);
                #endif
                lv_gltfview_set_spin_degree_offset(demo_gltfview, spin_counter_degrees);
            }

            lv_indev_get_point(mouse, &_mousepoint);

            int WINDOW_WIDTH_MINUS_MARGIN = ui_get_window_width()-WINDOW_CONTROL_MARGIN;
            int WINDOW_HEIGHT_MINUS_MARGIN = ui_get_window_height()-WINDOW_CONTROL_MARGIN;
            bool mouse_in_window = ((_mousepoint.x >= WINDOW_CONTROL_MARGIN) && (_mousepoint.x <= (WINDOW_WIDTH_MINUS_MARGIN)) && (_mousepoint.y >= WINDOW_CONTROL_MARGIN) && (_mousepoint.y <= (WINDOW_HEIGHT_MINUS_MARGIN)) );

            if (mouse_in_window) {
                lv_indev_state_t mouse_state = lv_indev_get_state(mouse);
                double subjectRadius = lv_gltf_get_int_radiusX1000(demo_gltfdata) / 1000.f;
                double movePow = min(subjectRadius, pow(subjectRadius, 0.5));
                if ((mouse_state & 0x0F) == LV_INDEV_STATE_PR) demo_nav_process_drag(movePow, (mouse_state & 0xF0), _mousepoint.x, _mousepoint.y, lastMouseX, lastMouseY);
            }

            #ifdef EXPERIMENTAL_GROUNDCAST
            if ((lastMouseX != _mousepoint.x) || (lastMouseY != _mousepoint.y)) lv_gltfview_mark_dirty(demo_gltfview);
            #endif

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
                ROLLING_FPS = (ROLLING_FPS > 0) ? ((ROLLING_FPS * 3.0f) + ((float)frames_this_second / (float)seconds_this_second) ) / 4.0f : ((float)frames_this_second / (float)seconds_this_second);
                seconds_this_second = 0.f;
                if (frames_rendered_this_second > 0) {
                    #ifndef NDEBUG
                        printf("[DEBUG BUILD] ");
                    #endif
                    printf("Frames Drawn: %ld | Average FPS: %2.1f | Ticks per sec: %2.1f\n", frames_rendered_this_second, ROLLING_FPS, MEASURED_CLOCKS_PER_SEC);
                }
                ticks_this_second = 0;
                frames_this_second = 0;
                frames_rendered_this_second = 0;
                usec_per_frame_optimal = (int)(1000000.f / ROLLING_FPS);
            }

            lv_gltfview_set_timestep(demo_gltfview, anim_enabled ? sec_span * anim_rate : 0.f );
            last_poll = this_poll;

            #ifdef EXPERIMENTAL_GROUNDCAST
            if (mouse_in_window) {
                bool _res = lv_gltfview_raycast_ground_position(demo_gltfview, _mousepoint.x - INNER_BG_CROP_LEFT, _mousepoint.y - INNER_BG_CROP_TOP, ui_get_window_width() - (INNER_BG_CROP_LEFT + INNER_BG_CROP_RIGHT), ui_get_window_height() - (INNER_BG_CROP_TOP + INNER_BG_CROP_BOTTOM),  0.0, _groundpos);
                if (_res && (ov_cursor != NULL)) {
                    ov_cursor->data1 = _groundpos[0];
                    ov_cursor->data2 = _groundpos[1];
                    ov_cursor->data3 = _groundpos[2];
                }
            }
            #endif /* EXPERIMENTAL_GROUNDCAST */

            #ifdef ENABLE_DESKTOP_MODE
            if (desktop_mode && (totalframenum != framenum)) {
                // Do nothing, frame is cached
            } else
            #endif
            { 
                gltf_texture = lv_gltfview_render( shaderCache, demo_gltfview, demo_gltfdata, true, 0, 0, 0, 0 );
                if (needs_system_gltfdata && (use_scenecam == false)) {
                    if (!lv_gltfview_check_frame_was_cached(demo_gltfview)) {
                        gltf_texture = lv_gltfview_render( shaderCache, demo_gltfview, system_gltfdata, false, 0,0,0,0);
                    }
                }
            }
            if (reapply_layout_flag) demo_ui_reposition_all();

            if (!lv_gltfview_check_frame_was_cached(demo_gltfview)) {
                frames_rendered_this_second += 1;
                if (!stub_mode) {
                    lv_3dtexture_set_src(gltfview_3dtex, gltf_texture);
                    lv_obj_invalidate(gltfview_3dtex);
                    lv_refr_now(NULL);
                }
                glfwPollEvents();

                bool file_alpha = lv_gltfview_get_bg_mode(demo_gltfview) != BG_ENVIRONMENT;
                #ifdef ENABLE_DESKTOP_MODE
                if (desktop_mode ){
                    if (totalframenum != framenum) {
                        demo_os_integrate_save_png_from_new_thread(framenum, desktop_mode, maxFrames, file_alpha, NULL);
                    } else {
                        //printf ("Currently on desktop frame #%05d\n", framenum);
                        char * pixels =(char *)lv_malloc(ui_get_primary_texture_height() * ui_get_primary_texture_width() * 4);
                        lv_gltfview_utils_get_capture_buffer( pixels, demo_gltfview, gltf_texture, file_alpha, lv_gltfview_check_frame_was_antialiased(demo_gltfview) ? 1 : 0, ui_get_primary_texture_width(), ui_get_primary_texture_height() );
                        demo_os_integrate_save_png_from_new_thread(framenum, desktop_mode, maxFrames, file_alpha, pixels);
                    }
                } else 
                #endif
                {
                    if (frameCount > 0) {
                        char _buffer[100];
                        snprintf(_buffer, sizeof(_buffer), "/home/pi/Desktop/lv_gltf_viewer/render_frames/frame%03d.png", (maxFrames - frameCount));
                        if (frame_grab_ui) {
                            lv_gltfview_utils_save_texture_to_png( demo_gltfview, lv_opengles_texture_get_texture_id(display_texture), _buffer, false, 10, 0, ui_get_window_width(),  ui_get_window_height() );
                        } else {
                            lv_gltfview_utils_save_png(demo_gltfview, _buffer, file_alpha, 10);
                        }
                    }
                } 
            } else {
                glfwPollEvents();
                usleep(33000);
            }

            totalframenum += 1;
            if (frameCount > 0) {
                frameCount -= 1;
                if (frameCount == 0) {
                    demo_os_integrate_signal_window_close();
                }
            }
        }

        if (needs_system_gltfdata) lv_gltfdata_destroy(system_gltfdata);
        lv_gltfdata_destroy(demo_gltfdata);
        lv_gltfview_destroy(demo_gltfview);
        lv_gltfview_shadercache_destroy(shaderCache);

        #ifdef ENABLE_DESKTOP_MODE
        if (desktop_mode) {
            running = false;
            for (int i = 0; i < MAX_THREADS; i++) pthread_join(desktop_mode_worker_threads[i], NULL);
        }
        #endif
    }
    lv_free(demo_gltfview);
    #ifndef NDEBUG
        printf("Note: Because this is a debug build, it is normal for there to be a significant delay when closing the application.  To avoid this in the future, rebuild in release mode by running the switch_release.sh script in the project's /ex folder.\n");
    #endif
    exit(0);
}