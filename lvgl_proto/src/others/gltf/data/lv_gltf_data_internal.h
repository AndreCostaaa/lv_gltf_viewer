#ifndef LV_GLTFDATA_PRIVATE_H
#define LV_GLTFDATA_PRIVATE_H

typedef uint32_t _GLENUM;
typedef uint32_t _GLUINT;
typedef int32_t  _GLINT;

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
#include "sup/include/lv_gltf_data_datatypes.h"
using NodeOverrideMap = std::map<NodePtr, lv_gltf_override_t>;           // Map of Overrides by Node

struct MeshData;
#endif

typedef struct {
    _GLUINT count;
    _GLUINT instanceCount;
    _GLUINT firstIndex;
    _GLINT baseVertex;
    _GLUINT baseInstance;
} IndirectDrawCommand;

typedef struct {
    IndirectDrawCommand draw;
    _GLENUM primitiveType;
    _GLENUM indexType;
    _GLUINT vertexArray;

    _GLUINT vertexBuffer;
    _GLUINT indexBuffer;

    _GLUINT materialUniformsIndex;
    _GLUINT albedoTexture;
    _GLUINT emissiveTexture;
    _GLUINT metalRoughTexture;
    _GLUINT occlusionTexture;
    _GLUINT normalTexture;
    _GLUINT diffuseTransmissionTexture;
    _GLUINT diffuseTransmissionColorTexture;
    _GLUINT transmissionTexture;
    _GLUINT transmissionTexcoordIndex;
    
    _GLINT baseColorTexcoordIndex;
    _GLINT emissiveTexcoordIndex;
    
    _GLINT metallicRoughnessTexcoordIndex;
    _GLINT occlusionTexcoordIndex;
    _GLINT normalTexcoordIndex;
    _GLINT diffuseTransmissionTexcoordIndex;
    _GLINT diffuseTransmissionColorTexcoordIndex;

    _GLINT clearcoatTexture;
    _GLINT clearcoatRoughnessTexture;
    _GLINT clearcoatNormalTexture;
    _GLINT clearcoatTexcoordIndex;
    _GLINT clearcoatRoughnessTexcoordIndex;
    _GLINT clearcoatNormalTexcoordIndex;

    _GLUINT thicknessTexture;
    _GLINT thicknessTexcoordIndex;

    _GLUINT diffuseTexture;
    _GLINT diffuseTexcoordIndex;

    _GLUINT specularGlossinessTexture;
    _GLINT specularGlossinessTexcoordIndex;

} Primitive;

typedef struct {
    _GLUINT texture;
} Texture;

typedef struct {
    _GLINT camera;
    _GLINT viewProjectionMatrixUniform;
    _GLINT modelMatrixUniform;
    _GLINT viewMatrixUniform;
    _GLINT projectionMatrixUniform;

    _GLINT envIntensity;
    _GLINT envDiffuseSampler;
    _GLINT envSpecularSampler;
    _GLINT envSheenSampler;
    _GLINT envGgxLutSampler;
    _GLINT envCharlieLutSampler;
    _GLINT envMipCount;
    
    _GLINT exposure;
    _GLINT roughnessFactor;

    _GLINT baseColorFactor;
    _GLINT baseColorSampler;
    _GLINT baseColorUVSet;
    _GLINT baseColorUVTransform;

    _GLINT emissiveFactor;
    _GLINT emissiveSampler;
    _GLINT emissiveUVSet;
    _GLINT emissiveUVTransform;
    _GLINT emissiveStrength;

    _GLINT metallicFactor;
    _GLINT metallicRoughnessSampler;
    _GLINT metallicRoughnessUVSet;
    _GLINT metallicRoughnessUVTransform;

    _GLINT occlusionStrength;
    _GLINT occlusionSampler;
    _GLINT occlusionUVSet;
    _GLINT occlusionUVTransform;

    _GLINT normalScale;
    _GLINT normalSampler;
    _GLINT normalUVSet;
    _GLINT normalUVTransform;

    _GLINT clearcoatFactor;
    _GLINT clearcoatRoughnessFactor;
    _GLINT clearcoatSampler;
    _GLINT clearcoatUVSet;
    _GLINT clearcoatUVTransform;
    _GLINT clearcoatRoughnessSampler;
    _GLINT clearcoatRoughnessUVSet;
    _GLINT clearcoatRoughnessUVTransform;
    _GLINT clearcoatNormalScale;
    _GLINT clearcoatNormalSampler;    
    _GLINT clearcoatNormalUVSet;
    _GLINT clearcoatNormalUVTransform;

    _GLINT thickness;
    _GLINT thicknessSampler;
    _GLINT thicknessUVSet;
    _GLINT thicknessUVTransform;

    _GLINT diffuseTransmissionSampler;
    _GLINT diffuseTransmissionUVSet;
    _GLINT diffuseTransmissionUVTransform;    

    _GLINT diffuseTransmissionColorSampler;
    _GLINT diffuseTransmissionColorUVSet;
    _GLINT diffuseTransmissionColorUVTransform;

    _GLINT sheenColorFactor;
    _GLINT sheenRoughnessFactor;

    _GLINT specularColorFactor;
    _GLINT specularFactor;

    _GLINT diffuseTransmissionColorFactor;
    _GLINT diffuseTransmissionFactor;

    _GLINT ior;
    _GLINT alphaCutoff;

    _GLINT dispersion;
    _GLINT screenSize;
    _GLINT transmissionFactor;
    //_GLINT transmissionScale;
    _GLINT transmissionSampler;
    _GLINT transmissionUVSet;
    _GLINT transmissionUVTransform;
    _GLINT transmissionFramebufferSampler;
    _GLINT transmissionFramebufferSize;

    _GLINT attenuationDistance;
    _GLINT attenuationColor;
    
    _GLINT jointsSampler;

    _GLINT diffuseFactor;
    //_GLINT specularFactor;
    _GLINT glossinessFactor;

    _GLINT diffuseSampler;
    _GLINT diffuseUVSet;
    _GLINT diffuseUVTransform;
    _GLINT specularGlossinessSampler;
    _GLINT specularGlossinessUVSet;
    _GLINT specularGlossinessUVTransform;
    
} UniformLocs;

typedef struct {
    bool ready;
    uint32_t program;
    uint32_t bg_program;
    uint32_t vert;
    uint32_t frag;
} gl_renwin_shaderset_t;

#endif /* LV_GLTFDATA_PRIVATE_H */

