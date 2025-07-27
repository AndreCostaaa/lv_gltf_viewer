#ifndef LV_GLTFVIEW_PRIVATE_H
#define LV_GLTFVIEW_PRIVATE_H

#include "fastgltf/math.hpp"
#include "lv_gltf_view.h"
#include "lv_gltf_view_internal.h"
#include <stdbool.h>
#include <stdint.h>

#include "../data/lv_gltf_data_internal.hpp"

#ifdef __cplusplus

#include <fastgltf/types.hpp>

struct _MatrixSet {
    fastgltf::math::fmat4x4 viewMatrix = fastgltf::math::fmat4x4(1.0f);
    fastgltf::math::fmat4x4 projectionMatrix = fastgltf::math::fmat4x4(1.0f);
    fastgltf::math::fmat4x4 viewProjectionMatrix = fastgltf::math::fmat4x4(1.0f);
};

typedef enum {
    ANTIALIAS_OFF = 0,
    ANTIALIAS_CONSTANT = 1,
    ANTIALIAS_NOT_MOVING = 2,
    ANTIALIAS_UNKNOWN = 999
} AntialiasingMode;

typedef enum {
    BG_CLEAR = 0,
    BG_SOLID = 1,
    BG_ENVIRONMENT = 2,
    BG_UNKNOWN = 999
} BackgroundMode;

typedef struct {
    bool overrideBaseColor;
    uint32_t materialVariant;
    uint64_t sceneIndex;
} _ViewerOpts;

typedef struct {
    uint64_t opaqueRenderTexture;
    uint64_t opaqueFramebuffer;
    uint64_t opaqueFramebufferScratch;
    uint64_t opaqueDepthTexture;
    uint64_t opaqueFramebufferWidth;
    uint64_t opaqueFramebufferHeight;
    uint64_t windowHeight;
    uint64_t windowWidth;
    float fWindowHeight;
    float fWindowWidth;
    uint64_t vertex_count;
} _ViewerMetrics;

typedef struct {
    bool init_success;
    uint32_t texture;
    uint32_t renderbuffer;
    unsigned framebuffer;
} gl_renwin_state_t;


typedef uint32_t (*render_func_t)(lv_gl_shader_manager_t *, lv_gltf_view_t *, lv_gltf_data_t *, ...);

typedef struct {
    float pitch;
    float yaw;
    float distance;
    float fov;                  // The vertical FOV, in degrees.  If this is zero, the view will be orthographic (non-perspective)
    int32_t width;              // The user specified width and height of this output
    int32_t height;
    int32_t render_width;       // If anti-aliasing is not applied this frame, these are the same as width/height, if antialiasing
    int32_t render_height;      // is enabled, these are width/height * antialias upscale power (currently 2.0)
    bool recenter_flag;
    float spin_degree_offset;   // This amount is added to the yaw and can be incremented overtime for a spinning platter effect
    float focal_x;
    float focal_y;
    float focal_z;
    uint8_t bg_r;               // The background color r/g/b/a - note the rgb components have affect on antialiased edges that border on empty space, even when alpha is zero.
    uint8_t bg_g;
    uint8_t bg_b;
    uint8_t bg_a;
    bool frame_was_cached;
    bool frame_was_antialiased;
    bool dirty;
    int32_t camera;             // -1 for default (first scene camera if available or platter if not), 0 = platter, 1+ = Camera index in the order it appeared within the current scene render.  Any value higher than the scene's camera count will be limited to the scene's camera count.
    int32_t anim;               // -1 for no animations, 0+ = Animation index.  Any value higher than the scene's animation count will be limited to the scene's animation count.
    float timestep;             // How far to step the current animation in seconds
    int32_t error_frames;       // temporary counter of how many times the texture failed to update in the past second
    int32_t aa_mode;            // The anti-aliasing mode: 0 = None, 1 = Always, 2 = When Moving or Animated
    int32_t bg_mode;            // The background mode: 0 = Clear, 1 = Solid Color, 2 = The Environment
    float blur_bg;              // How much to blur the environment background, between 0.0 and 1.0
    float env_pow;              // Environmental brightness, 1.0 default
    float exposure;             // Image exposure level, 1.0 default
    uint64_t last_render_system_msec;  // The system time of the last render in 1/1000th's of a second.
    uint64_t last_render_total_msec;  // The total time of the last render in 1/1000th's of a second.  (Note this does not include any time used outside of the render loop ie lv_refr_now() )
    render_func_t render_func;
} gl_viewer_desc_t;

struct gl_shader_light_t {
    float direction[3];   // Represents a vec3
    float range;          // Float value
    float color[3];       // Represents a vec3
    float intensity;      // Float value
    float position[3];    // Represents a vec3
    float innerConeCos;   // Float value
    float outerConeCos;   // Float value
    int type;             // Integer value
};

typedef struct  {
    _ViewerOpts     options;
    _ViewerMetrics  metrics;
    gl_renwin_state_t render_state;
    gl_renwin_state_t opaque_render_state;
    bool render_state_ready;
    bool renderOpaqueBuffer;
} _ViewerState;

struct _lv_gltf_view_t {
    _ViewerState state;
    _MatrixSet mats;

    fastgltf::math::fvec4 overrideBaseColorFactor = fastgltf::math::fvec4(1.0f);
    fastgltf::math::fvec3 direction = fastgltf::math::fvec3(0.0f, 0.0f, -1.0f);
    fastgltf::math::fvec3 cameraPos = fastgltf::math::fvec3(0.0f, 0.0f, 0.0f);
    fastgltf::Optional<std::size_t> cameraIndex = std::nullopt;

    float envRotationAngle = 0.f;
    float bound_radius;

    gl_viewer_desc_t desc;
    gl_viewer_desc_t _lastViewDesc;

};

