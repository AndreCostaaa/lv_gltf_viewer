#ifndef LV_GLTFINTERNALINTERFACE_H
#define LV_GLTFINTERNALINTERFACE_H

#include "lv_gltfview_datatypes.h"
typedef pViewer         _VIEW;
typedef pGltf_data_t    _DATA;
typedef uint64_t        _UINT;
void*               get_asset (_DATA D);
void*               get_matrix_view(_VIEW _viewer);
void*               get_matrix_proj(_VIEW _viewer);
void*               get_matrix_viewproj(_VIEW _viewer);
void*               get_texdata_set(_VIEW _viewer);
Texture *           get_texdata(_VIEW _viewer, uint64_t _index);
uint64_t            get_texdata_glid(_VIEW _viewer, uint64_t _index);
_ViewerState*       get_viewer_state(_VIEW _viewer);
gl_viewer_desc_t*   get_viewer_desc(_VIEW _viewer);
_ViewerOpts*        get_viewer_opts(_VIEW _viewer);
UniformLocs*        get_uniform_ids(_VIEW _viewer, uint64_t _materialIndex);
uint64_t            get_shader_program(_VIEW _viewer, uint64_t _materialIndex);
_ViewerMetrics*     get_viewer_metrics(_VIEW _viewer);
gl_renwin_shaderset_t * get_shader_set(_VIEW _viewer, uint64_t _materialIndex);
float*              get_bounds_min (_DATA D);
float*              get_bounds_max (_DATA D);
float*              get_center(_DATA D);
double              get_radius(_DATA D);
const char*         lv_gltf_get_filename(_DATA D);
gltf_probe_info *   lv_gltfview_get_probe(_DATA _data);
bool centerpoint_cache_contains (_DATA D,_UINT I,int32_t P);

#define GET_ASSET(d)                ((ASSET*)get_asset(d))
#define PROBE(d)          ((gltf_probe_info*)lv_gltfview_get_probe(d))
#define TEXDSET(x)   ((std::vector<Texture>*)get_texdata_set(x))
#define TEXD(x,y)                 ((Texture*)get_texdata(x, y))
#define TEXDGLID(x,y)             ((uint64_t)get_texdata_glid(x, y))
#define MATRIXSET(v)           ((_MatrixSet*)get_matrix_set(v))
#define GET_VIEW_MAT(v)             ((FMAT4*)get_matrix_view(v))
#define GET_PROJ_MAT(v)             ((FMAT4*)get_matrix_proj(v))
#define GET_VIEWPROJ_MAT(v)         ((FMAT4*)get_matrix_viewproj(v))
#define SKINTEXS(d)             ((IntVector*)get_skintex_set(d))
#define MESHDSET(v) ((std::vector<MeshData>*)get_meshdata_set(v))
#define GET_PRIM_FROM_MESH(m, i)((Primitive*)get_prim_from_mesh(m,i))

void set_shader     (_VIEW _viewer, uint64_t _materialIndex, UniformLocs _uniforms, gl_renwin_shaderset_t _shaderset);
void set_probe      (_DATA _data, gltf_probe_info _probe);
void set_probe      (_DATA D,gltf_probe_info _probe);
void set_cam_pos    (_VIEW V,float x,float y,float z);
void allocate_index (_DATA D, _UINT I);
void recache_centerpoint (_DATA D,_UINT I,int32_t P);

#endif /*LV_GLTFINTERNALINTERFACE_H*/