
#include "fastgltf/math.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lv_gltf_view.h"
#include "../data/lv_gltf_bind.h"
#include "lv_gltf_view_internal.hpp"
#include "sup/include/shader_includes.h"
#include <algorithm>
#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <sys/time.h>

#include <lvgl.h>
#include <drivers/glfw/lv_opengles_debug.h> /* GL_CALL */

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image/stb_image_resize.h"

#include "../data/lv_gltf_data.h"
#include "../data/lv_gltf_data_internal.hpp"
#include "../data/lv_gltf_data_internal.h"

static MapofTransformMap _ibmBySkinThenNode;


static void render_materials(lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data, lv_gl_shader_manager_t * manager,
                             const MaterialIndexMap & map);

namespace fastgltf
{
FASTGLTF_EXPORT template <typename AssetType, typename Callback>
#if FASTGLTF_HAS_CONCEPTS
    requires std::same_as<std::remove_cvref_t<AssetType>, Asset>
    && std::is_invocable_v<Callback, fastgltf::Node &, fastgltf::math::fmat4x4 &, fastgltf::math::fmat4x4 &>
#endif
inline void custom_iterateSceneNodes(AssetType&& asset, std::size_t sceneIndex, math::fmat4x4 * initial,
                                     Callback callback)
{
    auto & scene = asset.scenes[sceneIndex];
    auto & nodes = asset.nodes;
    auto function = [&](std::size_t nodeIndex, math::fmat4x4 & parentWorldMatrix, auto & self) -> void {
        //assert(asset.nodes.size() > nodeIndex);
        auto & node = nodes[nodeIndex];
        auto _localMat = getTransformMatrix(node, math::fmat4x4());
        std::invoke(callback, node, parentWorldMatrix, _localMat);
        uint32_t num_children = node.children.size();
        if(num_children > 0) {
            math::fmat4x4 _parentWorldTemp = parentWorldMatrix * _localMat;
            if(num_children > 1) {
                math::fmat4x4 per_child_copy = math::fmat4x4(_parentWorldTemp);
                for(auto & child : node.children) self(child, per_child_copy, self);
            }
            else {
                self(node.children[0], _parentWorldTemp,  self);
            }
        }
    };
    // auto tempmat = fastgltf::math::fmat4x4(*initial);
    //for (auto& sceneNode : scene.nodeIndices) function(sceneNode, &tempmat, function);
    for(auto & sceneNode : scene.nodeIndices) function(sceneNode, *initial, function);
}
}
// It's simpler here to just declare the functions as part of the fastgltf::math namespace.
namespace fastgltf::math
{
/** Creates a right-handed view matrix */
[[nodiscard]] fmat4x4 lookAtRH(const fvec3 & eye, const fvec3 & center, const fvec3 & up) noexcept
{
    auto dir = normalize(center - eye);
    auto lft = normalize(cross(dir, up));
    auto rup = cross(lft, dir);

    mat<float, 4, 4> ret(1.f);
    ret.col(0) = { lft.x(), rup.x(), -dir.x(), 0.f };
    ret.col(1) = { lft.y(), rup.y(), -dir.y(), 0.f };
    ret.col(2) = { lft.z(), rup.z(), -dir.z(), 0.f };
    ret.col(3) = { -dot(lft, eye), -dot(rup, eye), dot(dir, eye), 1.f };
    return ret;
}

/**
 * Creates a right-handed perspective matrix, with the near and far clips at -1 and +1, respectively.
 * @param fov The FOV in radians
 */
[[nodiscard]] fmat4x4 perspectiveRH(float fov, float ratio, float zNear, float zFar) noexcept
{
    mat<float, 4, 4> ret(0.f);
    auto tanHalfFov = std::tan(fov / 2.f);
    ret.col(0).x() = 1.f / (ratio * tanHalfFov);
    ret.col(1).y() = 1.f / tanHalfFov;
    ret.col(2).z() = -(zFar + zNear) / (zFar - zNear);
    ret.col(2).w() = -1.f;
    ret.col(3).z() = -(2.f * zFar * zNear) / (zFar - zNear);
    return ret;
}

template <typename T>
[[nodiscard]] fastgltf::math::quat<T> eulerToQuaternion(T P, T Y, T R)
{
    // Convert degrees to radians if necessary
    // roll = roll * (M_PI / 180.0);
    // pitch = pitch * (M_PI / 180.0);
    // yaw = yaw * (M_PI / 180.0);
    T H = T(0.5);
    Y *= H;
    P *= H;
    R *= H;
    T cy = cos(Y), sy = sin(Y), cp = cos(P), sp = sin(P), cr = cos(R), sr = sin(R);
    T cr_cp = cr * cp, sp_sy = sp * sy, sr_cp = sr * cp, sp_cy = sp * cy;
    return fastgltf::math::quat<T>(
               sr_cp * cy - cr * sp_sy, // X
               sr_cp * sy + cr * sp_cy, // Y
               cr_cp * sy - sr * sp_cy, // Z
               cr_cp * cy + sr * sp_sy  // W
           );
}

template <typename T>
[[nodiscard]] fastgltf::math::vec<T, 3> quaternionToEuler(fastgltf::math::quat<T> q)
{
    T Q11 = q[1] * q[1];
    // Roll (Z)
    T sinr_cosp = T(2.0) * (q[3] * q[0] + q[1] * q[2]);
    T cosr_cosp = T(1.0) - T(2.0) * (q[0] * q[0] + Q11);
    // Pitch (X)
    T sinp = T(2.0) * (q[3] * q[1] - q[2] * q[0]);
    // Yaw (Y)
    T siny_cosp = T(2.0) * (q[3] * q[2] + q[0] * q[1]);
    T cosy_cosp = T(1.0) - T(2.0) * (Q11 + q[2] * q[2]);

    return fastgltf::math::vec<T, 3>(
               (std::abs(sinp) >= T(1)) ? std::copysign(T(3.1415926) / T(2), sinp) : std::asin(sinp),
               std::atan2(siny_cosp, cosy_cosp),
               std::atan2(sinr_cosp, cosr_cosp)
           );
}

} // namespace fastgltf::math

gl_viewer_desc_t * lv_gltf_view_get_desc(lv_gltf_view_t * v)
{
    return &(v->desc);
}

float absf(float v)
{
    return v > 0 ? v : -v;
}

bool lv_gltf_view_check_dirty(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->dirty;
}

void        lv_gltf_view_mark_dirty(lv_gltf_view_t * view)
{
    lv_gltf_view_get_desc(view)->dirty = true;
}
uint32_t    lv_gltf_view_get_aa_mode(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->aa_mode;
}
uint32_t    lv_gltf_view_get_bg_mode(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->bg_mode;
}
uint32_t    lv_gltf_view_get_width(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->width;
}
uint32_t    lv_gltf_view_get_height(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->height;
}
float       lv_gltf_view_get_pitch(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->pitch;
}
float       lv_gltf_view_get_yaw(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->yaw;
}
float       lv_gltf_view_get_distance(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->distance;
}
float       lv_gltf_view_get_fov(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->fov;
}
float       lv_gltf_view_get_focal_x(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->focal_x;
}
float       lv_gltf_view_get_focal_y(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->focal_y;
}
float       lv_gltf_view_get_focal_z(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->focal_z;
}
float       lv_gltf_view_get_spin_degree_offset(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->spin_degree_offset;
}
uint32_t    lv_gltf_view_get_last_render_total_msec(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->last_render_total_msec;
}
uint64_t    lv_gltf_view_get_last_render_start_msec(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->last_render_system_msec;
}
uint32_t    lv_gltf_view_get_fps_goal_delay(lv_gltf_view_t * view, double goal_fps_d)
{
    float goal_fps = goal_fps_d > 0.f ? goal_fps_d : 0.1f;
    uint32_t goal_fps_msec = ((1.0f / goal_fps) * 1000);
    struct timeval now_time;
    gettimeofday(&now_time, NULL);
    uint64_t now_msec = (((now_time.tv_sec * 1000000) + now_time.tv_usec) / 1000);
    uint64_t last_start_msec = lv_gltf_view_get_desc(view)->last_render_system_msec;
    if(last_start_msec == 0) {
        return goal_fps_msec;
    }
    int64_t ret_delay = goal_fps_msec - (now_msec - last_start_msec);
    if(ret_delay > goal_fps_msec) {
        printf("ret delay was abnormal: %d msec - using fps delay instead\n", goal_fps_msec);
        return goal_fps_msec;
    }
    return ret_delay > 0 ? (uint32_t)ret_delay : 0;
}