void*                       get_matrix_view(lv_gltf_view_t* view);
void*                       get_matrix_proj(lv_gltf_view_t* view);
void*                       get_matrix_viewproj(lv_gltf_view_t* view);
#define GET_VIEW_MAT(v)     ((fastgltf::math::fmat4x4*)get_matrix_view(v))
#define GET_PROJ_MAT(v)     ((fastgltf::math::fmat4x4*)get_matrix_proj(v))
#define GET_VIEWPROJ_MAT(v) ((fastgltf::math::fmat4x4*)get_matrix_viewproj(v))

_ViewerState*       get_viewer_state(lv_gltf_view_t* view);
gl_viewer_desc_t*   lv_gltf_view_get_desc(lv_gltf_view_t* view);
_ViewerOpts*        get_viewer_opts(lv_gltf_view_t* view);
_ViewerMetrics*     get_viewer_metrics(lv_gltf_view_t* view);

void set_shader     (lv_gltf_view_t* view, uint64_t I, lv_gltf_uniform_locs _uniforms, lv_gltf_renwin_shaderset_t _shaderset);
void set_cam_pos    (lv_gltf_view_t* view,float x,float y,float z);
void __free_viewer_struct(lv_gltf_view_t* view);
void setup_environment_rotation_matrix(float env_rotation_angle, uint32_t shader_program);
void setup_uniform_color_alpha(GLint uniform_loc, fastgltf::math::nvec4 color);
void setup_uniform_color(GLint uniform_loc, fastgltf::math::nvec3 color);
uint32_t setup_texture(uint32_t tex_unit, uint32_t tex_name, int32_t tex_coord_index, 
                                    std::unique_ptr<fastgltf::TextureTransform>& tex_transform, 
                                    GLint sampler, GLint uv_set, GLint uv_transform);

void setup_cleanup_opengl_output(gl_renwin_state_t *state);

void lv_gltf_opengl_state_push(void);
void lv_gltf_opengl_state_pop(void);
gl_renwin_state_t setup_primary_output(uint32_t texture_width, uint32_t texture_height, bool mipmaps_enabled);
void setup_finish_frame(void);
void setup_compile_and_load_bg_shader(lv_opengl_shader_cache_t * shaders);
lv_gltf_renwin_shaderset_t setup_compile_and_load_shaders(lv_opengl_shader_cache_t * shaders);
void setup_uniform_locations(lv_gltf_uniform_locs* uniforms, uint32_t _shader_prog_program);
gl_renwin_state_t setup_opaque_output(uint32_t texture_width, uint32_t texture_height);
UintVector * animation_get_channel_set(std::size_t anim_num, lv_gltf_data_t * gltf_data,  fastgltf::Node& node);
void animation_matrix_apply(float timestamp, std::size_t anim_num, lv_gltf_data_t * gltf_data,  fastgltf::Node& node, fastgltf::math::fmat4x4& matrix);

void setup_view_proj_matrix_from_camera(lv_gltf_view_t *viewer, int32_t _cur_cam_num, 
                                         gl_viewer_desc_t *view_desc, const fastgltf::math::fmat4x4 view_mat, 
                                         const fastgltf::math::fvec3 view_pos, lv_gltf_data_t * gltf_data, 
                                         bool transmission_pass);

void setup_view_proj_matrix(lv_gltf_view_t *viewer, gl_viewer_desc_t *view_desc, 
                            lv_gltf_data_t * gltf_data, bool transmission_pass);
bool setup_restore_opaque_output( gl_viewer_desc_t *view_desc, gl_renwin_state_t _ret, uint32_t texture_w, uint32_t texture_h, bool prepare_bg);
void setup_draw_environment_background(lv_opengl_shader_cache_t * shaders, lv_gltf_view_t * viewer, float blur);

bool setup_restore_primary_output( gl_viewer_desc_t *view_desc, gl_renwin_state_t _ret, uint32_t texture_w, uint32_t texture_h, 
                                  uint32_t texture_offset_w, uint32_t texture_offset_h, bool prepare_bg);
/**
 * @brief Copy the viewer descriptor from one state to another.
 *
 * @param from_state Pointer to the source viewer descriptor.
 * @param to_state Pointer to the destination viewer descriptor.
 */
void lv_gltf_copy_viewer_desc(gl_viewer_desc_t* from_state, gl_viewer_desc_t* to_state);

/**
 * @brief Compare two viewer descriptors for equality.
 *
 * @param from_state Pointer to the first viewer descriptor.
 * @param to_state Pointer to the second viewer descriptor.
 * @return true if the descriptors are equal, false otherwise.
 */
bool lv_gltf_compare_viewer_desc(gl_viewer_desc_t* from_state, gl_viewer_desc_t* to_state);

/**
 * @brief Get a description of the viewer.
 *
 * @param V Pointer to the lv_gltf_view.
 * @return Pointer to the viewer description structure.
 */
gl_viewer_desc_t* lv_gltf_view_get_desc(lv_gltf_view_t * V);

fastgltf::math::fvec3 get_cam_pos(lv_gltf_view_t* view);
fastgltf::math::fvec3 animation_get_vec3_at_timestamp(lv_gltf_data_t * _data, fastgltf::AnimationSampler * sampler, float _seconds);

void set_matrix_view(lv_gltf_view_t* view, fastgltf::math::fmat4x4 M);
void set_matrix_proj(lv_gltf_view_t* view, fastgltf::math::fmat4x4 M);
void set_matrix_viewproj(lv_gltf_view_t* view, fastgltf::math::fmat4x4 M);

float lv_gltf_animation_get_total_time(lv_gltf_data_t * data, uint32_t animnum );

#endif 
#endif /* LV_GLTFVIEW_PRIVATE_H */


