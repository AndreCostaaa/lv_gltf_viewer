

#include "demo.h"
#include "stb_image/stb_image.h"
#include <signal.h>     /* to trap ctrl-break */

static GLFWwindow * glfw_window;

bool demo_os_integrate_window_should_close( void ){
    return glfwWindowShouldClose(glfw_window);
}

void demo_os_integrate_signal_window_close(void){
    glfwSetWindowShouldClose(glfw_window, GLFW_TRUE);
}

void os_integrate_filedrop_callback(GLFWwindow* _window, int count, const char** paths){
    LV_UNUSED(_window);
    int i;
    for (i = 0;  i < count;  i++) {
        printf("file dropped into window: %s\n", paths[i]);
    }
}

void os_integrate_handle_sigint(int sig){
    LV_UNUSED(sig);
    printf("\nShutting down app (from ctrl-c)...\n");
    demo_os_integrate_signal_window_close();
}

void os_integrate_window_close_callback(GLFWwindow* _window){
    LV_UNUSED(_window);
    printf("\nShutting down app (from window close)...\n");
    demo_os_integrate_signal_window_close();
}

void demo_os_integrate_setup_glfw_window( lv_glfw_window_t * lv_window ) {
    
    glfw_window = (GLFWwindow *)lv_glfw_window_get_glfw_window(lv_window);

    glfwSetWindowCloseCallback(glfw_window, os_integrate_window_close_callback);
    glfwSetWindowSizeLimits(glfw_window, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetWindowAttrib(glfw_window, GLFW_RESIZABLE, false);
    glfwSetWindowAspectRatio(glfw_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetDropCallback(glfw_window, os_integrate_filedrop_callback);
    
    GLFWimage images[1]; 
    int width, height, channel;
    stbi_uc* img = stbi_load("./window_icon_32px.png", &width, &height, &channel, 0); //rgba channels 
    if (img == NULL) printf("Icon Can Not Be Loaded\n");
    images[0].height = 32;
    images[0].width = 32;
    images[0].pixels = img;
    glfwSetWindowIcon(glfw_window, 1, images);
    stbi_image_free(images[0].pixels);

    glfwMakeContextCurrent(glfw_window);
    signal(SIGINT, os_integrate_handle_sigint);

}

bool demo_os_integrate_confirm_desktop_mode_ok( void ) {
    // Construct the command
    char command[256];
    snprintf(command, sizeof(command), "sudo ./__confirm_ramdrive.sh %s", DESKTOP_OUTPUT_RAMTEMP_PATH);
    // Call the script
    int result = system(command);
    return (result == 0) ? true : false;
}