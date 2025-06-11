#include "demo.h"
#include <math.h>

void nav_turn( int mouse_x, int mouse_y,int last_mouse_x, int last_mouse_y) {

    // Calculate the change in mouse position
    float deltaX = (float)(mouse_x - last_mouse_x);
    float deltaY = (float)(mouse_y - last_mouse_y);
    // Sensitivity factor for rotation
    float sensitivity = 0.5f;
    // Calculate pitch and yaw changes
    float pitchChange = deltaY * -sensitivity;
    float yawChange = deltaX * -sensitivity;
    //bool viewChanged = false;
    // Update camera rotation
    if (fabs(pitchChange) > 0.001f) {
        lv_gltfview_inc_pitch(demo_gltfview, pitchChange);
        //viewChanged = true;
    }
    if (fabs(yawChange) > 0.001f) {
        lv_gltfview_inc_yaw(demo_gltfview, yawChange);
        //viewChanged = true;
    }
    // if (viewChanged) printf("Pitch Change: %f, Yaw Change: %f\n", pitchChange, yawChange);
}
void nav_drag_xz(float unit_distance, int mouse_x, int mouse_y, int last_mouse_x, int last_mouse_y) {
    
    // Calculate the change in mouse position
    float deltaX = (float)(mouse_x - last_mouse_x);
    float deltaY = (float)(mouse_y - last_mouse_y);
    
    // Sensitivity factor for movement
    float sensitivity = 0.0025f * unit_distance;
    // Calculate strafing and forward/backward motion
    float offset_yaw = lv_gltfview_get_yaw(demo_gltfview) + lv_gltfview_get_spin_degree_offset(demo_gltfview); //spin_counter_degrees;  // The yaw in the view description will not represent the actual visible yaw if the platter has spun the orientation off base 0
    //bool viewChanged = false;
    if (fabs(deltaY) > 0.001f) {
        //viewChanged = true;
        // Calculate the direction based on the current yaw angle
        // Update camera position based on strafing and forward/backward motion
        float forwardBackwardAmount = deltaY * sensitivity;
        float forwardRadians = offset_yaw * PI_TO_RAD; // Convert forward yaw to radians
        lv_gltfview_inc_focal_x(demo_gltfview, -sinf(forwardRadians) * forwardBackwardAmount);
        lv_gltfview_inc_focal_z(demo_gltfview, -cosf(forwardRadians) * forwardBackwardAmount);
    }
    if (fabs(deltaX) > 0.001f) {
        //viewChanged = true;
        float strafeAmount = deltaX * sensitivity;
        float strafeRadians = (offset_yaw + 90.0f) * PI_TO_RAD; // Convert right yaw to radians
        lv_gltfview_inc_focal_x(demo_gltfview, -sinf(strafeRadians) * strafeAmount);
        lv_gltfview_inc_focal_z(demo_gltfview, -cosf(strafeRadians) * strafeAmount);        
    }
    // if ( viewChanged ) printf("Camera Position: (%.2f, %.2f, %.2f)\n",  lv_gltfview_get_focal_x(demo_gltfview), lv_gltfview_get_focal_y(demo_gltfview),  lv_gltfview_get_focal_z(demo_gltfview));
}
void nav_drag_y(float unit_distance, int mouse_y, int last_mouse_y) {
    // Calculate the change in mouse position
    //float deltaX = (float)(mouse_x - last_mouse_x);
    float deltaY = (float)(mouse_y - last_mouse_y);
    // Sensitivity factor for movement
    float sensitivity = 0.0025f * unit_distance;
    if (fabs(deltaY) > 0.001f) {
        lv_gltfview_inc_focal_y(demo_gltfview, deltaY * sensitivity);        
        // printf("Elev up/down - elevation: %f\n", lv_gltfview_get_focal_y(demo_gltfview));
    }
}
void nav_zoom( int mouse_y, int last_mouse_y) {
    // Calculate the change in mouse position
    //float deltaX = (float)(mouse_x - last_mouse_x);
    float deltaY = (float)(mouse_y - last_mouse_y);
    
    // Sensitivity factor for movement
    float sensitivity = 0.0025f;// * unit_distance;
    if (fabs(deltaY) > 0.001f) {
        float inOutAmount = deltaY * sensitivity;
        lv_gltfview_inc_distance(demo_gltfview, inOutAmount);        
        // printf("Zoom in/out - distance: %f\n", lv_gltfview_get_distance(demo_gltfview));
    }
}

void demo_nav_process_drag(float movement_power, uint32_t mouse_state_ex, int mouse_x, int mouse_y, int last_mouse_x, int last_mouse_y) {
    if (mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_LEFT) nav_turn(mouse_x, mouse_y, last_mouse_x, last_mouse_y);
    else if (mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_RIGHT) nav_drag_xz(movement_power, mouse_x, mouse_y, last_mouse_x, last_mouse_y);
    else if (mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_MIDDLE) nav_drag_y(movement_power, mouse_y, last_mouse_y);
    else if (mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_4) nav_zoom(mouse_y, last_mouse_y);
    else if (mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_5) printf("Mouse Button #5 Pressed\n");
}