void lv_gltf_view_set_pitch(lv_gltf_view_t * view, int pitch_degrees_x100)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    float _newval = pitch_degrees_x100 * 0.01f;
    if(absf(desc->pitch - _newval) > 0.0001f) {
        desc->pitch = _newval;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_yaw(lv_gltf_view_t * view, int yaw_degrees_x100)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    float _newval = yaw_degrees_x100 * 0.01f;
    if(absf(desc->yaw - _newval) > 0.0001f) {
        desc->yaw = _newval;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_distance(lv_gltf_view_t * view, int distance_units_x1000)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    float _newval = distance_units_x1000 * 0.001f;
    if(absf(desc->distance - _newval) > 0.001f) {
        desc->distance = _newval;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_focal_x(lv_gltf_view_t * view, float focal_x)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(desc->focal_x - focal_x) > 0.0001f) {
        desc->focal_x = focal_x;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_focal_y(lv_gltf_view_t * view, float focal_y)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(desc->focal_y - focal_y) > 0.0001f) {
        desc->focal_y = focal_y;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_focal_z(lv_gltf_view_t * view, float focal_z)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(desc->focal_z - focal_z) > 0.00001f) {
        desc->focal_z = focal_z;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_fov(lv_gltf_view_t * view, float vertical_fov)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(desc->fov - vertical_fov) > 0.001f) {
        desc->fov = vertical_fov;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_bg_r(lv_gltf_view_t * view, uint32_t r)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(desc->bg_r != r) {
        desc->bg_r = r;
        desc->dirty = true;
    }
}
void lv_gltf_view_set_bgcolor_red(lv_gltf_view_t * view, uint32_t r)
{
    lv_gltf_view_set_bg_r(view, r);
}

void lv_gltf_view_set_bg_g(lv_gltf_view_t * view, uint32_t g)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(desc->bg_g != g) {
        desc->bg_g = g;
        desc->dirty = true;
    }
}
void lv_gltf_view_set_bgcolor_green(lv_gltf_view_t * view, uint32_t g)
{
    lv_gltf_view_set_bg_g(view, g);
}

void lv_gltf_view_set_bg_b(lv_gltf_view_t * view, uint32_t b)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(desc->bg_b != b) {
        desc->bg_b = b;
        desc->dirty = true;
    }
}
void lv_gltf_view_set_bgcolor_blue(lv_gltf_view_t * view, uint32_t b)
{
    lv_gltf_view_set_bg_b(view, b);
}

void lv_gltf_view_set_bg_a(lv_gltf_view_t * view, uint32_t a)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(desc->bg_a != a) {
        desc->bg_a = a;
        desc->dirty = true;
    }
}
void lv_gltf_view_set_bg_opa(lv_gltf_view_t * view, uint32_t a)
{
    lv_gltf_view_set_bg_a(view, a);
}

void lv_gltf_view_set_bgcolor_RGB(lv_gltf_view_t * view, uint32_t r, uint32_t g, uint32_t b)
{
    lv_gltf_view_set_bgcolor_red(view, r);
    lv_gltf_view_set_bgcolor_green(view, g);
    lv_gltf_view_set_bgcolor_blue(view, b);
}

void lv_gltf_view_set_bgcolor_RGBA(lv_gltf_view_t * view, uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
    lv_gltf_view_set_bgcolor_red(view, r);
    lv_gltf_view_set_bgcolor_green(view, g);
    lv_gltf_view_set_bgcolor_blue(view, b);
    lv_gltf_view_set_bg_opa(view, a);
}

void lv_gltf_view_inc_pitch(lv_gltf_view_t * view, float pitch_inc_degrees)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(pitch_inc_degrees) > 0.0001f) {
        desc->pitch += pitch_inc_degrees;
        if(desc->pitch > 89.0f) {
            desc->pitch = 89.0f;
        }
        else if(desc->pitch < -89.0f) {
            desc->pitch = -89.0f;
        }
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_yaw(lv_gltf_view_t * view, float yaw_inc_degrees)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(yaw_inc_degrees) > 0.0001f) {
        desc->yaw += yaw_inc_degrees;
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_distance(lv_gltf_view_t * view, float distance_inc_units)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(distance_inc_units) != 0.f) {
        desc->distance += distance_inc_units;
        if(desc->distance < 0.01f) {
            desc->distance = 0.01f;
        }
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_focal_x(lv_gltf_view_t * view, float focal_x_inc)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(focal_x_inc) > 0.0001f) {
        desc->focal_x += focal_x_inc;
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_focal_y(lv_gltf_view_t * view, float focal_y_inc)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(focal_y_inc) > 0.0001f) {
        desc->focal_y += focal_y_inc;
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_focal_z(lv_gltf_view_t * view, float focal_z_inc)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(focal_z_inc) > 0.0001f) {
        desc->focal_z += focal_z_inc;
        desc->dirty = true;
    }
}

void lv_gltf_view_inc_spin_degree_offset(lv_gltf_view_t * view, float spin_degree_inc)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(spin_degree_inc) > 0.0001f) {
        desc->spin_degree_offset += spin_degree_inc;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_camera(lv_gltf_view_t * view, int camera_number)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(desc->camera != camera_number) {
        desc->camera = camera_number;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_timestep(lv_gltf_view_t * view, float timestep)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    desc->timestep = timestep;
}

void lv_gltf_view_set_width(lv_gltf_view_t * view, uint32_t new_width)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(desc->width  != (int32_t)new_width) {
        desc->width = (int32_t)new_width;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_height(lv_gltf_view_t * view, uint32_t new_height)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(desc->height  != (int32_t)new_height) {
        desc->height = (int32_t)new_height;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_anim(lv_gltf_view_t * view, uint32_t anim_num)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(desc->anim  != (int32_t)anim_num) {
        desc->anim = (int32_t)anim_num;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_bg_mode(lv_gltf_view_t * view, uint32_t bg_mode)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(desc->bg_mode != (int32_t)bg_mode) {
        desc->bg_mode = (int32_t)bg_mode;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_aa_mode(lv_gltf_view_t * view, uint32_t aa_mode)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(desc->aa_mode != (int32_t)aa_mode) {
        desc->aa_mode = (int32_t)aa_mode;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_blur_bg(lv_gltf_view_t * view, float blur_bg_amount)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(desc->blur_bg - blur_bg_amount) > 0.0001f) {
        desc->blur_bg = blur_bg_amount;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_env_pow(lv_gltf_view_t * view, float env_pow)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(desc->env_pow - env_pow) > 0.0001f) {
        desc->env_pow = env_pow;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_exposure(lv_gltf_view_t * view, float exposure)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(desc->exposure - exposure) > 0.0001f) {
        desc->exposure = exposure;
        desc->dirty = true;
    }
}

void lv_gltf_view_set_spin_degree_offset(lv_gltf_view_t * view, float spin_degree_offset)
{
    gl_viewer_desc_t * desc = (gl_viewer_desc_t *)lv_gltf_view_get_desc(view);
    if(absf(desc->spin_degree_offset - spin_degree_offset) > 0.0001f) {
        desc->spin_degree_offset = spin_degree_offset;
        desc->dirty = true;
    }
}

bool lv_gltf_view_check_frame_was_cached(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->frame_was_cached;
}

bool lv_gltf_view_check_frame_was_antialiased(lv_gltf_view_t * view)
{
    return lv_gltf_view_get_desc(view)->frame_was_antialiased;
}
int64_t lv_gltf_view_get_node_handle_by_name(const char * nodename)
{
    LV_UNUSED(nodename);
    return -1;
}

void lv_gltf_view_destroy(lv_gltf_view_t * _viewer)
{
    __free_viewer_struct(_viewer);  // Currently does nothing, this could be removed
    clear_defines();
}

/**
 * @brief Draw a primitive from the GLTF model.
 *
 * @param prim_num The index of the primitive to draw.
 * @param view_desc Pointer to the viewer description structure.
 * @param viewer Pointer to the lv_gltf_view instance used for rendering.
 * @param gltf_data Pointer to the GLTF data containing the model information.
 * @param node Reference to the node representing the primitive in the GLTF structure.
 * @param mesh_index The index of the mesh within the GLTF model.
 * @param matrix The transformation matrix to apply to the primitive.
 * @param env_tex The environment textures to use for rendering.
 * @param is_transmission_pass Flag indicating whether this is a transmission rendering pass.
 */
void draw_primitive(int32_t prim_num,
                    gl_viewer_desc_t * view_desc,
                    lv_gltf_view_t * viewer,
                    lv_gltf_data_t * gltf_data,
                    fastgltf::Node & node,
                    std::size_t mesh_index,
                    const fastgltf::math::fmat4x4 & matrix,
                    lv_gl_shader_manager_env_textures_t env_tex,
                    bool is_transmission_pass)
{
    lv_gltf_mesh_data_t * mesh = lv_gltf_data_get_mesh(gltf_data, mesh_index);
    const auto & asset = lv_gltf_data_get_asset(gltf_data);
    const auto & vopts = get_viewer_opts(viewer);
    const auto & _prim_data = lv_gltf_data_get_primitive_from_mesh(mesh, prim_num);
    auto & _prim_gltf_data = asset->meshes[mesh_index].primitives[prim_num];
    auto & mappings = _prim_gltf_data.mappings;
    std::size_t materialIndex = (!mappings.empty() && mappings[vopts->materialVariant].has_value())
                                ? mappings[vopts->materialVariant].value() + 1
                                : ((_prim_gltf_data.materialIndex.has_value()) ? (_prim_gltf_data.materialIndex.value() + 1) : 0);

    gltf_data->last_material_index = 999999;

    GL_CALL(glBindVertexArray(_prim_data->vertexArray));
    if((gltf_data->last_material_index == materialIndex) &&
       (gltf_data->last_pass_was_transmission == is_transmission_pass)) {
        GL_CALL(glUniformMatrix4fv(get_uniform_ids(gltf_data, materialIndex)->modelMatrixUniform, 1, GL_FALSE, &matrix[0][0]));
    }
    else {
        view_desc->error_frames += 1;
        gltf_data->last_material_index = materialIndex;
        gltf_data->last_pass_was_transmission = is_transmission_pass;
        auto program = lv_gltf_data_get_shader_program(gltf_data, materialIndex);
        const auto & uniforms = get_uniform_ids(gltf_data, materialIndex);
        GL_CALL(glUseProgram(program));

        GL_CALL(glUniformMatrix4fv(uniforms->modelMatrixUniform, 1, GL_FALSE, &matrix[0][0]));
        GL_CALL(glUniformMatrix4fv(uniforms->viewMatrixUniform, 1, false, GET_VIEW_MAT(viewer)->data()));
        GL_CALL(glUniformMatrix4fv(uniforms->projectionMatrixUniform, 1, false, GET_PROJ_MAT(viewer)->data()));
        GL_CALL(glUniformMatrix4fv(uniforms->viewProjectionMatrixUniform, 1, false, GET_VIEWPROJ_MAT(viewer)->data()));
        const auto & _campos = get_cam_pos(viewer);
        GL_CALL(glUniform3f(uniforms->camera,  _campos[0], _campos[1], _campos[2]));

        GL_CALL(glUniform1f(uniforms->exposure, view_desc->exposure));
        GL_CALL(glUniform1f(uniforms->envIntensity, view_desc->env_pow));
        GL_CALL(glUniform1i(uniforms->envMipCount, (int32_t)env_tex.mipCount));
        setup_environment_rotation_matrix(viewer->envRotationAngle, program);
        GL_CALL(glEnable(GL_CULL_FACE));
        GL_CALL(glDisable(GL_BLEND));
        GL_CALL(glEnable(GL_DEPTH_TEST));
        GL_CALL(glDepthMask(GL_TRUE));
        GL_CALL(glCullFace(GL_BACK));
        uint32_t _texnum = 0;

        bool has_material = asset->materials.size() > (materialIndex - 1);
        if(!has_material) {
            setup_uniform_color_alpha(uniforms->baseColorFactor, fastgltf::math::fvec4(1.0f));
            GL_CALL(glUniform1f(uniforms->roughnessFactor, 0.5f));
            GL_CALL(glUniform1f(uniforms->metallicFactor,  0.5f));
            GL_CALL(glUniform1f(uniforms->ior, 1.5f));
            GL_CALL(glUniform1f(uniforms->dispersion, 0.0f));
            GL_CALL(glUniform1f(uniforms->thickness, 0.01847f));
        }
        else {
            auto & gltfMaterial = asset->materials[materialIndex - 1];
            if(is_transmission_pass && (gltfMaterial.transmission != NULL)) {
                return;
            }
            if(gltfMaterial.doubleSided) GL_CALL(glDisable(GL_CULL_FACE));
            if(gltfMaterial.alphaMode == fastgltf::AlphaMode::Blend) {
                GL_CALL(glEnable(GL_BLEND));
                //GL_CALL(glDisable(GL_DEPTH_TEST));
                //GL_CALL(glCullFace(GL_FRONT));
                GL_CALL(glDepthMask(GL_FALSE));
                GL_CALL(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
                GL_CALL(glBlendEquation(GL_FUNC_ADD));
                GL_CALL(glEnable(GL_CULL_FACE));
            }
            else {
                if(gltfMaterial.alphaMode == fastgltf::AlphaMode::Mask) {
                    GL_CALL(glUniform1f(uniforms->alphaCutoff, gltfMaterial.alphaCutoff));
                    GL_CALL(glDisable(GL_CULL_FACE));
                }
            }

            // Update any scene lights present

            if(gltf_data->node_by_light_index.size() > 0) {
                size_t max_light_nodes = gltf_data->node_by_light_index.size();
                size_t max_scene_lights = asset->lights.size();
                if(max_scene_lights != max_light_nodes) {
                    std::cerr << "ERROR: Scene light count != scene light node count\n";
                }
                else {
                    // Scene contains lights and they may have just moved so update their position within the current shaders
                    // Update any lights present in the scene file
                    for(size_t ii = 0; ii < max_scene_lights; ii++) {
                        size_t i = ii + 1;
                        // Update each field of the light struct
                        std::string _prefix = "u_Lights[" + std::to_string(i) + "].";
                        auto & lightNode = gltf_data->node_by_light_index[ii];
                        fastgltf::math::fmat4x4 lightNodeMat = lv_gltf_data_get_cached_transform(gltf_data, lightNode);
                        const auto & m = lightNodeMat.data();
                        char _targ1[100];

                        strncpy(_targ1, (_prefix + "position").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform3fv(glGetUniformLocation(program, _targ1), 1, &lightNodeMat[3][0]);

                        strncpy(_targ1, (_prefix + "direction").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        fastgltf::math::fvec3 tlight_dir = fastgltf::math::fvec3(-lightNodeMat[2][0], -lightNodeMat[2][1], -lightNodeMat[2][2]);
                        glUniform3fv(glGetUniformLocation(program, _targ1), 1, &tlight_dir[0]);

                        strncpy(_targ1, (_prefix + "range").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        if(asset->lights[ii].range.has_value()) {
                            float light_scale = fastgltf::math::length(fastgltf::math::fvec3(m[0], m[4], m[8]));
                            glUniform1f(glGetUniformLocation(program, _targ1), asset->lights[ii].range.value() * light_scale);
                        }
                        else glUniform1f(glGetUniformLocation(program, _targ1), 9999.f);

                        strncpy(_targ1, (_prefix + "color").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform3fv(glGetUniformLocation(program, _targ1), 1, &(asset->lights[ii].color.data()[0]));

                        strncpy(_targ1, (_prefix + "intensity").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform1f(glGetUniformLocation(program, _targ1), asset->lights[ii].intensity);

                        strncpy(_targ1, (_prefix + "innerConeCos").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform1f(glGetUniformLocation(program, _targ1),
                                    asset->lights[ii].innerConeAngle.has_value() ? std::cos(asset->lights[ii].innerConeAngle.value()) : -1.0f);

                        strncpy(_targ1, (_prefix + "outerConeCos").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform1f(glGetUniformLocation(program, _targ1),
                                    asset->lights[ii].outerConeAngle.has_value() ? std::cos(asset->lights[ii].outerConeAngle.value()) : -1.0f);

                        strncpy(_targ1, (_prefix + "type").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform1i(glGetUniformLocation(program, _targ1), (int)asset->lights[ii].type);
                    }
                }
            }

            //if (viewer->overrideBaseColor){
            //    GL_CALL(glUniform4f(uniforms->baseColorFactor, viewer->overrideBaseColorFactor[0], viewer->overrideBaseColorFactor[1], viewer->overrideBaseColorFactor[2], viewer->overrideBaseColorFactor[3]));
            //} else {
            setup_uniform_color_alpha(uniforms->baseColorFactor, gltfMaterial.pbrData.baseColorFactor);
            setup_uniform_color(uniforms->emissiveFactor, gltfMaterial.emissiveFactor);
            //}
            GL_CALL(glUniform1f(uniforms->emissiveStrength, gltfMaterial.emissiveStrength));
            GL_CALL(glUniform1f(uniforms->roughnessFactor, gltfMaterial.pbrData.roughnessFactor));
            GL_CALL(glUniform1f(uniforms->metallicFactor,  gltfMaterial.pbrData.metallicFactor));

            GL_CALL(glUniform1f(uniforms->ior, gltfMaterial.ior));
            GL_CALL(glUniform1f(uniforms->dispersion, gltfMaterial.dispersion));

            if(gltfMaterial.pbrData.baseColorTexture.has_value()) _texnum = setup_texture(_texnum, _prim_data->albedoTexture,
                                                                                              _prim_data->baseColorTexcoordIndex, gltfMaterial.pbrData.baseColorTexture->transform,  uniforms->baseColorSampler,
                                                                                              uniforms->baseColorUVSet, uniforms->baseColorUVTransform);
            if(gltfMaterial.emissiveTexture.has_value()) _texnum = setup_texture(_texnum, _prim_data->emissiveTexture,
                                                                                     _prim_data->emissiveTexcoordIndex, gltfMaterial.emissiveTexture->transform, uniforms->emissiveSampler,
                                                                                     uniforms->emissiveUVSet, uniforms->emissiveUVTransform);
            if(gltfMaterial.pbrData.metallicRoughnessTexture.has_value()) _texnum = setup_texture(_texnum,
                                                                                                      _prim_data->metalRoughTexture, _prim_data->metallicRoughnessTexcoordIndex,
                                                                                                      gltfMaterial.pbrData.metallicRoughnessTexture->transform, uniforms->metallicRoughnessSampler,
                                                                                                      uniforms->metallicRoughnessUVSet, uniforms->metallicRoughnessUVTransform);
            if(gltfMaterial.occlusionTexture.has_value()) {
                GL_CALL(glUniform1f(uniforms->occlusionStrength, static_cast<float>(gltfMaterial.occlusionTexture->strength)));
                _texnum = setup_texture(_texnum, _prim_data->occlusionTexture, _prim_data->occlusionTexcoordIndex,
                                        gltfMaterial.occlusionTexture->transform, uniforms->occlusionSampler, uniforms->occlusionUVSet,
                                        uniforms->occlusionUVTransform);
            }

            if(gltfMaterial.normalTexture.has_value()) {
                GL_CALL(glUniform1f(uniforms->normalScale, static_cast<float>(gltfMaterial.normalTexture->scale)));
                _texnum = setup_texture(_texnum, _prim_data->normalTexture, _prim_data->normalTexcoordIndex,
                                        gltfMaterial.normalTexture->transform, uniforms->normalSampler, uniforms->normalUVSet, uniforms->normalUVTransform);
            }

            if(gltfMaterial.clearcoat) {
                GL_CALL(glUniform1f(uniforms->clearcoatFactor, static_cast<float>(gltfMaterial.clearcoat->clearcoatFactor)));
                GL_CALL(glUniform1f(uniforms->clearcoatRoughnessFactor,
                                    static_cast<float>(gltfMaterial.clearcoat->clearcoatRoughnessFactor)));

                if(gltfMaterial.clearcoat->clearcoatTexture.has_value()) _texnum = setup_texture(_texnum, _prim_data->clearcoatTexture,
                                                                                                     _prim_data->clearcoatTexcoordIndex, gltfMaterial.clearcoat->clearcoatTexture->transform, uniforms->clearcoatSampler,
                                                                                                     uniforms->clearcoatUVSet, uniforms->clearcoatUVTransform);
                if(gltfMaterial.clearcoat->clearcoatRoughnessTexture.has_value()) _texnum = setup_texture(_texnum,
                                                                                                              _prim_data->clearcoatRoughnessTexture, _prim_data->clearcoatRoughnessTexcoordIndex,
                                                                                                              gltfMaterial.clearcoat->clearcoatRoughnessTexture->transform, uniforms->clearcoatRoughnessSampler,
                                                                                                              uniforms->clearcoatRoughnessUVSet, uniforms->clearcoatRoughnessUVTransform);
                if(gltfMaterial.clearcoat->clearcoatNormalTexture.has_value()) {
                    GL_CALL(glUniform1f(uniforms->clearcoatNormalScale,
                                        static_cast<float>(gltfMaterial.clearcoat->clearcoatNormalTexture->scale)));
                    _texnum = setup_texture(_texnum, _prim_data->clearcoatNormalTexture, _prim_data->clearcoatNormalTexcoordIndex,
                                            gltfMaterial.clearcoat->clearcoatNormalTexture->transform, uniforms->clearcoatNormalSampler,
                                            uniforms->clearcoatNormalUVSet, uniforms->clearcoatNormalUVTransform);
                }
            }

            if(gltfMaterial.volume) {
                GL_CALL(glUniform1f(uniforms->attenuationDistance, gltfMaterial.volume->attenuationDistance));
                setup_uniform_color(uniforms->attenuationColor, gltfMaterial.volume->attenuationColor);
                GL_CALL(glUniform1f(uniforms->thickness, gltfMaterial.volume->thicknessFactor));
                if(gltfMaterial.volume->thicknessTexture.has_value()) {
                    _texnum = setup_texture(_texnum, _prim_data->thicknessTexture, _prim_data->thicknessTexcoordIndex,
                                            gltfMaterial.volume->thicknessTexture->transform, uniforms->thicknessSampler, uniforms->thicknessUVSet,
                                            uniforms->thicknessUVTransform);
                }
            }

            if(gltfMaterial.transmission) {
                GL_CALL(glUniform1f(uniforms->transmissionFactor, gltfMaterial.transmission->transmissionFactor));
                GL_CALL(glUniform2i(uniforms->screenSize, (int32_t)view_desc->render_width, (int32_t)view_desc->render_height));
                if(gltfMaterial.transmission->transmissionTexture.has_value()) _texnum = setup_texture(_texnum,
                                                                                                           _prim_data->transmissionTexture, _prim_data->transmissionTexcoordIndex,
                                                                                                           gltfMaterial.transmission->transmissionTexture->transform, uniforms->transmissionSampler, uniforms->transmissionUVSet,
                                                                                                           uniforms->transmissionUVTransform);
            }

            if(gltfMaterial.sheen) {
                //std::cout << "*** SHEEN FACTORS : Roughness: " << gltfMaterial.sheen->sheenRoughnessFactor << " : R/G/B " <<  gltfMaterial.sheen->sheenColorFactor[0] << " / " <<  gltfMaterial.sheen->sheenColorFactor[1] << " / " <<  gltfMaterial.sheen->sheenColorFactor[2] << " ***\n";
                setup_uniform_color(uniforms->sheenColorFactor, gltfMaterial.sheen->sheenColorFactor);
                GL_CALL(glUniform1f(uniforms->sheenRoughnessFactor, static_cast<float>(gltfMaterial.sheen->sheenRoughnessFactor)));
                if(gltfMaterial.sheen->sheenColorTexture.has_value()) {
                    std::cout << "***!!! MATERIAL HAS UNHANDLED SHEEN TEXTURE***\n";
                }
            }
            if(gltfMaterial.specular) {
                //std::cout << "*** SPECULAR FACTORS : specularFactor: " << gltfMaterial.specular->specularFactor << " : R/G/B " <<  gltfMaterial.specular->specularColorFactor[0] << " / " <<  gltfMaterial.specular->specularColorFactor[1] << " / " <<  gltfMaterial.specular->specularColorFactor[2] << " ***\n";
                setup_uniform_color(uniforms->specularColorFactor, gltfMaterial.specular->specularColorFactor);
                GL_CALL(glUniform1f(uniforms->specularFactor, static_cast<float>(gltfMaterial.specular->specularFactor)));
                if(gltfMaterial.specular->specularTexture.has_value()) {
                    std::cout << "***!!! MATERIAL HAS UNHANDLED SPECULAR TEXTURE***\n";
                }
                if(gltfMaterial.specular->specularColorTexture.has_value()) {
                    std::cout << "***!!! MATERIAL HAS UNHANDLED SPECULAR COLOR TEXTURE***\n";
                }
            }

            if(gltfMaterial.specularGlossiness) {
                std::cout <<
                          "*** WARNING: MODEL USES OUTDATED LEGACY MODE PBR_SPECULARGLOSS - PLEASE UPDATE THIS MODEL TO A NEW SHADING MODEL ***\n";
                setup_uniform_color_alpha(uniforms->diffuseFactor, gltfMaterial.specularGlossiness->diffuseFactor);
                setup_uniform_color(uniforms->specularFactor, gltfMaterial.specularGlossiness->specularFactor);
                GL_CALL(glUniform1f(uniforms->glossinessFactor, static_cast<float>(gltfMaterial.specularGlossiness->glossinessFactor)));
                if(gltfMaterial.specularGlossiness->diffuseTexture.has_value()) _texnum = setup_texture(_texnum,
                                                                                                            _prim_data->diffuseTexture, _prim_data->diffuseTexcoordIndex,
                                                                                                            gltfMaterial.specularGlossiness->diffuseTexture->transform, uniforms->diffuseSampler, uniforms->diffuseUVSet,
                                                                                                            uniforms->diffuseUVTransform);
                if(gltfMaterial.specularGlossiness->specularGlossinessTexture.has_value()) _texnum = setup_texture(_texnum,
                                                                                                                       _prim_data->specularGlossinessTexture, _prim_data->specularGlossinessTexcoordIndex,
                                                                                                                       gltfMaterial.specularGlossiness->specularGlossinessTexture->transform, uniforms->specularGlossinessSampler,
                                                                                                                       uniforms->specularGlossinessUVSet, uniforms->specularGlossinessUVTransform);
            }

#ifdef FASTGLTF_DIFFUSE_TRANSMISSION_SUPPORT
            if(gltfMaterial.diffuseTransmission) {
                std::cout << "*** DIFFUSE TRANSMISSION : factor : " << gltfMaterial.diffuseTransmission->diffuseTransmissionFactor <<
                          " : R/G/B " <<  gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor[0] << " / " <<
                          gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor[1] << " / " <<
                          gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor[2] << " ***\n";
                setup_uniform_color(uniforms->diffuseTransmissionColorFactor,
                                    gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor);
                GL_CALL(glUniform1f(uniforms->diffuseTransmissionFactor,
                                    static_cast<float>(gltfMaterial.diffuseTransmission->diffuseTransmissionFactor)));
                if(gltfMaterial.diffuseTransmission->diffuseTransmissionTexture.has_value()) _texnum = setup_texture(_texnum,
                                                                                                                         _prim_data->diffuseTransmissionTexture, _prim_data->diffuseTransmissionTexcoordIndex,
                                                                                                                         gltfMaterial.diffuseTransmission->diffuseTransmissionTexture->transform, uniforms->diffuseTransmissionSampler,
                                                                                                                         uniforms->diffuseTransmissionUVSet, uniforms->diffuseTransmissionUVTransform);
                if(gltfMaterial.diffuseTransmission->diffuseTransmissionColorTexture.has_value()) _texnum = setup_texture(_texnum,
                                                                                                                              _prim_data->diffuseTransmissionColorTexture, _prim_data->diffuseTransmissionColorTexcoordIndex,
                                                                                                                              gltfMaterial.diffuseTransmission->diffuseTransmissionColorTexture->transform, uniforms->diffuseTransmissionColorSampler,
                                                                                                                              uniforms->diffuseTransmissionColorUVSet, uniforms->diffuseTransmissionColorUVTransform);
            }
#endif
        }
        const auto & vstate = get_viewer_state(viewer);
        if(!is_transmission_pass && vstate->renderOpaqueBuffer) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, vstate->opaque_render_state.texture));
            GL_CALL(glUniform1i(uniforms->transmissionFramebufferSampler, _texnum));
            GL_CALL(glUniform2i(uniforms->transmissionFramebufferSize, (int32_t)vstate->metrics.opaqueFramebufferWidth,
                                (int32_t)vstate->metrics.opaqueFramebufferHeight));
            _texnum++;
        }

        if(node.skinIndex.has_value()) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, lv_gltf_data_get_skin_texture_at(gltf_data, node.skinIndex.value())));
            GL_CALL(glUniform1i(uniforms->jointsSampler, _texnum));
            _texnum++;
        }
        if(env_tex.diffuse != GL_NONE) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, env_tex.diffuse));
            GL_CALL(glUniform1i(uniforms->envDiffuseSampler, _texnum++));
        }
        if(env_tex.specular != GL_NONE) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, env_tex.specular));
            GL_CALL(glUniform1i(uniforms->envSpecularSampler, _texnum++));
        }
        if(env_tex.sheen != GL_NONE) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, env_tex.sheen));
            GL_CALL(glUniform1i(uniforms->envSheenSampler, _texnum++));
        }
        if(env_tex.ggxLut != GL_NONE) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, env_tex.ggxLut));
            GL_CALL(glUniform1i(uniforms->envGgxLutSampler, _texnum++));
        }
        if(env_tex.charlieLut != GL_NONE) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, env_tex.charlieLut));
            GL_CALL(glUniform1i(uniforms->envCharlieLutSampler, _texnum++));
        }

    }

    std::size_t index_count = 0;
    auto & indexAccessor = asset->accessors[asset->meshes[mesh_index].primitives[prim_num].indicesAccessor.value()];
    if(indexAccessor.bufferViewIndex.has_value()) {
        index_count = (uint32_t)indexAccessor.count;
    }
    if(index_count > 0) {
        GL_CALL(glDrawElements(_prim_data->primitiveType, index_count, _prim_data->indexType, 0));
    }
}

void lv_gltf_view_reset_between_models(lv_gltf_view_t * viewer)
{
    const auto & vstate =    get_viewer_state(viewer);
    if(vstate->render_state_ready) {
        setup_cleanup_opengl_output(&vstate->render_state);
        vstate->render_state_ready = false;
    }
    vstate->renderOpaqueBuffer = false;
    _ibmBySkinThenNode.clear();
}

void lv_gltf_view_recache_all_transforms(lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data)
{
    const auto view_desc = lv_gltf_view_get_desc(viewer);
    const auto & asset =     lv_gltf_data_get_asset(gltf_data);
    int32_t PREF_CAM_NUM = std::min(view_desc->camera, (int32_t)lv_gltf_data_get_camera_count(gltf_data) - 1);
    int32_t anim_num = view_desc->anim;
    uint32_t sceneIndex = 0;

    gltf_data->last_camera_index = PREF_CAM_NUM;
    lv_gltf_data_clear_transform_cache(gltf_data);
    gltf_data->current_camera_index = -1;
    gltf_data->has_any_cameras = false;
    auto tmat = fastgltf::math::fmat4x4{};
    auto cammat =  fastgltf::math::fmat4x4{};
    fastgltf::custom_iterateSceneNodes(*asset, sceneIndex, &tmat, [&](fastgltf::Node & node,
                                                                      fastgltf::math::fmat4x4 & parentworldmatrix,
                                                                      fastgltf::math::fmat4x4 & localmatrix) {
        bool made_changes = false;
        bool made_rotation_changes = false;
        if(animation_get_channel_set(anim_num, gltf_data, node)->size() > 0) {
            animation_matrix_apply(gltf_data->local_timestamp, anim_num, gltf_data, node, localmatrix);
            made_changes = true;
        }
        if(gltf_data->node_binds.find(&node) != gltf_data->node_binds.end()) {
            lv_gltf_bind_t * current_override = gltf_data->node_binds[&node];
            fastgltf::math::fvec3 local_pos;
            fastgltf::math::fquat local_quat;
            fastgltf::math::fvec3 local_scale;
            fastgltf::math::decomposeTransformMatrix(localmatrix, local_scale, local_quat, local_pos);
            fastgltf::math::fvec3 local_rot = quaternionToEuler(local_quat);

            // Traverse through all linked overrides
            while(current_override != nullptr) {
                if(current_override->prop == LV_GLTF_BIND_PROP_ROTATION) {
                    if(current_override->dir) {
                        current_override->data[0] = local_rot[0];
                        current_override->data[1] = local_rot[1];
                        current_override->data[2] = local_rot[2];
                    }
                    else {
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_1) local_rot[0] = current_override->data[0];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_2) local_rot[1] = current_override->data[1];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_3) local_rot[2] = current_override->data[2];
                        made_changes = true;
                        made_rotation_changes = true;
                    }
                }
                else if(current_override->prop == LV_GLTF_BIND_PROP_POSITION) {
                    if(current_override->dir) {
                        current_override->data[0] = local_pos[0];
                        current_override->data[1] = local_pos[1];
                        current_override->data[2] = local_pos[2];
                    }
                    else {
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_1) local_pos[0] = current_override->data[0];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_2) local_pos[1] = current_override->data[1];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_3) local_pos[2] = current_override->data[2];
                        made_changes = true;
                    }
                }
                else if(current_override->prop == LV_GLTF_BIND_PROP_WORLD_POSITION) {
                    fastgltf::math::fvec3 world_pos;
                    fastgltf::math::fquat world_quat;
                    fastgltf::math::fvec3 world_scale;
                    fastgltf::math::decomposeTransformMatrix(parentworldmatrix * localmatrix, world_scale, world_quat, world_pos);
                    //fastgltf::math::fvec3 world_rot = quaternionToEuler(world_quat);

                    if(current_override->dir) {
                        current_override->data[0] = world_pos[0];
                        current_override->data[1] = world_pos[1];
                        current_override->data[2] = world_pos[2];
                    } /*else {
                        if(current_override->data_mask & LV_GLTF_OVERRIDE_MASK_CHANNEL_1) world_pos[0] = current_override->data[0];
                        if(current_override->data_mask & LV_GLTF_OVERRIDE_MASK_CHANNEL_2) world_pos[1] = current_override->data[1];
                        if(current_override->data_mask & LV_GLTF_OVERRIDE_MASK_CHANNEL_3) world_pos[2] = current_override->data[2];
                        made_changes = true;
                    }*/
                }
                else if(current_override->prop == LV_GLTF_BIND_PROP_SCALE) {
                    if(current_override->dir) {
                        current_override->data[0] = local_scale[0];
                        current_override->data[1] = local_scale[1];
                        current_override->data[2] = local_scale[2];
                    }
                    else {
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_1) local_scale[0] = current_override->data[0];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_2) local_scale[1] = current_override->data[1];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_3) local_scale[2] = current_override->data[2];
                        made_changes = true;
                    }
                }

                // Move to the next override in the linked list
                current_override = current_override->next_bind;
            }

            // Rebuild the local matrix after applying all overrides
            localmatrix =
                fastgltf::math::scale(
                    fastgltf::math::rotate(
                        fastgltf::math::translate(
                            fastgltf::math::fmat4x4(),
                            local_pos),
                        made_rotation_changes ? fastgltf::math::eulerToQuaternion(local_rot[0], local_rot[1], local_rot[2]) : local_quat),
                    local_scale);
        }

        if(made_changes || !lv_gltf_data_has_cached_transform(gltf_data, &node)) {
            lv_gltf_data_set_cached_transform(gltf_data, &node, parentworldmatrix * localmatrix);
        }
        if(node.cameraIndex.has_value() && (gltf_data->current_camera_index < PREF_CAM_NUM)) {
            gltf_data->has_any_cameras = true;
            gltf_data->current_camera_index += 1;
            if(gltf_data->current_camera_index == PREF_CAM_NUM) {
                cammat = (parentworldmatrix * localmatrix);
            }
        }
    });
    if(gltf_data->has_any_cameras) {
        gltf_data->view_pos[0] = cammat[3][0];
        gltf_data->view_pos[1] = cammat[3][1];
        gltf_data->view_pos[2] = cammat[3][2];
        gltf_data->view_mat = fastgltf::math::invert(cammat);
    }
    else gltf_data->current_camera_index = -1;

}

