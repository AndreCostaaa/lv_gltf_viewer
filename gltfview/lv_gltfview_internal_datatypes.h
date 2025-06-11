#ifndef LV_GLTFINTERNALDATATYPES_H
#define LV_GLTFINTERNALDATATYPES_H

#include <vector>
#include <map>

using IntVector = std::vector<int32_t>;                         // Vector of int32_t's
using LongVector = std::vector<int64_t>;                        // Vector of int64_t's
#include "lv_gltfview_fastgltf_stub.h"
using NodePtr = fastgltf::Node*;                                // Pointer to fastgltf::Node
using Transform = fastgltf::math::fmat4x4;                      // A standard 4x4 transform matrix
using NodeIndexPair = std::pair<NodePtr, int32_t>;              // Pair of Node pointer and int32_t
using NodeIndexDistancePair = std::pair<float, NodeIndexPair>;  // Pair of float and Node/Index pair
using NodePairVector = std::vector<NodeIndexPair>;              // Vector of NodeIndexPair
using NodeDistanceVector = std::vector<NodeIndexDistancePair>;  // Vector of NodeIndexDistancePair
using MaterialIndexMap = std::map<uint32_t, NodePairVector>;    // Map of uint32_t to NodePairVector
using NodeTransformMap = std::map<NodePtr, Transform>;          // Map of Node Pointers to Transforms
using MapofTransformMap = std::map<int32_t, NodeTransformMap>;  // Map of 4x4 Transform Maps by int32_t index (skin)
using StringNodeMap = std::map<std::string, NodePtr>;           // Map of Nodes by string (name)
using NodeIntMap = std::map<NodePtr, uint32_t>;                 // Map of Nodes by string (name)
using NodeVector = std::vector<NodePtr>;                        // Map of Nodes by string (name)
using NodePrimCenterMap = std::map<uint32_t, std::map<uint32_t, fastgltf::math::fvec4>>; // Map of Node Index to Map of Prim Index to CenterXYZ+RadiusW Vec4
#include "lv_gltfview_datatypes.h"
using NodeOverrideMap = std::map<NodePtr, _Override>;           // Map of Overrides by Node

#ifdef __cplusplus
struct MeshData;
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

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTFINTERNALDATATYPES_H*/
