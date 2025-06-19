#include "lib/fastgltf/include/fastgltf/types.hpp"

#include "../../data/sup/include/lv_gltf_data_datatypes.h"
#include "include/lv_gltf_view_datatypes.h"
#include "../lv_gltf_view_internal.h"
#include <algorithm>

typedef lv_gltf_view_t *    _VIEW;
typedef pGltf_data_t        _DATA;
typedef FVEC3               _VEC3;
typedef FVEC4               _VEC4;
typedef FMAT4               _MAT4;
typedef uint64_t            _UINT;
typedef NodePtr             _NODE;
#define _RET return

struct _MatrixSet {
    FMAT4 viewMatrix = FMAT4(1.0f);
    FMAT4 projectionMatrix = FMAT4(1.0f);
    FMAT4 viewProjectionMatrix = FMAT4(1.0f);
};

struct lv_gltf_view_struct {
    _ViewerState state;
    _MatrixSet mats;

    FVEC4 overrideBaseColorFactor = FVEC4(1.0f);
    FVEC3 direction = FVEC3(0.0f, 0.0f, -1.0f);
    FVEC3 cameraPos = FVEC3(0.0f, 0.0f, 0.0f);
    fastgltf::Optional<std::size_t> cameraIndex = std::nullopt;

    float envRotationAngle = 0.f;
    float bound_radius;

    gl_viewer_desc_t desc;
    gl_viewer_desc_t _lastViewDesc;

};

uint32_t get_viewer_datasize(void) {
    return sizeof(lv_gltf_view_t);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
void init_viewer_struct(_VIEW _ViewerMem) {
    lv_gltf_view_t _newViewer;
    auto _newMetrics = &_newViewer.state.metrics;
        _newMetrics->opaqueFramebufferWidth = 256;
        _newMetrics->opaqueFramebufferHeight = 256;
        _newMetrics->vertex_count = 0;
    auto _newDesc = &_newViewer.desc;
        _newDesc->pitch = 0.f;
        _newDesc->yaw = 0.f;
        _newDesc->distance = 1.f;
        _newDesc->width = 768;
        _newDesc->height = 592;
        _newDesc->focal_x = 0.f;
        _newDesc->focal_y = 0.f;
        _newDesc->focal_z = 0.f;
        _newDesc->exposure = 0.8f;
        _newDesc->env_pow = 1.8f;
        _newDesc->blur_bg = 0.2f;
        _newDesc->bg_mode = 0;
        _newDesc->aa_mode = 2;
        _newDesc->camera = 0;
        _newDesc->anim = 0;
        _newDesc->spin_degree_offset = 0.f;        
        _newDesc->timestep = 0.f;
        _newDesc->error_frames = 0;
        _newDesc->dirty = true;
        _newDesc->recenter_flag = true;
        _newDesc->frame_was_cached = false;
        _newDesc->frame_was_antialiased = false;

    _newViewer.state.options.sceneIndex = 0;
    _newViewer.state.options.materialVariant = 0;
    _newViewer.state.render_state_ready = false;
    _newViewer.state.renderOpaqueBuffer = false;
    _newViewer.cameraIndex = std::nullopt;
    _newViewer.envRotationAngle = 0.0f;
    memcpy (_ViewerMem, &_newViewer, sizeof (lv_gltf_view_t));
}
#pragma GCC diagnostic pop

void __free_viewer_struct(_VIEW V) {
    // Nothing to do here
}


void            set_matrix_view             (_VIEW V,_MAT4 M) {V->mats.viewMatrix = M;}
void            set_matrix_proj             (_VIEW V,_MAT4 M) {V->mats.projectionMatrix = M;}
void            set_matrix_viewproj         (_VIEW V,_MAT4 M) {V->mats.viewProjectionMatrix = M;}
_VEC3           get_cam_pos                 (_VIEW V)         {_RET (V->cameraPos); }
uint32_t        get_output_framebuffer      (_VIEW V)         {_RET !V->state.render_state_ready ? V->state.render_state.framebuffer : 0;}
void*           get_matrix_view             (_VIEW V)         {_RET &(V->mats.viewMatrix);}
void*           get_matrix_proj             (_VIEW V)         {_RET &(V->mats.projectionMatrix);}
void*           get_matrix_viewproj         (_VIEW V)         {_RET &(V->mats.viewProjectionMatrix);}
_ViewerOpts*    get_viewer_opts             (_VIEW V)         {_RET &(V->state.options);}
_ViewerMetrics* get_viewer_metrics          (_VIEW V)         {_RET &(V->state.metrics);}
_ViewerState*   get_viewer_state            (_VIEW V)         {_RET &(V->state);}
gl_viewer_desc_t* lv_gltf_view_get_desc      (_VIEW V)         {_RET &(V->desc);}
_MatrixSet*     get_matrix_set              (_VIEW V)         {_RET &(V->mats);}
double          get_view_radius             (_VIEW V)         {_RET (double)V->bound_radius;}

void set_cam_pos(_VIEW V,float x,float y,float z) { V->cameraPos[0] = x; V->cameraPos[1] = y; V->cameraPos[2] = z; }
