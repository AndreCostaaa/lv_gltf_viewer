#include "lib/fastgltf/include/fastgltf/types.hpp"

#include "__include/datatypes.h"
#include "lv_gltfview_internal.h"
#include <algorithm>

#ifndef __MESH_DATA_DEFINED
#define __MESH_DATA_DEFINED
struct MeshData {
    _GLUINT drawsBuffer;
    std::vector<Primitive> primitives;
};
#endif /* __MESH_DATA_DEFINED */

typedef pViewer         _VIEW;
typedef pGltf_data_t    _DATA;
typedef FVEC3           _VEC3;
typedef FVEC4           _VEC4;
typedef FMAT4           _MAT4;
typedef uint64_t        _UINT;
typedef MeshData        _MESH;
typedef NodePtr         _NODE;
#define _RET return

struct lv_gltfdata_struct {
    ASSET asset;
    bool load_success;
    gltf_probe_info probe;
    StringNodeMap* node_by_path;
    NodeIntMap* index_by_node;
    NodeVector* node_by_index;
    NodeTransformMap* node_transform_cache;
    MaterialIndexMap* opaque_nodes_by_materialIndex;
    MaterialIndexMap* blended_nodes_by_materialIndex;
    NodeDistanceVector* distance_sort_nodes;
    MapofTransformMap* ibmBySkinThenNode;
    NodeOverrideMap* overrides;
    LongVector* validated_skins;
    IntVector skin_tex;
    NodePrimCenterMap* local_mesh_to_center_points_by_primitive;
    
    float vertex_max[3];
    float vertex_min[3];
    float vertex_cen[3];
    float bound_radius;
    uint32_t vertex_count;
    uint32_t index_count;
    uint32_t __prim_type;
    uint32_t ebo;
    uint32_t vbo1;
    uint32_t vbo2;
    bool has_positions;
    bool has_normals;
    bool has_colors;
    bool has_uv1;
    bool has_uv2;
    bool has_joints1;
    bool has_joints2;
    bool has_weights1;
    bool has_weights2;
    bool has_morphing;
    bool has_skins;
    int32_t color_bytes;
    const char* filename;
    //bool render_state_ready;
};

struct _MatrixSet {
    FMAT4 viewMatrix = FMAT4(1.0f);
    FMAT4 projectionMatrix = FMAT4(1.0f);
    FMAT4 viewProjectionMatrix = FMAT4(1.0f);
};

struct lv_gltfview_struct {
    ASSET asset;
    
    _ViewerState state;
    _MatrixSet mats;

    std::vector<_GLUINT> bufferAllocations;
    std::vector<MeshData> meshes;
    std::vector<Texture> textures;
    std::vector<FMAT4> cameras;
    std::vector<_GLUINT> materialBuffers;
    std::vector<UniformLocs> shaderUniforms;
    std::vector<gl_renwin_shaderset_t> shaderSets;

    FVEC4 overrideBaseColorFactor = FVEC4(1.0f);
    FVEC3 direction = FVEC3(0.0f, 0.0f, -1.0f);
    FVEC3 cameraPos = FVEC3(0.0f, 0.0f, 0.0f);
    fastgltf::Optional<std::size_t> cameraIndex = std::nullopt;

    float envRotationAngle = 0.f;

    gl_viewer_desc_t desc;
};

MeshData* lv_gltf_get_new_meshdata(_VIEW _viewer) {
    MeshData outMesh = {};
    _viewer->meshes.emplace_back(outMesh);
    return &(_viewer->meshes[_viewer->meshes.size() - 1 ]);
}

uint32_t get_gltf_datastruct_datasize(void) {
    return sizeof(lv_gltfdata_t);
}

uint32_t get_viewer_datasize(void) {
    return sizeof(lv_gltfview_t);
}

