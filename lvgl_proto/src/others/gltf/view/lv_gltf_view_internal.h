#ifndef LV_GLTFVIEW_PRIVATE_H
#define LV_GLTFVIEW_PRIVATE_H

#ifdef __cplusplus

#include "sup/include/lv_gltf_view_datatypes.h"
#include "../data/lv_gltf_data_internal.h"

#endif

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

typedef struct  {
    _ViewerOpts     options;
    _ViewerMetrics  metrics;
    gl_renwin_state_t render_state;
    gl_renwin_state_t opaque_render_state;
    bool render_state_ready;
    bool renderOpaqueBuffer;
} _ViewerState;

typedef lv_gltf_view_t * _VIEW;
typedef uint64_t        _UINT;

void*                       get_matrix_view(_VIEW V);
void*                       get_matrix_proj(_VIEW V);
void*                       get_matrix_viewproj(_VIEW V);
#define GET_VIEW_MAT(v)     ((FMAT4*)get_matrix_view(v))
#define GET_PROJ_MAT(v)     ((FMAT4*)get_matrix_proj(v))
#define GET_VIEWPROJ_MAT(v) ((FMAT4*)get_matrix_viewproj(v))

_ViewerState*       get_viewer_state(_VIEW V);
gl_viewer_desc_t*   lv_gltf_view_get_desc(_VIEW V);
_ViewerOpts*        get_viewer_opts(_VIEW V);
_ViewerMetrics*     get_viewer_metrics(_VIEW V);

void set_shader     (_VIEW V,_UINT I, UniformLocs _uniforms, gl_renwin_shaderset_t _shaderset);
void set_cam_pos    (_VIEW V,float x,float y,float z);

#endif /* LV_GLTFVIEW_PRIVATE_H */


