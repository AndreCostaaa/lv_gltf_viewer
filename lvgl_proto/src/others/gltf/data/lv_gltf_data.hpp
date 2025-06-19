#ifndef LV_GLTFDATA_HPP
#define LV_GLTFDATA_HPP

#include "lv_gltf_data_internal.h"
#include "sup/include/lv_gltf_data_datatypes.h"

#ifdef __cplusplus

struct lv_gltf_data_struct {
    ASSET * asset;
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
    IntVector* skin_tex;
    NodePrimCenterMap* local_mesh_to_center_points_by_primitive;
    
    //std::vector<_GLUINT> bufferAllocations;
    std::vector<MeshData>* meshes;
    std::vector<Texture>* textures;
    std::vector<FMAT4>* cameras;
    std::vector<_GLUINT>* materialBuffers;
    std::vector<UniformLocs>* shaderUniforms;
    std::vector<gl_renwin_shaderset_t>* shaderSets;

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


    // ---

    //gl_viewer_desc_t _lastViewDesc;
    bool has_any_cameras;
    int32_t current_camera_index;
    int32_t last_camera_index;
    fastgltf::Node * selected_camera_node;
    FMAT4 viewMat;
    FVEC3 viewPos;

    int32_t last_anim_num;
    float cur_anim_maxtime;
    float local_timestamp;

    uint64_t _lastMaterialIndex; 
    bool _lastPassWasTransmission;

    bool _lastFrameWasAntialiased;
    bool _lastFrameNoMotion;
    bool __lastFrameNoMotion;
    bool nodes_parsed;

    bool view_is_linked = false;
    lv_gltf_data_t * linked_view_source;
    //bool render_state_ready;
};




typedef lv_gltf_data_t * _DATA;
typedef uint64_t        _UINT;
typedef NodePtr             _NODE;
typedef FMAT4               _MAT4;
void*                   get_texdata_set(_DATA D);
Texture *               get_texdata(_DATA D, _UINT I);
uint64_t                get_texdata_glid(_DATA D, _UINT I);
UniformLocs*            get_uniform_ids(_DATA D, _UINT I);
uint64_t                get_shader_program(_DATA D, _UINT I);
gl_renwin_shaderset_t * get_shader_set(_DATA D, _UINT I);
float*                  get_bounds_min (_DATA D);
float*                  get_bounds_max (_DATA D);
float*                  setup_get_center(_DATA D);
double                  get_radius(_DATA D);
const char*             lv_gltf_get_filename(_DATA D);
gltf_probe_info *       lv_gltf_view_get_probe(_DATA _data);
bool centerpoint_cache_contains (_DATA D,_UINT I,int32_t P);
void*           get_prim_from_mesh          (MeshData* M, uint64_t I);

void*                   get_asset (_DATA D);
#define GET_ASSET(d)    ((ASSET*)get_asset(d))
#define PROBE(d)        ((gltf_probe_info*)lv_gltf_view_get_probe(d))
#define TEXDSET(x)      ((std::vector<Texture>*)get_texdata_set(x))
#define TEXD(x,y)       ((Texture*)get_texdata(x, y))
#define TEXDGLID(x,y)   ((_UINT)get_texdata_glid(x, y))
#define MATRIXSET(v)    ((_MatrixSet*)get_matrix_set(v))
#define SKINTEXS(d)     ((IntVector*)get_skintex_set(d))
#define GET_PRIM_FROM_MESH(m, i)    ((Primitive*)get_prim_from_mesh(m,i))

void set_probe          (_DATA D,gltf_probe_info _probe);
void allocate_index     (_DATA D,_UINT I);
void recache_centerpoint(_DATA D,_UINT I,int32_t P);
MeshData*  get_meshdata_num (_DATA D,_UINT I);
int32_t         get_skintex_at              (_DATA D,_UINT I);
void*           get_skintex_set             (_DATA D);

bool            validated_skins_contains    (_DATA D,int64_t I);
void            validate_skin               (_DATA D,int64_t I);

void add_opaque_node_prim(_DATA D, _UINT I, _NODE N, int32_t P );
MaterialIndexMap::iterator get_opaque_begin(_DATA D);
MaterialIndexMap::iterator get_opaque_end(_DATA D);

void add_blended_node_prim(_DATA D, _UINT I, _NODE N, int32_t P ) ;
MaterialIndexMap::iterator get_blended_begin(_DATA D);
MaterialIndexMap::iterator get_blended_end(_DATA D);

void clear_distance_sort (_DATA D);
void add_distance_sort_prim(_DATA D, NodeIndexDistancePair P );
NodeDistanceVector::iterator get_distance_sort_begin(_DATA D);
NodeDistanceVector::iterator get_distance_sort_end(_DATA D);

void            set_cached_transform        (_DATA D,_NODE N,_MAT4 M);
void            clear_transform_cache       (_DATA D);
_MAT4           get_cached_transform        (_DATA D,_NODE N);
bool            transform_cache_is_empty    (_DATA D);
_UINT           get_skins_size              (_DATA D);
int32_t         get_skin                    (_DATA D, uint64_t I);
void injest_discover_defines(_DATA data_obj, void *node, void *prim);

void set_shader(_DATA D, uint64_t _index, UniformLocs _uniforms, gl_renwin_shaderset_t _shaderset);
void init_shaders(_DATA D, uint64_t _max_index);
uint32_t get_gltf_datastruct_datasize(void);


#endif

#endif /*LV_GLTFVIEW_H*/
