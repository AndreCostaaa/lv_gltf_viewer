#ifndef LV_GLTFINTERNALDATATYPES_H
#define LV_GLTFINTERNALDATATYPES_H

#include "lv_gltfview_private.h"

#ifdef __cplusplus
//struct MeshData;
extern "C" {
#endif

struct _MatrixSet;

typedef pViewer         _VIEW;
typedef pGltf_data_t    _DATA;
typedef FVEC3           _VEC3;
typedef FVEC4           _VEC4;
typedef FMAT4           _MAT4;
typedef uint64_t        _UINT;
typedef MeshData        _MESH;
typedef NodePtr         _NODE;
#define _RET return

    uint32_t get_gltf_datastruct_datasize(void);
    uint32_t get_viewer_datasize(void);
    uint32_t get_primitive_datasize(void);
    void __init_gltf_datastruct(pGltf_data_t _DataStructMem, const char * gltf_path);
    void init_viewer_struct(pViewer _ViewerMem);
    void __free_viewer_struct(pViewer _viewer);

    MeshData*           lv_gltf_get_new_meshdata( _VIEW _viewer );
    void                lv_gltf_set_node_at_path(_DATA _data, std::string _path, NodePtr node);
    void                lv_gltf_set_node_index(_DATA _data, _UINT I, NodePtr node);
    int64_t             lv_gltf_get_int_radiusX1000 (_DATA gltf_data);
    
    gl_viewer_desc_t* lv_gltfview_get_desc           (_VIEW V);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTFINTERNALDATATYPES_H*/
