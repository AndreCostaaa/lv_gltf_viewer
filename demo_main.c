

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "demo.h"

#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

unsigned int get_gltf_datastruct_datasize(void);
unsigned int get_viewer_datasize(void);
unsigned int get_primitive_datasize(void);

//#define SOFTWARE_ONLY
//#define EXPERIMENTAL_DESKTOP

float TIME_SCALE = 1.0f;

unsigned int _current_tab = 0;

lv_obj_t * tab_pages[MAX_TABS];

const char* MY_WINDOW_TITLE = "glTF Viewer [ LVGL.io ]";

float elevation = 0.0f;
float spin_rate = 0.f;
float anim_rate = 1.f;
//float exposure = 1.0f;
int camera = -1;
int anim = -1;

bool use_scenecam = false;
bool anim_enabled = true;
bool animate_spin = true;
bool camselection_changed = true;
bool animselection_changed = true;

float _norm_mouse_x = 0.f;
float _norm_mouse_y = 0.f;

ShaderCache_struct _shaderCache;
pShaderCache shaderCache;
gl_environment_textures _environment;
pGltf_data_t _model_data;
pViewer _viewer;

GLFWwindow * glfw_window;

pOverride ov_boom;
pOverride ov_stick;
pOverride ov_bucket;
pOverride ov_swing;
pOverride ov_cursor;

float spin_counter_degrees = 0.f;

double min(double a, double b) { return (a < b) ? a : b; }

bool setIBLLoadPhaseCallback(void (*_load_progress_callback)(const char*, const char* , float, float, float, float));
gl_environment_textures setup_environment(gl_environment_textures* _lastEnv, const char* _env_filename, int _env_angle_degreesX10  );

void demo_load_progress_callback(const char* phase_title, const char* sub_phase_title, float phase_progress, float phase_progress_max, float sub_phase_progress, float sub_phase_progress_max)
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

