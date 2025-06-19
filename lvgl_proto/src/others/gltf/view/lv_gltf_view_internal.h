#ifndef LV_GLTFVIEW_PRIVATE_H
#define LV_GLTFVIEW_PRIVATE_H

#ifdef __cplusplus

#ifndef FASTGLTF_MATH_HPP
    namespace fastgltf { class Asset;
    #if defined(FASTGLTF_USE_64BIT_FLOAT) && FASTGLTF_USE_64BIT_FLOAT
        using num = double;
    #else
        using num = float;
    #endif
        namespace math {
            template <typename T>
            class mat; 
            template <size_t Rows, size_t Cols>
            using fmat = mat<float>; 
            using fmat4x4 = fmat<4, 4>;
            using fmat3x3 = fmat<3, 3>;

            template <typename T>
            class vec; 
            template <size_t Cols>
            using fvec = vec<float>; 
            using fvec2 = fvec<2>;
            using fvec3 = fvec<3>;
            using fvec4 = fvec<4>;
            using nvec2 = vec<num, 2>;
            using nvec3 = vec<num, 3>;
            using nvec4 = vec<num, 4>;
        }
        struct Node;
    }
#endif

#include <vector>
#include <map>

using UintVector = std::vector<uint32_t>;                         // Vector of int32_t's
using IntVector = std::vector<int32_t>;                         // Vector of int32_t's
using LongVector = std::vector<int64_t>;                        // Vector of int64_t's
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
#include "sup/include/lv_gltf_view_datatypes.h"
using NodeOverrideMap = std::map<NodePtr, lv_gltf_override_t>;           // Map of Overrides by Node

struct MeshData;

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

void*                   get_matrix_view(_VIEW V);
void*                   get_matrix_proj(_VIEW V);
void*                   get_matrix_viewproj(_VIEW V);
#define GET_VIEW_MAT(v)                 ((FMAT4*)get_matrix_view(v))
#define GET_PROJ_MAT(v)                 ((FMAT4*)get_matrix_proj(v))
#define GET_VIEWPROJ_MAT(v)             ((FMAT4*)get_matrix_viewproj(v))
_ViewerState*           get_viewer_state(_VIEW V);
gl_viewer_desc_t*       lv_gltf_view_get_desc(_VIEW V);
_ViewerOpts*            get_viewer_opts(_VIEW V);
_ViewerMetrics*         get_viewer_metrics(_VIEW V);

void set_shader         (_VIEW V,_UINT I, UniformLocs _uniforms, gl_renwin_shaderset_t _shaderset);
void set_cam_pos        (_VIEW V,float x,float y,float z);

#endif /* LV_GLTFVIEW_PRIVATE_H */