uint32_t get_primitive_datasize(void) {
    return sizeof(Primitive);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
void __init_gltf_datastruct(_DATA _DataStructMem, const char * gltf_path) {
    lv_gltfdata_t _newDataStruct;
    _newDataStruct.asset = ASSET();
    _newDataStruct.filename = gltf_path;
    _newDataStruct.load_success = false;  

    memcpy (_DataStructMem, &_newDataStruct, sizeof (lv_gltfdata_t));
    _DataStructMem->overrides = new std::map<fastgltf::Node *, _Override>();
    _DataStructMem->node_by_path = new StringNodeMap();
    _DataStructMem->index_by_node = new NodeIntMap();
    _DataStructMem->node_by_index = new NodeVector();
    _DataStructMem->node_transform_cache = new NodeTransformMap();
    _DataStructMem->opaque_nodes_by_materialIndex = new MaterialIndexMap();
    _DataStructMem->blended_nodes_by_materialIndex = new MaterialIndexMap();
    _DataStructMem->distance_sort_nodes = new NodeDistanceVector();
    _DataStructMem->ibmBySkinThenNode = new MapofTransformMap();
    _DataStructMem->validated_skins = new std::vector<int64_t>();
    _DataStructMem->local_mesh_to_center_points_by_primitive = new std::map<uint32_t, std::map<uint32_t, FVEC4>>();  
}

void init_viewer_struct(_VIEW _ViewerMem) {
    lv_gltfview_t _newViewer;
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
    memcpy (_ViewerMem, &_newViewer, sizeof (lv_gltfview_t));
}
#pragma GCC diagnostic pop

void __free_data_struct(_DATA _data) {
    _data->overrides->clear(); delete _data->overrides;
    _data->node_by_path->clear(); delete _data->node_by_path;
    _data->index_by_node->clear(); delete _data->index_by_node;
    _data->node_by_index->clear(); delete _data->node_by_index; 
    _data->node_transform_cache->clear(); delete _data->node_transform_cache;
    _data->opaque_nodes_by_materialIndex->clear(); delete _data->opaque_nodes_by_materialIndex;
    _data->blended_nodes_by_materialIndex->clear(); delete _data->blended_nodes_by_materialIndex;
    _data->distance_sort_nodes->clear(); delete _data->distance_sort_nodes;
    _data->ibmBySkinThenNode->clear(); delete _data->ibmBySkinThenNode;
    _data->validated_skins->clear(); delete _data->validated_skins;
    _data->local_mesh_to_center_points_by_primitive->clear();delete _data->local_mesh_to_center_points_by_primitive;
}

void __free_viewer_struct(_VIEW V) {
    V->meshes.erase(V->meshes.begin(), V->meshes.end());V->meshes.clear(); V->meshes.shrink_to_fit();
    V->textures.erase(V->textures.begin(), V->textures.end());V->textures.clear();V->textures.shrink_to_fit();
    V->bufferAllocations.erase(V->bufferAllocations.begin(), V->bufferAllocations.end());V->bufferAllocations.clear();V->bufferAllocations.shrink_to_fit();
    V->cameras.erase(V->cameras.begin(), V->cameras.end());V->cameras.clear();V->cameras.shrink_to_fit();
    V->materialBuffers.erase(V->materialBuffers.begin(), V->materialBuffers.end());V->materialBuffers.clear();V->materialBuffers.shrink_to_fit();
    V->shaderSets.erase(V->shaderSets.begin(), V->shaderSets.end());V->shaderSets.clear();V->shaderSets.shrink_to_fit();
    V->shaderUniforms.erase(V->shaderUniforms.begin(), V->shaderUniforms.end());V->shaderUniforms.clear();V->shaderUniforms.shrink_to_fit();
}

FVEC4 lv_gltf_get_primitive_centerpoint(_DATA ret_data, fastgltf::Mesh& mesh, uint32_t prim_num);

const char*     lv_gltf_get_filename        (_DATA D)         {_RET (D->filename);}
void*           get_asset                   (_DATA D)         {_RET &(D->asset);}
void            set_asset                   (_DATA D,ASSET A) {D->asset = std::move(A);}
void            set_matrix_view             (_VIEW V,_MAT4 M) {V->mats.viewMatrix = M;}
void            set_matrix_proj             (_VIEW V,_MAT4 M) {V->mats.projectionMatrix = M;}
void            set_matrix_viewproj         (_VIEW V,_MAT4 M) {V->mats.viewProjectionMatrix = M;}
_VEC3           get_cam_pos                 (_VIEW V)         {_RET (V->cameraPos); }
void*           get_meshdata_set            (_VIEW V)         {_RET &(V->meshes);}
uint32_t        get_output_framebuffer      (_VIEW V)         {_RET !V->state.render_state_ready ? V->state.render_state.framebuffer : 0;}
void*           get_matrix_view             (_VIEW V)         {_RET &(V->mats.viewMatrix);}
void*           get_matrix_proj             (_VIEW V)         {_RET &(V->mats.projectionMatrix);}
void*           get_matrix_viewproj         (_VIEW V)         {_RET &(V->mats.viewProjectionMatrix);}
void*           get_texdata_set             (_VIEW V)         {_RET &(V->textures);}
_ViewerOpts*    get_viewer_opts             (_VIEW V)         {_RET &(V->state.options);}
_ViewerMetrics* get_viewer_metrics          (_VIEW V)         {_RET &(V->state.metrics);}
_ViewerState*   get_viewer_state            (_VIEW V)         {_RET &(V->state);}
gl_viewer_desc_t* lv_gltfview_get_desc           (_VIEW V)         {_RET &(V->desc);}
_MatrixSet*     get_matrix_set              (_VIEW V)         {_RET &(V->mats);}
double          get_radius                  (_DATA D)         {_RET (double)D->bound_radius;}
int64_t         lv_gltf_get_int_radiusX1000 (_DATA D)         {_RET (int64_t)(D->bound_radius * 1000);}
float*          get_center                  (_DATA D)         {_RET D->vertex_cen;}
float*          get_bounds_min              (_DATA D)         {_RET D->vertex_min;}
float*          get_bounds_max              (_DATA D)         {_RET D->vertex_max;}
void*           get_skintex_set             (_DATA D)         {_RET &(D->skin_tex);}
int32_t         get_skintex_at              (_DATA D,_UINT I) {_RET D->skin_tex[I];}
uint64_t        get_shader_program          (_VIEW V,_UINT I) {_RET V->shaderSets[I].program;}
Texture*        get_texdata                 (_VIEW V,_UINT I) {_RET &(V->textures[I]);}
UniformLocs*    get_uniform_ids             (_VIEW V,_UINT I) {_RET &(V->shaderUniforms[I]);}
uint64_t        get_texdata_glid            (_VIEW V,_UINT I) {_RET get_texdata(V, I)->texture;}
void            allocate_index              (_DATA D,_UINT I) {(*D->node_by_index).resize(I);}
void            set_probe                   (_DATA D,gltf_probe_info _probe)    {D->probe = std::move(_probe);}
void            lv_gltf_set_node_at_path    (_DATA D,std::string P,_NODE N)   {(*D->node_by_path)[P] = N; }
void            lv_gltf_set_node_index      (_DATA D,_UINT I,_NODE N)   {(*D->node_by_index)[I] = N;  (*D->index_by_node)[N] = I;}
void*           get_prim_from_mesh          (MeshData* M, uint64_t I)   {_RET &(M->primitives[I]);}

_MAT4           get_cached_transform        (_DATA D,_NODE N)           {_RET ((*D->node_transform_cache)[N]);}
void            set_cached_transform        (_DATA D,_NODE N,_MAT4 M)   {(*D->node_transform_cache)[N] = M;}
void            clear_transform_cache       (_DATA D)                   { D->node_transform_cache->clear();}

void            recache_centerpoint         (_DATA D,_UINT I,int32_t P) { (*D->local_mesh_to_center_points_by_primitive)[I][P] = lv_gltf_get_primitive_centerpoint(D, D->asset.meshes[I], P); }
bool            centerpoint_cache_contains  (_DATA D,_UINT I,int32_t P) {_RET ((D->local_mesh_to_center_points_by_primitive->find(I) == D->local_mesh_to_center_points_by_primitive->end()) || ( (*D->local_mesh_to_center_points_by_primitive)[I].find(P) == (*D->local_mesh_to_center_points_by_primitive)[I].end())) ? false : true; }    
bool            validated_skins_contains    (_DATA D,int64_t I)  {_RET ((std::find(D->validated_skins->begin(), D->validated_skins->end(),I) != D->validated_skins->end()));}
void            validate_skin               (_DATA D,int64_t I)  {D->validated_skins->push_back(I);}
_UINT           get_skins_size              (_DATA D)  {_RET D->validated_skins->size();}
int32_t         get_skin                    (_DATA D, uint64_t I)  {_RET (*D->validated_skins)[I];}

void add_opaque_node_prim(_DATA D, _UINT I, _NODE N, int32_t P ) { (*D->opaque_nodes_by_materialIndex)[I].push_back(std::make_pair(N, std::as_const(P))); }
MaterialIndexMap::iterator get_opaque_begin(_DATA D) {_RET D->opaque_nodes_by_materialIndex->begin(); }
MaterialIndexMap::iterator get_opaque_end(_DATA D) {_RET D->opaque_nodes_by_materialIndex->end(); }

void add_blended_node_prim(_DATA D, _UINT I, _NODE N, int32_t P ) { (*D->blended_nodes_by_materialIndex)[I].push_back(std::make_pair(N, std::as_const(P))); }
MaterialIndexMap::iterator get_blended_begin(_DATA D) {_RET D->blended_nodes_by_materialIndex->begin(); }
MaterialIndexMap::iterator get_blended_end(_DATA D) {_RET D->blended_nodes_by_materialIndex->end(); }

void clear_distance_sort (_DATA D) { D->distance_sort_nodes->clear();}
void add_distance_sort_prim(_DATA D, NodeIndexDistancePair P ) { D->distance_sort_nodes->push_back(P); }
NodeDistanceVector::iterator get_distance_sort_begin(_DATA D) {_RET D->distance_sort_nodes->begin(); }
NodeDistanceVector::iterator get_distance_sort_end(_DATA D) {_RET D->distance_sort_nodes->end(); }

gl_renwin_shaderset_t* get_shader_set (_VIEW V,_UINT I) {_RET &(V->shaderSets[I]);}

_VEC3 get_cached_centerpoint(_DATA D, _UINT I, int32_t P, _MAT4 M) {
    FVEC4 tv = FVEC4((*D->local_mesh_to_center_points_by_primitive)[I][P]);
    tv[3] = 1.f;
    tv = M * tv ;
    return FVEC3(tv[0], tv[1], tv[2]);
}

void set_shader(_VIEW V, uint64_t _index, UniformLocs _uniforms, gl_renwin_shaderset_t _shaderset) {
    V->shaderUniforms[_index] = _uniforms;
    V->shaderSets[_index] = _shaderset;
}

void init_shaders(_VIEW V, uint64_t _max_index) {
    auto _prevsize = V->shaderSets.size(); 
    V->shaderSets.resize(_max_index+1);
    V->shaderUniforms.resize(_max_index+1);
    if (_prevsize < _max_index) {
        for (uint64_t _ii = _prevsize; _ii <= _max_index; _ii++){
            V->shaderSets[_ii] = gl_renwin_shaderset_t();
            V->shaderSets[_ii].ready = false; } }
}

void set_bounds_info(_DATA D, _VEC3 _vmin, _VEC3 _vmax, _VEC3 _vcen, float _radius) {
    { auto _d = _vmin.data(); D->vertex_min[0] = _d[0]; D->vertex_min[1] = _d[1]; D->vertex_min[2] = _d[2]; }
    { auto _d = _vmax.data(); D->vertex_max[0] = _d[0]; D->vertex_max[1] = _d[1]; D->vertex_max[2] = _d[2]; }
    { auto _d = _vcen.data(); D->vertex_cen[0] = _d[0]; D->vertex_cen[1] = _d[1]; D->vertex_cen[2] = _d[2]; }
    D->bound_radius = _radius;
}

void set_cam_pos(_VIEW V,float x,float y,float z) { V->cameraPos[0] = x; V->cameraPos[1] = y; V->cameraPos[2] = z; }