uint32_t lv_gltf_view_render(lv_opengl_shader_cache_t * shaders, lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data,
                             bool prepare_bg, uint32_t crop_left,  uint32_t crop_right,  uint32_t crop_top,  uint32_t crop_bottom)
{

    const auto & asset = lv_gltf_data_get_asset(gltf_data);
    const auto & vstate =    get_viewer_state(viewer);
    const auto view_desc = lv_gltf_view_get_desc(viewer);
    const auto & vopts =     &(vstate->options);
    const auto & vmetrics =  &(vstate->metrics);
    bool opt_draw_bg = prepare_bg && (view_desc->bg_mode == BG_ENVIRONMENT);
    bool opt_aa_this_frame = (view_desc->aa_mode == ANTIALIAS_CONSTANT) || ((view_desc->aa_mode == ANTIALIAS_NOT_MOVING) &&
                                                                            (gltf_data->_last_frame_no_motion == true)); //((_lastFrameNoMotion == true) && (_lastFrameWasAntialiased == false)));
    if(prepare_bg == false) {
        // If this data object is a secondary render pass, inherit the anti-alias setting for this frame from the first gltf_data drawn
        opt_aa_this_frame = view_desc->frame_was_antialiased;
    }

    lv_gltf_opengl_state_push();
    uint32_t sceneIndex = 0;
    gl_renwin_state_t _output;
    gl_renwin_state_t _opaque;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    view_desc->last_render_system_msec = ((start_time.tv_sec * 1000000) + start_time.tv_usec) / 1000;

    view_desc->frame_was_cached = true;
    view_desc->render_width = view_desc->width * (opt_aa_this_frame ? 2 : 1);
    view_desc->render_height = view_desc->height * (opt_aa_this_frame ? 2 : 1);
    bool size_changed = false;
    if(view_desc->width != viewer->_lastViewDesc.width) size_changed = true;
    if(view_desc->height != viewer->_lastViewDesc.height) size_changed = true;

    if((opt_aa_this_frame != gltf_data->last_frame_was_antialiased) || size_changed) {
        // Antialiasing state has changed since the last render
        if(prepare_bg == true) {
            if(vstate->render_state_ready) {
                setup_cleanup_opengl_output(&vstate->render_state);
                vstate->render_state = setup_primary_output((uint32_t)view_desc->render_width, (uint32_t)view_desc->render_height,
                                                            opt_aa_this_frame);
            }
        }
        gltf_data->last_frame_was_antialiased = opt_aa_this_frame;
    }

    if(opt_aa_this_frame) {
        crop_left = crop_left << 1;
        crop_right = crop_right << 1;
        crop_top = crop_top << 1;
        crop_bottom = crop_bottom << 1;
    }
    view_desc->frame_was_antialiased = opt_aa_this_frame;
    if(prepare_bg == true) {
        if(!vstate->render_state_ready) {
            _output = setup_primary_output((uint32_t)view_desc->render_width, (uint32_t)view_desc->render_height,
                                           opt_aa_this_frame);
            setup_finish_frame();
            vstate->render_state = _output;
        }
    }

    if(!gltf_data->nodes_parsed) {
        gltf_data->nodes_parsed = true;
        std::vector<int64_t> _used = std::vector<int64_t>();
        int64_t _max_index = 0;
        fastgltf::iterateSceneNodes(*asset, sceneIndex, fastgltf::math::fmat4x4(), [&](fastgltf::Node & node,
        fastgltf::math::fmat4x4 matrix) {
            // TO-DO: replace this iterate with one that doesn't bother with any matrix math.  Since this is a one time loop at start up, it's ok for now.
            LV_UNUSED(matrix);
            if(node.meshIndex) {
                auto & mesh_index = node.meshIndex.value();
                if(node.skinIndex) {
                    auto skin_index = node.skinIndex.value();
                    if(!lv_gltf_data_validated_skins_contains(gltf_data, skin_index)) {
                        lv_gltf_data_validate_skin(gltf_data, skin_index);
                        auto skin = asset->skins[skin_index];
                        std::size_t num_joints = skin.joints.size();
                        std::cout << "Skin #" << std::to_string(skin_index) << ": Joints: " << std::to_string(num_joints) << "\n";
                        if(skin.inverseBindMatrices) {
                            auto & _ibmVal = skin.inverseBindMatrices.value();
                            auto & _ibmAccessor = asset->accessors[_ibmVal];
                            if(_ibmAccessor.bufferViewIndex) {  // To-do: test if this gets confused when bufferViewIndex == 0
                                fastgltf::iterateAccessorWithIndex<fastgltf::math::fmat4x4>(*asset, _ibmAccessor, [&](fastgltf::math::fmat4x4 _matrix,
                                std::size_t idx) {
                                    auto & _jointNode = asset->nodes[skin.joints[idx]];
                                    _ibmBySkinThenNode[skin_index][&_jointNode] = _matrix;
                                });
                            }
                        }
                    }
                }
                for(size_t mp = 0; mp < asset->meshes[mesh_index].primitives.size(); mp++) {
                    auto & _prim_gltf_data = asset->meshes[mesh_index].primitives[mp];
                    auto & mappings = _prim_gltf_data.mappings;
                    int64_t materialIndex = (!mappings.empty() &&
                                             mappings[vopts->materialVariant]) ?  mappings[vopts->materialVariant].value() + 1 : ((_prim_gltf_data.materialIndex) ?
                                                                                                                                  (_prim_gltf_data.materialIndex.value() + 1) : 0);
                    if(materialIndex < 0) {
                        lv_gltf_data_add_opaque_node_primitive(gltf_data, 0, &node, mp);
                    }
                    else {
                        auto & material = asset->materials[materialIndex - 1];
                        if(material.transmission) vstate->renderOpaqueBuffer = true;
                        if(material.alphaMode == fastgltf::AlphaMode::Blend || (material.transmission != NULL)) {
                            lv_gltf_data_add_blended_node_primitive(gltf_data, materialIndex + 1, &node, mp);
                        }
                        else {
                            lv_gltf_data_add_opaque_node_primitive(gltf_data, materialIndex + 1, &node, mp);
                        }
                        _max_index = std::max(_max_index, materialIndex);
                    }
                }
            }
        });

        lv_gltf_data_init_shaders(gltf_data, _max_index);
        setup_compile_and_load_bg_shader(shaders);
        fastgltf::iterateSceneNodes(*asset, sceneIndex, fastgltf::math::fmat4x4(), [&](fastgltf::Node & node,
        fastgltf::math::fmat4x4 matrix) {
            LV_UNUSED(matrix);
            if(node.meshIndex) {
                auto & mesh_index = node.meshIndex.value();
                for(uint64_t mp = 0; mp < asset->meshes[mesh_index].primitives.size(); mp++) {
                    auto & _prim_gltf_data = asset->meshes[mesh_index].primitives[mp];
                    auto & mappings = _prim_gltf_data.mappings;
                    int64_t materialIndex = (!mappings.empty() &&
                                             mappings[vopts->materialVariant]) ?  mappings[vopts->materialVariant].value() + 1 : ((_prim_gltf_data.materialIndex) ?
                                                                                                                                  (_prim_gltf_data.materialIndex.value() + 1) : 0);
                    const auto & _ss = lv_gltf_data_get_shader_set(gltf_data, materialIndex);
                    if(materialIndex > -1 && _ss->ready == false) {
                        lv_gltf_data_injest_discover_defines(gltf_data, &node, &_prim_gltf_data);
                        auto _shaderset  = setup_compile_and_load_shaders(shaders);
                        auto _unilocs = lv_gltf_uniform_locs();
                        setup_uniform_locations(&_unilocs, _shaderset.program);
                        lv_gltf_data_set_shader(gltf_data, materialIndex, _unilocs, _shaderset);
                    }
                }
            }
        });
    }

    if(!vstate->render_state_ready) {
        if(prepare_bg == true) {
            vstate->render_state_ready = true;
            if(vstate->renderOpaqueBuffer) {
                std::cout << "**** CREATING OPAQUE RENDER BUFFER OBJECTS ****\n";
                _opaque = setup_opaque_output(vmetrics->opaqueFramebufferWidth, vmetrics->opaqueFramebufferHeight);
                vstate->opaque_render_state = _opaque;
                setup_finish_frame();
            }
        }
    }
    bool _motionDirty = false;
    if(view_desc->dirty) {
        //std::cout << "DIRTY VIEW TRIGGER WINDOW MOTION\n";
        _motionDirty = true;
    }
    view_desc->dirty = false;

    _output = vstate->render_state;
    int32_t anim_num = view_desc->anim;
    if((anim_num >= 0) && ((int64_t)lv_gltf_data_get_animation_count(gltf_data) > anim_num)) {
        if(std::abs(view_desc->timestep) > 0.0001f) {
            //std::cout << "ACTIVE ANIMATION TRIGGER WINDOW MOTION\n";
            gltf_data->local_timestamp += view_desc->timestep;
            _motionDirty = true;
        }
        if(gltf_data->last_anim_num != anim_num) {
            gltf_data->cur_anim_maxtime = lv_gltf_animation_get_total_time(gltf_data, anim_num);
            gltf_data->last_anim_num = anim_num;
        }
        if(gltf_data->local_timestamp >= gltf_data->cur_anim_maxtime) gltf_data->local_timestamp = 0.05f;
        else if(gltf_data->local_timestamp < 0.0f) gltf_data->local_timestamp = gltf_data->cur_anim_maxtime - 0.05f;
        //std::cout << "Animation #" << std::to_string(anim_num) << " | Time = " << std::to_string(local_timestamp) << "\n";
    }

    // TODO: check if the override actually affects the transform and that the affected object is visible in the scene

    //if (lv_gltf_compare_viewer_desc(view_desc, &(gltf_data->_lastViewDesc))) {
    //    std::cout << "VIEWER DESC CHANGE TRIGGER WINDOW MOTION\n";
    //    _motionDirty = true;
    //    lv_gltf_copy_viewer_desc(view_desc, &(gltf_data->_lastViewDesc));
    //}
    lv_gltf_copy_viewer_desc(view_desc, &(viewer->_lastViewDesc));

    bool ___lastFrameNoMotion = gltf_data->__last_frame_no_motion;
    gltf_data->__last_frame_no_motion = gltf_data->_last_frame_no_motion;
    gltf_data->_last_frame_no_motion = true;
    int32_t PREF_CAM_NUM = LV_MIN(view_desc->camera, (int32_t)lv_gltf_data_get_camera_count(gltf_data) - 1);
    if(_motionDirty || (PREF_CAM_NUM != gltf_data->last_camera_index) ||
       lv_gltf_data_transform_cache_is_empty(gltf_data))  {
        //printf("View info: focal x/y/z = %.2f/%.2f/%.2f | pitch/yaw/distance = %.2f/%.2f/%.2f\n", view_desc->focal_x, view_desc->focal_y, view_desc->focal_z, view_desc->pitch, view_desc->yaw, view_desc->distance );
        gltf_data->_last_frame_no_motion = false;
        _motionDirty = false;
        lv_gltf_view_recache_all_transforms(viewer, gltf_data);
    }

    if((gltf_data->_last_frame_no_motion == true) && (gltf_data->__last_frame_no_motion == true) &&
       (___lastFrameNoMotion == true)) {
        // Nothing changed at all, return the previous output frame
        setup_finish_frame();
        lv_gltf_opengl_state_pop();
        return _output.texture;
    }

    uint32_t _ss = lv_gltf_data_get_skins_size(gltf_data);
    if(_ss > 0) {
        lv_gltf_data_destroy_textures(gltf_data);
        uint64_t i = 0u;
        uint32_t SIZEOF_16FLOATS = sizeof(float) * 16;
        while(i < _ss) {
            auto skinIndex = lv_gltf_data_get_skin(gltf_data, i);
            auto skin = asset->skins[skinIndex];
            auto _ibm = _ibmBySkinThenNode[skinIndex];

            std::size_t num_joints = skin.joints.size();
            std::size_t _tex_width = std::ceil(std::sqrt((float)num_joints * 8.0f));

            GLuint rtex = lv_gltf_data_create_texture(gltf_data);
            GL_CALL(glBindTexture(GL_TEXTURE_2D, rtex));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            /* TODO: perf: Avoid doing memory allocations inside loops */
            float * _data = new float[_tex_width * _tex_width * 4];
            std::size_t _dpos = 0;
            for(uint64_t j = 0; j < num_joints; j++) {
                auto & _jointNode = asset->nodes[skin.joints[j]];
                fastgltf::math::fmat4x4 _finalJointMat = lv_gltf_data_get_cached_transform(gltf_data,
                                                                                           &_jointNode) * _ibm[&_jointNode];  // _ibmBySkinThenNode[skinIndex][&_jointNode];
                std::memcpy(&_data[_dpos], _finalJointMat.data(), SIZEOF_16FLOATS); // Copy final joint matrix
                std::memcpy(&_data[_dpos + 16], fastgltf::math::transpose(fastgltf::math::invert(_finalJointMat)).data(),
                            SIZEOF_16FLOATS);   // Copy normal matrix
                _dpos += 32;
            }
            GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _tex_width, _tex_width, 0, GL_RGBA, GL_FLOAT, _data));
            GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
            delete[] _data;
            ++i;
        }
    }

    NodeDistanceVector distance_sort_nodes;

    for(const auto & kv : gltf_data->blended_nodes_by_material_index) {
        for(const auto & pair : kv.second) {
            auto node = pair.first;
            auto new_node = NodeIndexDistancePair(fastgltf::math::length(gltf_data->view_pos - lv_gltf_data_get_centerpoint(
                                                                             gltf_data, lv_gltf_data_get_cached_transform(gltf_data, node), node->meshIndex.value(), pair.second)),
                                                  NodeIndexPair(node, pair.second));
            distance_sort_nodes.push_back(new_node);
        }
    }
    // Sort __distance_sort_nodes by the first member (distance)
    /*std::sort(gltf_data->distance_sort_nodes.begin(), gltf_data->distance_sort_nodes.end(),*/
    std::sort(distance_sort_nodes.begin(), distance_sort_nodes.end(),
    [](const NodeIndexDistancePair & a, const NodeIndexDistancePair & b) {
        return a.first < b.first;
    });

    gltf_data->last_material_index = 99999;  // Reset the last material index to an unused value once per frame at the start
    if(vstate->renderOpaqueBuffer) {
        if(gltf_data->has_any_cameras) setup_view_proj_matrix_from_camera(viewer, gltf_data->current_camera_index, view_desc,
                                                                              gltf_data->view_mat, gltf_data->view_pos, gltf_data, true);
        else setup_view_proj_matrix(viewer, view_desc, gltf_data, true);
        _opaque = vstate->opaque_render_state;
        if(setup_restore_opaque_output(view_desc, _opaque, vmetrics->opaqueFramebufferWidth, vmetrics->opaqueFramebufferHeight,
                                       prepare_bg)) {
            // Should drawing this frame be canceled due to GL_INVALID_OPERATION error from possibly conflicting update cycles?
            view_desc->error_frames += 1;
            std::cout << "CANCELED FRAME A\n";
            return _output.texture;
        }
        if(opt_draw_bg) setup_draw_environment_background(shaders, viewer,
                                                              view_desc->blur_bg * 0.4f); //    GL_CALL(glUseProgram(_shader_prog.bg_program));

        render_materials(viewer, gltf_data, shaders, gltf_data->opaque_nodes_by_material_index);
        for(const auto & node_distance_pair : distance_sort_nodes) {
            const auto & node_element = node_distance_pair.second;
            const auto & node = node_element.first;
            draw_primitive(node_element.second, view_desc, viewer, gltf_data, *node, node->meshIndex.value(),
                           lv_gltf_data_get_cached_transform(gltf_data, node), *(shaders->last_env), true);
        }

        GL_CALL(glBindTexture(GL_TEXTURE_2D, _opaque.texture));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
        setup_finish_frame();
    }

    if(gltf_data->has_any_cameras) setup_view_proj_matrix_from_camera(viewer, gltf_data->current_camera_index, view_desc,
                                                                          gltf_data->view_mat, gltf_data->view_pos, gltf_data, false);
    else setup_view_proj_matrix(viewer, view_desc, gltf_data, false);
    viewer->envRotationAngle = shaders->last_env->angle;

    {
        if(setup_restore_primary_output(view_desc, _output, (uint32_t)view_desc->render_width - (crop_left + crop_right),
                                        (uint32_t)view_desc->render_height - (crop_top + crop_bottom), crop_left, crop_bottom, prepare_bg)) {
            // Should drawing this frame be canceled due to GL_INVALID_OPERATION error from possibly conflicting update cycles?
            view_desc->error_frames += 1;
            std::cout << "CANCELED FRAME B\n";
            return _output.texture;
        }
        if(opt_draw_bg) setup_draw_environment_background(shaders, viewer, view_desc->blur_bg);
        render_materials(viewer, gltf_data, shaders, gltf_data->opaque_nodes_by_material_index);

        for(const auto & node_distance_pair : distance_sort_nodes) {
            const auto & node_element = node_distance_pair.second; // Access the second member (NodeIndexPair)
            const auto & node = node_element.first;
            draw_primitive(node_element.second, view_desc, viewer, gltf_data, *node, node->meshIndex.value(),
                           lv_gltf_data_get_cached_transform(gltf_data, node), *(shaders->last_env), false);
        }
        if(opt_aa_this_frame) {
            GL_CALL(glBindTexture(GL_TEXTURE_2D, _output.texture));
            GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
        }
        setup_finish_frame();
    }
    lv_gltf_opengl_state_pop();
    view_desc->frame_was_cached = false;

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);
    view_desc->last_render_total_msec = (((finish_time.tv_sec * 1000000) + finish_time.tv_usec) / 1000) -
                                        view_desc->last_render_system_msec;

    return _output.texture;
}


static void render_materials(lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data, lv_gl_shader_manager_t * manager,
                             const MaterialIndexMap & map)
{

    for(const auto & kv : map) {
        for(const auto & pair : kv.second) {
            auto node = pair.first;
            draw_primitive(pair.second, &viewer->desc, viewer, gltf_data, *node, node->meshIndex.value(),
                           lv_gltf_data_get_cached_transform(gltf_data, node), *(manager->last_env), true);
        }
    }
}
