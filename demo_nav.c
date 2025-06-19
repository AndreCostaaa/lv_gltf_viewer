#include "demo.h"
#include <math.h>

float goal_pitch;
float goal_yaw;
float goal_distance;
float goal_focal_x;
float goal_focal_y;
float goal_focal_z;

void nav_turn( int mouse_x, int mouse_y,int last_mouse_x, int last_mouse_y) {
    // Calculate the change in mouse position
    float deltaX = (float)(mouse_x - last_mouse_x);
    float deltaY = (float)(mouse_y - last_mouse_y);
    // Sensitivity factor for rotation
    float sensitivity = 0.195f;
    // Calculate pitch and yaw changes
    float pitchChange = deltaY * -sensitivity;
    float yawChange = deltaX * -sensitivity;
    //bool viewChanged = false;
    // Update camera rotation
    if (fabs(pitchChange) > 0.001f) {
        goal_pitch += pitchChange;
        if (goal_pitch > 89.9f) { goal_pitch = 89.9f; }
        else if (goal_pitch < -89.9f) { goal_pitch = -89.9f; }
        //viewChanged = true;
    }
    if (fabs(yawChange) > 0.001f) {
        goal_yaw += yawChange;
        //viewChanged = true;
    }
    // if (viewChanged) printf("Pitch Change: %f, Yaw Change: %f\n", pitchChange, yawChange);
}
void nav_drag_xz(float unit_distance, int mouse_x, int mouse_y, int last_mouse_x, int last_mouse_y) {
    
    // Calculate the change in mouse position
    float deltaX = (float)(mouse_x - last_mouse_x);
    float deltaY = (float)(mouse_y - last_mouse_y);
    
    // Sensitivity factor for movement
    float sensitivity = 0.005f * unit_distance;
    // Calculate strafing and forward/backward motion
    float offset_yaw = lv_gltf_view_get_yaw(demo_gltfview) + lv_gltf_view_get_spin_degree_offset(demo_gltfview); //spin_counter_degrees;  // The yaw in the view description will not represent the actual visible yaw if the platter has spun the orientation off base 0
    //bool viewChanged = false;
    if (fabs(deltaY) > 0.001f) {
        //viewChanged = true;
        // Calculate the direction based on the current yaw angle
        // Update camera position based on strafing and forward/backward motion
        float forwardBackwardAmount = deltaY * sensitivity;
        float forwardRadians = offset_yaw * PI_TO_RAD; // Convert forward yaw to radians
        goal_focal_x += (-sinf(forwardRadians) * forwardBackwardAmount);
        goal_focal_z += (-cosf(forwardRadians) * forwardBackwardAmount);
    }
    if (fabs(deltaX) > 0.001f) {
        //viewChanged = true;
        float strafeAmount = deltaX * sensitivity;
        float strafeRadians = (offset_yaw + 90.0f) * PI_TO_RAD; // Convert right yaw to radians
        goal_focal_x += (-sinf(strafeRadians) * strafeAmount);
        goal_focal_z += ( -cosf(strafeRadians) * strafeAmount);
    }
    // if ( viewChanged ) printf("Camera Position: (%.2f, %.2f, %.2f)\n",  lv_gltf_view_get_focal_x(demo_gltfview), lv_gltf_view_get_focal_y(demo_gltfview),  lv_gltf_view_get_focal_z(demo_gltfview));
}
void nav_drag_y(float unit_distance, int mouse_y, int last_mouse_y) {
    // Calculate the change in mouse position
    float deltaY = (float)(mouse_y - last_mouse_y);
    // Sensitivity factor for movement
    float sensitivity = 0.0025f * unit_distance;
    if (fabs(deltaY) > 0.001f) {
        goal_focal_y += deltaY * sensitivity;
    }
}
void nav_zoom( int mouse_y, int last_mouse_y) {
    // Calculate the change in mouse position
    float deltaY = (float)(mouse_y - last_mouse_y);
    
    // Sensitivity factor for movement
    float sensitivity = 0.0025f;
    if (fabs(deltaY) > 0.001f) {
        float inOutAmount = deltaY * sensitivity;
        goal_distance += inOutAmount;
        if (goal_distance < 0.f) {
            goal_distance = 0.f;
        }
    }
}

void demo_nav_process_drag(float movement_power, uint32_t mouse_state_ex, int mouse_x, int mouse_y, int last_mouse_x, int last_mouse_y) {
    if (mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_LEFT)         nav_turn(mouse_x, mouse_y, last_mouse_x, last_mouse_y);
    else if (mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_RIGHT)   nav_drag_xz(movement_power, mouse_x, mouse_y, last_mouse_x, last_mouse_y);
    else if (mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_MIDDLE)  nav_drag_y(movement_power, mouse_y, last_mouse_y);
    else if (mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_4)       nav_zoom(mouse_y, last_mouse_y);
    else if (mouse_state_ex == LV_INDEV_STATE_EX_MOUSE_5)       printf("Mouse Button #5 Pressed\n");
}

void demo_nav_gradual_to_goals( void ) {
    float EASE_POWER = 0.5f;
    float EASE_CLOSE_ENOUGH = 0.001f;

    float tfx = lerp(lv_gltf_view_get_focal_x(demo_gltfview), goal_focal_x, EASE_POWER / 8.f);
    if (fabsf(tfx - goal_focal_x) < (EASE_CLOSE_ENOUGH/100.f)) {tfx = goal_focal_x;}
    lv_gltf_view_set_focal_x(demo_gltfview, tfx); 

    float tfy = lerp(lv_gltf_view_get_focal_y(demo_gltfview), goal_focal_y, EASE_POWER / 8.f);
    if (fabsf(tfy - goal_focal_y) < (EASE_CLOSE_ENOUGH/100.f)) {tfy = goal_focal_y;}
    lv_gltf_view_set_focal_y(demo_gltfview, tfy); 

    float tfz = lerp(lv_gltf_view_get_focal_z(demo_gltfview), goal_focal_z, EASE_POWER / 8.f);
    if (fabsf(tfz - goal_focal_z) < (EASE_CLOSE_ENOUGH/100.f)) {tfz = goal_focal_z;}
    lv_gltf_view_set_focal_z(demo_gltfview, tfz); 

    float tyaw = lerp(lv_gltf_view_get_yaw(demo_gltfview), goal_yaw, EASE_POWER / 28.f);
    if (fabsf(tyaw - goal_yaw) < EASE_CLOSE_ENOUGH/100.f) {tyaw = goal_yaw;}
    bool looped = demo_ui_apply_yaw_value(tyaw );
    if (looped) {
        // Move the goal and lerped yaw back into the -180 <-> +180 degree range
        while (goal_yaw < -180.f) goal_yaw += 360.f;
        while (goal_yaw > 180.f) goal_yaw -= 360.f;
        while (tyaw < -180.f) tyaw += 360.f;
        while (tyaw > 180.f) tyaw -= 360.f;
    }
    lv_gltf_view_set_yaw(demo_gltfview, (int)(tyaw * 10.f)); 

    float tpitch = lerp(lv_gltf_view_get_pitch(demo_gltfview), goal_pitch, EASE_POWER / 28.f);
    if (fabsf(tpitch - goal_pitch) < EASE_CLOSE_ENOUGH/100.f) {tpitch = goal_pitch;}
    lv_gltf_view_set_pitch(demo_gltfview, (int)(tpitch * 10.f)); demo_ui_apply_pitch_value(tpitch );

    float tdistance = lerp(lv_gltf_view_get_distance(demo_gltfview), goal_distance, EASE_POWER / 8.f);
    if (fabsf(tdistance - goal_distance) < (EASE_CLOSE_ENOUGH/100.f)) {tdistance = goal_distance;}
    lv_gltf_view_set_distance(demo_gltfview, (int)(tdistance * 1000.f)); demo_ui_apply_distance_value(tdistance );
}