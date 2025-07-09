#ifndef LV_GLTFDATA_PRIVATE_H
#define LV_GLTFDATA_PRIVATE_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <misc/lv_types.h>

typedef struct {
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLint baseVertex;
    GLuint baseInstance;
} IndirectDrawCommand;

typedef struct {
    IndirectDrawCommand draw;
    GLenum primitiveType;
    GLenum indexType;
    GLuint vertexArray;

    GLuint vertexBuffer;
    GLuint indexBuffer;

    GLuint materialUniformsIndex;
    GLuint albedoTexture;
    GLuint emissiveTexture;
    GLuint metalRoughTexture;
    GLuint occlusionTexture;
    GLuint normalTexture;
    GLuint diffuseTransmissionTexture;
    GLuint diffuseTransmissionColorTexture;
    GLuint transmissionTexture;
    GLuint transmissionTexcoordIndex;

    GLint baseColorTexcoordIndex;
    GLint emissiveTexcoordIndex;

    GLint metallicRoughnessTexcoordIndex;
    GLint occlusionTexcoordIndex;
    GLint normalTexcoordIndex;
    GLint diffuseTransmissionTexcoordIndex;
    GLint diffuseTransmissionColorTexcoordIndex;

    GLint clearcoatTexture;
    GLint clearcoatRoughnessTexture;
    GLint clearcoatNormalTexture;
    GLint clearcoatTexcoordIndex;
    GLint clearcoatRoughnessTexcoordIndex;
    GLint clearcoatNormalTexcoordIndex;

    GLuint thicknessTexture;
    GLint thicknessTexcoordIndex;

    GLuint diffuseTexture;
    GLint diffuseTexcoordIndex;

    GLuint specularGlossinessTexture;
    GLint specularGlossinessTexcoordIndex;

} Primitive;

typedef struct {
    GLuint texture;
} Texture;

typedef struct {
    GLint camera;
    GLint viewProjectionMatrixUniform;
    GLint modelMatrixUniform;
    GLint viewMatrixUniform;
    GLint projectionMatrixUniform;

    GLint envIntensity;
    GLint envDiffuseSampler;
    GLint envSpecularSampler;
    GLint envSheenSampler;
    GLint envGgxLutSampler;
    GLint envCharlieLutSampler;
    GLint envMipCount;

    GLint exposure;
    GLint roughnessFactor;

    GLint baseColorFactor;
    GLint baseColorSampler;
    GLint baseColorUVSet;
    GLint baseColorUVTransform;

    GLint emissiveFactor;
    GLint emissiveSampler;
    GLint emissiveUVSet;
    GLint emissiveUVTransform;
    GLint emissiveStrength;

    GLint metallicFactor;
    GLint metallicRoughnessSampler;
    GLint metallicRoughnessUVSet;
    GLint metallicRoughnessUVTransform;

    GLint occlusionStrength;
    GLint occlusionSampler;
    GLint occlusionUVSet;
    GLint occlusionUVTransform;

    GLint normalScale;
    GLint normalSampler;
    GLint normalUVSet;
    GLint normalUVTransform;

    GLint clearcoatFactor;
    GLint clearcoatRoughnessFactor;
    GLint clearcoatSampler;
    GLint clearcoatUVSet;
    GLint clearcoatUVTransform;
    GLint clearcoatRoughnessSampler;
    GLint clearcoatRoughnessUVSet;
    GLint clearcoatRoughnessUVTransform;
    GLint clearcoatNormalScale;
    GLint clearcoatNormalSampler;
    GLint clearcoatNormalUVSet;
    GLint clearcoatNormalUVTransform;

    GLint thickness;
    GLint thicknessSampler;
    GLint thicknessUVSet;
    GLint thicknessUVTransform;

    GLint diffuseTransmissionSampler;
    GLint diffuseTransmissionUVSet;
    GLint diffuseTransmissionUVTransform;

    GLint diffuseTransmissionColorSampler;
    GLint diffuseTransmissionColorUVSet;
    GLint diffuseTransmissionColorUVTransform;

    GLint sheenColorFactor;
    GLint sheenRoughnessFactor;

    GLint specularColorFactor;
    GLint specularFactor;

    GLint diffuseTransmissionColorFactor;
    GLint diffuseTransmissionFactor;

    GLint ior;
    GLint alphaCutoff;

    GLint dispersion;
    GLint screenSize;
    GLint transmissionFactor;
    //GLint transmissionScale;
    GLint transmissionSampler;
    GLint transmissionUVSet;
    GLint transmissionUVTransform;
    GLint transmissionFramebufferSampler;
    GLint transmissionFramebufferSize;

    GLint attenuationDistance;
    GLint attenuationColor;

    GLint jointsSampler;

    GLint diffuseFactor;
    //GLint specularFactor;
    GLint glossinessFactor;

    GLint diffuseSampler;
    GLint diffuseUVSet;
    GLint diffuseUVTransform;
    GLint specularGlossinessSampler;
    GLint specularGlossinessUVSet;
    GLint specularGlossinessUVTransform;

} UniformLocs;

typedef struct {
    bool ready;
    uint32_t program;
    uint32_t bg_program;
    uint32_t vert;
    uint32_t frag;
} gl_renwin_shaderset_t;

#endif /* LV_GLTFDATA_PRIVATE_H */