void filedrop_cb(GLFWwindow* _window, int count, const char** paths)
{
    LV_UNUSED(_window);
    int i;
    for (i = 0;  i < count;  i++) {
        printf("file dropped into window: %s\n", paths[i]);
    }
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

void setup_glfw_window(GLFWwindow * _window) {
    glfwSetWindowCloseCallback(_window, window_close_callback);
    glfwSetWindowSizeLimits(_window, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetWindowAttrib(_window, GLFW_RESIZABLE, false);
    glfwSetWindowAspectRatio(_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetDropCallback(_window, filedrop_cb);
}

void setup_shadercache(const char * hdr_filepath, int degrees_x10 ) {
    _shaderCache = ShaderCache(src_includes, sizeof(src_includes)/sizeof(key_value), src_vertex(), src_frag() );
    shaderCache = &_shaderCache;
    _environment = setup_environment(NULL, hdr_filepath, degrees_x10  );
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

    _model_data = lv_malloc(get_gltf_datastruct_datasize() );
    
    lv_timer_handler();
    lv_task_handler();
    setup_shadercache(_hdr_filename, 1800);

    lv_gltfview_load(_filename, _model_data, _viewer, shaderCache);
}

int main(int argc, char *argv[])
{
    _viewer = lv_malloc(get_viewer_datasize() );
    init_viewer_struct(_viewer);

    int lastMouseX = 0, lastMouseY = 0;
    int frameCount = 0;

    anim_rate = 1.0f;
    char gltfFilePath[MAX_PATH_LENGTH] = {0};
    char hdrFilePath[MAX_PATH_LENGTH] = "media/hdr/directional.jpg";

    // Set the defaults
    lv_gltfview_set_env_pow(_viewer, 1.8f );
    lv_gltfview_set_exposure(_viewer, 0.8f );
    lv_gltfview_set_distance(_viewer, 1000);
    lv_gltfview_set_yaw(_viewer, 420 );
    lv_gltfview_set_pitch(_viewer, -200 );
    lv_gltfview_set_blur_bg(_viewer, 0.25f );
    lv_gltfview_set_aa_mode(_viewer, ANTIALIAS_NOT_MOVING );
    lv_gltfview_set_bg_mode(_viewer, BG_CLEAR);
    
    if ( apply_commandline_options(_viewer, gltfFilePath, hdrFilePath, &frameCount, &anim_rate, argc, argv) ) {
        // Output the parsed parameters
        printf("glTF File Path: %s\n", gltfFilePath[0] ? gltfFilePath : "None provided");
        printf("HDR File Path: %s\n", hdrFilePath[0] ? hdrFilePath : "None provided");
        printf("Antialiasing Mode: %d\n", lv_gltfview_get_aa_mode(_viewer));
        printf("Background Mode: %d\n",  lv_gltfview_get_bg_mode(_viewer));
        printf("Pitch: %.2f\n", lv_gltfview_get_pitch(_viewer));
        printf("Yaw: %.2f\n", lv_gltfview_get_yaw(_viewer));
        printf("Frame Count: %d\n", frameCount);
        printf("Spin Rate (degrees per sec): %.3f\n", spin_rate);

        int maxFrames = frameCount;
        lv_init();

        // To force software rendering, find the #define SOFTWARE_ONLY line 
        // that's commented out at start of this file and uncomment it
        #ifdef SOFTWARE_ONLY
        setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
        #endif

        /* create a window and initialize OpenGL */
        window = lv_glfw_window_create_ex(WINDOW_WIDTH, WINDOW_HEIGHT, true, false, false, MY_WINDOW_TITLE, "gltf-view-new");
        lv_glfw_window_set_flip(window, false, false);
        glfw_window = (GLFWwindow *)lv_glfw_window_get_glfw_window(window);
        setup_glfw_window(glfw_window);

        GLFWimage images[1]; 
        int width, height, channel;
        stbi_uc* img = stbi_load("./window_icon_32px.png", &width, &height, &channel, 0); //rgba channels 
        if (img == NULL) printf("Icon Can Not Be Loaded\n");
        images[0].height = 32;
        images[0].width = 32;
        images[0].pixels = img;
        glfwSetWindowIcon(glfw_window, 1, images);
        glfwMakeContextCurrent(glfw_window);
        
        /* create a display that flushes to a texture */
        lv_display_t * texture = lv_opengles_texture_create(WINDOW_WIDTH, WINDOW_HEIGHT);
        lv_display_set_default(texture);

        lv_main_tabview();
        set_tab(TAB_VIEW);
        __make_ViewTab();
        __make_InfoTab();
        __make_WindowBevelsAndIcon();

        /* add the texture to the window */
        unsigned int display_texture = lv_opengles_texture_get_texture_id(texture);
        lv_glfw_texture_t * window_texture = lv_glfw_window_add_texture(window, display_texture, WINDOW_WIDTH, WINDOW_HEIGHT);

        /* get the mouse indev of the window texture */
        lv_indev_t * mouse = lv_glfw_texture_get_mouse_indev(window_texture);

        lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE );

        titleText = lv_label_create(lv_screen_active());
        lv_obj_align(titleText, LV_ALIGN_TOP_LEFT, 70, 10);
        lv_obj_set_style_text_opa(titleText, LV_OPA_50, LV_PART_MAIN);
        lv_obj_set_style_text_color(titleText, lv_color_hex(LVGL_BLUE), LV_PART_MAIN);
        lv_label_set_text(titleText, "3D Models");

        lv_loading_info_objects();
        setIBLLoadPhaseCallback(demo_load_progress_callback);
        lv_gltfview_set_load_phase_callback(demo_load_progress_callback);

        lv_obj_t * tex = lv_3dtexture_create(tab_pages[TAB_VIEW]);
        lv_obj_set_size(tex, BIG_TEXTURE_WIDTH, BIG_TEXTURE_HEIGHT);   
        lv_obj_add_flag(tex, LV_OBJ_FLAG_HIDDEN);
        lv_obj_align(tex, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_obj_clear_flag(tex, LV_OBJ_FLAG_CLICKABLE  );
        lv_3dtexture_set_src_flip(tex, false, false);

        lv_pitch_yaw_distance_sliders(tab_pages[TAB_VIEW]);
        lv_camera_select(tab_pages[TAB_VIEW]);
        lv_animation_select(tab_pages[TAB_VIEW]);

        lv_timer_handler();
        lv_task_handler();
        reload(gltfFilePath, hdrFilePath);

        ov_boom = lv_gltfview_add_override_by_id(_model_data, "/root_base/base_platform/cab_pivot/proximal_armlink", OP_ROTATION, OMC_CHAN2);
        ov_stick = lv_gltfview_add_override_by_id(_model_data, "/root_base/base_platform/cab_pivot/proximal_armlink/distal_armlink", OP_ROTATION, OMC_CHAN2);
        ov_bucket = lv_gltfview_add_override_by_id(_model_data, "/root_base/base_platform/cab_pivot/proximal_armlink/distal_armlink/bucket", OP_ROTATION, OMC_CHAN2);  // Not currently valid even with the right model loaded
        ov_swing = lv_gltfview_add_override_by_id(_model_data, "/root_base/base_platform/cab_pivot", OP_ROTATION, OMC_CHAN1 | OMC_CHAN2 | OMC_CHAN3);
        ov_cursor = lv_gltfview_add_override_by_id(_model_data, "/cursor", OP_POSITION, OMC_CHAN1 | OMC_CHAN2  | OMC_CHAN3);

        if ((ov_boom != NULL) && (ov_stick != NULL) && (ov_swing != NULL) && (ov_cursor != NULL) ) __add_override_controls(tab_pages[TAB_VIEW]);
        __fill_in_InfoTab( _model_data );

        if (lv_gltfview_get_probe(_model_data)->cameraCount > 0) {
            use_scenecam = true;
            camera = 0;
        }

        if (lv_gltfview_get_probe(_model_data)->animationCount > 0) anim = 0;
        __apply_camera_button_visibility(_model_data);
        __apply_anim_button_visibility(_model_data);
        lv_gltfview_set_camera(_viewer, use_scenecam ? camera : -1);
        lv_gltfview_set_width(_viewer, BIG_TEXTURE_WIDTH);
        lv_gltfview_set_height(_viewer, BIG_TEXTURE_HEIGHT);
        lv_gltfview_set_focal_x (_viewer, 0.f);  // Because this position is automatically determined the first time drawing an object, we explicitly do not set it here.
        lv_gltfview_set_focal_y (_viewer, 0.f);
        lv_gltfview_set_focal_z (_viewer, 0.f);
        lv_gltfview_set_anim(_viewer, anim_enabled ? anim : -1);

        lv_3dtexture_id_t gltf_texture = lv_gltfview_render( shaderCache, _viewer, _model_data );
        lv_3dtexture_set_src(tex, gltf_texture);

        lv_obj_add_flag(grp_loading, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(tex, LV_OBJ_FLAG_HIDDEN);

        lv_obj_invalidate(tex);
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

        signal(SIGINT, handle_sigint);

        const int WINDOW_CONTROL_MARGIN = 80; 
        const int WINDOW_WIDTH_MINUS_MARGIN = WINDOW_WIDTH-WINDOW_CONTROL_MARGIN;
        const int WINDOW_HEIGHT_MINUS_MARGIN = WINDOW_HEIGHT-WINDOW_CONTROL_MARGIN;

        struct timeval start;
        lv_point_t _mousepoint;
        lv_indev_state_t mouse_state, mouse_state_ex;

        float MEASURED_CLOCKS_PER_SEC = 165000.0f;
        float ROLLING_FPS = 0.0f;
        long unsigned int frames_this_second = 0;
        long unsigned int frames_rendered_this_second = 0;
        
        long unsigned int ticks_this_second = 0;
        long unsigned int ticks = 0;
        unsigned long int usec_span = 0;

        float _groundpos[3] = {0.f, 0.f, 0.f};

        time_t last_poll = time(0);
        clock_t last_clock = clock();
        double subjectRadius = lv_gltf_get_int_radiusX1000(_model_data) / 1000.f;
        double movePow = min(subjectRadius, pow(subjectRadius, 0.5));
        char * pixels = lv_malloc(BIG_TEXTURE_HEIGHT * BIG_TEXTURE_WIDTH * 4);

        {   
            bool _timing_break_flag = false;
            while (!_timing_break_flag) {
                time_t this_poll = time(0);
                _timing_break_flag = true; //difftime(this_poll, last_poll) > 0;
                lv_refr_now(NULL);
                last_poll = this_poll;
            } 
        }

        gettimeofday(&start, NULL);
        lv_indev_get_point(mouse, &_mousepoint);

        while(!glfwWindowShouldClose(glfw_window)) {
            uint32_t ms_delay = lv_timer_handler();
            if (camselection_changed) __apply_camera_button_visibility(_model_data);
            if (animselection_changed) __apply_anim_button_visibility(_model_data);
            
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
                lv_gltfview_set_spin_degree_offset(_viewer, spin_counter_degrees);
            }

            mouse_state = lv_indev_get_state(mouse);
            mouse_state_ex = mouse_state & 0xF0;
            mouse_state = mouse_state & 0x0F;
            lv_indev_get_point(mouse, &_mousepoint);
            bool mouse_in_window = ((_mousepoint.x >= WINDOW_CONTROL_MARGIN) && (_mousepoint.x <= (WINDOW_WIDTH_MINUS_MARGIN)) && (_mousepoint.y >= WINDOW_CONTROL_MARGIN) && (_mousepoint.y <= (WINDOW_HEIGHT_MINUS_MARGIN)) );
            bool leftButtonPressed = mouse_in_window && ((mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_LEFT) && (mouse_state == LV_INDEV_STATE_PR));
            bool rightButtonPressed = mouse_in_window && ((mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_RIGHT) && (mouse_state == LV_INDEV_STATE_PR) );
            bool middleButtonPressed = mouse_in_window && ((mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_MIDDLE) && (mouse_state == LV_INDEV_STATE_PR) );
            bool mouse4ButtonPressed = mouse_in_window && ((mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_4) && (mouse_state == LV_INDEV_STATE_PR) );
            bool mouse5ButtonPressed = mouse_in_window && ((mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_5) && (mouse_state == LV_INDEV_STATE_PR) );
            //bool rightButtonPressed = (data.state == LV_INDEV_STATE_PR && data.btn_state & LV_BTN_STATE_RIGHT);
            //bool middleButtonPressed = (data.state == LV_INDEV_STATE_PR && data.btn_state & LV_BTN_STATE_MID);            

            if (mouse_in_window) {
                bool _res = lv_gltfview_raycast_ground_position(_viewer, _mousepoint.x, _mousepoint.y, WINDOW_WIDTH, WINDOW_HEIGHT,  1.0, _groundpos);
                if (_res && (ov_cursor != NULL)) {
                    ov_cursor->data1 = _groundpos[0];
                    ov_cursor->data2 = _groundpos[1];
                    ov_cursor->data3 = _groundpos[2];
                }
                if (leftButtonPressed) update_camera_turn(_mousepoint.x, _mousepoint.y, lastMouseX, lastMouseY);
                else if (rightButtonPressed) update_camera_drag_xz(movePow, _mousepoint.x, _mousepoint.y, lastMouseX, lastMouseY);
                else if (middleButtonPressed) update_camera_drag_y(movePow, _mousepoint.y, lastMouseY);
                else if (mouse4ButtonPressed) update_camera_zoom(_mousepoint.y, lastMouseY);
                else if (mouse5ButtonPressed) printf("Mouse Button #5 Pressed\n");
            }

            lastMouseX = _mousepoint.x;
            lastMouseY = _mousepoint.y;
            lv_gltfview_set_camera(_viewer, use_scenecam ? camera : -1);
            lv_gltfview_set_anim(_viewer, anim_enabled ? anim : -1);

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
                    printf("Frames Drawn: %ld | Average FPS: %2.1f | Ticks per sec: %2.1f\n", frames_rendered_this_second, ROLLING_FPS, MEASURED_CLOCKS_PER_SEC);
                }
                ticks_this_second = 0;
                frames_this_second = 0;
                frames_rendered_this_second = 0;
                //view_desc.error_frames = 0;
            }

            lv_gltfview_set_timestep(_viewer, anim_enabled ? sec_span * anim_rate : 0.f );
            last_poll = this_poll;
            gltf_texture = lv_gltfview_render( shaderCache, _viewer, _model_data );
            if (!lv_gltfview_check_frame_was_cached(_viewer)) {
                frames_rendered_this_second += 1;
                lv_3dtexture_set_src(tex, gltf_texture);
                lv_obj_invalidate(tex);
                lv_refr_now(NULL);
                #ifdef EXPERIMENTAL_DESKTOP
                    GL_CALL(glBindTexture(GL_TEXTURE_2D, gltf_texture));
                    stbi_write_png_compression_level = 0;
                    stbi_flip_vertically_on_write(true);
                    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                    stbi_write_png("/var/ramtemp/background.png", BIG_TEXTURE_WIDTH, BIG_TEXTURE_HEIGHT, 4, pixels, BIG_TEXTURE_WIDTH * 4);
                    // Set the desktop background
                    system("pcmanfm --set-wallpaper /var/ramtemp/background.png");
                #else 
                    if (frameCount > 0) {
                        char _buffer[100];
                        snprintf(_buffer, sizeof(_buffer), "/home/pi/Desktop/lv_gltf_viewer/render_frames/frame%03d.png", (maxFrames - frameCount));
                        GL_CALL(glBindTexture(GL_TEXTURE_2D, gltf_texture));
                        stbi_write_png_compression_level = 0;
                        stbi_flip_vertically_on_write(true);
                        glGetTexImage(GL_TEXTURE_2D, lv_gltfview_check_frame_was_antialiased(_viewer) ? 1 : 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                        stbi_write_png(_buffer, BIG_TEXTURE_WIDTH, BIG_TEXTURE_HEIGHT, 4, pixels, BIG_TEXTURE_WIDTH * 4);
                    }
                #endif
            } else {
                usleep(30000);
            }
            glfwPollEvents();

            if (frameCount > 0) {
                frameCount -= 1;
                if (frameCount == 0) {
                    glfwSetWindowShouldClose(glfw_window, GLFW_TRUE);
                }
            }
        }
        
        lv_obj_clear_flag(grp_loading, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(tex, LV_OBJ_FLAG_HIDDEN);
        demo_load_progress_callback("Closing Application", "", 0.f, 0.f, 0.f, 0.f);
        usleep(20 * 1000);
        
        lv_obj_invalidate(grp_loading);
        lv_refr_now(NULL);
        lv_timer_handler();
        lv_task_handler();
        
        lv_gltfview_destroy(_viewer, _model_data, shaderCache);
        lv_free(pixels);
        stbi_image_free(images[0].pixels);

    }
    lv_free(_viewer);
    exit(0);
}