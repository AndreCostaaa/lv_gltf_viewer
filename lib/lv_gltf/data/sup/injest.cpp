#include "fastgltf/math.hpp"
#include "misc/lv_math.h"
#include <GL/glew.h>
#include "../../view/sup/include/shader_includes.h"
#include "../lv_gltf_data_internal.hpp"

#include <GL/gl.h>
#include <vector>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <iostream>

#include <webp/decode.h>

#include <stb_image/stb_image.h>

#define _MACRO_ADD_DEFINE_IF_EXISTS(ASSET_VAR, PRIMITIVE_VAR, ATTRIBUTE_ID, \
                                    DEFINE_TO_ADD)                          \
{                                                                   \
    auto *_it = PRIMITIVE_VAR->findAttribute(ATTRIBUTE_ID);     \
    if ((_it != PRIMITIVE_VAR->attributes.end()) &&             \
        (ASSET_VAR.accessors[_it->accessorIndex]               \
         .bufferViewIndex.has_value()))                 \
        addDefine(DEFINE_TO_ADD, NULL);                     \
}

#define _MACRO_ADD_TEXTURE_DEFINES(GLTF_MATERIAL_VAR, TEX_DEFINE_TO_ADD, \
                                   TEXUV_DEFINE_TO_ADD)                  \
if (GLTF_MATERIAL_VAR.has_value()) {                             \
    addDefine(TEX_DEFINE_TO_ADD, NULL);                      \
    if (GLTF_MATERIAL_VAR.value().transform)                 \
        addDefine(TEXUV_DEFINE_TO_ADD, NULL);            \
}

/**
 * @brief Discover defines for the given GLTF data object and node.
 *
 * @param data_obj Pointer to the GLTF data object.
 * @param node Pointer to the GLTF node.
 * @param prim Pointer to the GLTF primitive.
 */
void injest_discover_defines(lv_gltf_data_t * data, void * node, void * prim)
{
    const auto & asset = data->asset;
    auto it = (fastgltf::Primitive *)prim;
    clearDefines();
    //addDefine("LINEAR_OUTPUT", NULL );
    addDefine("_OPAQUE", "0");
    addDefine("_MASK", "1");
    addDefine("_BLEND", "2");

    assert(it->findAttribute("POSITION") !=
           it->attributes
           .end()); // A mesh primitive is required to hold the POSITION attribute.
    assert(it->indicesAccessor
           .has_value()); // We specify GenerateMeshIndices, so we should always have indices
    if(it->materialIndex.has_value() == false) {
        addDefine("ALPHAMODE", "_OPAQUE");
    }
    else {
        auto & material = asset.materials[it->materialIndex.value()];
        addDefine("TONEMAP_KHR_PBR_NEUTRAL", NULL);
        if(material.unlit) {
            addDefine("MATERIAL_UNLIT", NULL);
            addDefine("LINEAR_OUTPUT", NULL);
        }
        else {
            addDefine("MATERIAL_METALLICROUGHNESS", NULL);
            addDefine("LINEAR_OUTPUT", NULL);
        }

        if(data->node_by_light_index->size() > 0) {
            addDefine("USE_PUNCTUAL", NULL);
            addDefine("USE_IBL", NULL);
            size_t lc = data->node_by_light_index->size();
            if(lc == 1)
                addDefine("LIGHT_COUNT", "1");
            else if(lc == 2)
                addDefine("LIGHT_COUNT", "2");
            else if(lc == 3)
                addDefine("LIGHT_COUNT", "3");
            else if(lc == 4)
                addDefine("LIGHT_COUNT", "4");
            else if(lc == 5)
                addDefine("LIGHT_COUNT", "5");
            else if(lc == 6)
                addDefine("LIGHT_COUNT", "6");
            else if(lc == 7)
                addDefine("LIGHT_COUNT", "7");
            else if(lc == 8)
                addDefine("LIGHT_COUNT", "8");
            else if(lc == 9)
                addDefine("LIGHT_COUNT", "9");
            else if(lc == 10)
                addDefine("LIGHT_COUNT", "10");
            else
                std::cerr
                        << "ERROR: Too many scene lights, max is 10\n";
            std::cout << "Added " << std::to_string(lc)
                      << " lights to the shader\n";
        }
        else {
            addDefine("LIGHT_COUNT", "0");
            addDefine("USE_IBL", NULL);
        }

        // only set cutoff value for mask material
        if(material.alphaMode == fastgltf::AlphaMode::Mask) {
            addDefine("ALPHAMODE", "_MASK");
        }
        else if(material.alphaMode == fastgltf::AlphaMode::Opaque) {
            addDefine("ALPHAMODE", "_OPAQUE");
        }
        else {
            addDefine("ALPHAMODE", "_BLEND");
        }
        _MACRO_ADD_TEXTURE_DEFINES(material.pbrData.baseColorTexture,
                                   "HAS_BASE_COLOR_MAP",
                                   "HAS_BASECOLOR_UV_TRANSFORM");
        _MACRO_ADD_TEXTURE_DEFINES(
            material.pbrData.metallicRoughnessTexture,
            "HAS_METALLIC_ROUGHNESS_MAP",
            "HAS_METALLICROUGHNESS_UV_TRANSFORM");
        _MACRO_ADD_TEXTURE_DEFINES(material.occlusionTexture,
                                   "HAS_OCCLUSION_MAP",
                                   "HAS_OCCLUSION_UV_TRANSFORM");
        _MACRO_ADD_TEXTURE_DEFINES(material.normalTexture,
                                   "HAS_NORMAL_MAP",
                                   "HAS_NORMAL_UV_TRANSFORM");
        _MACRO_ADD_TEXTURE_DEFINES(material.emissiveTexture,
                                   "HAS_EMISSIVE_MAP",
                                   "HAS_EMISSIVE_UV_TRANSFORM");
        addDefine("MATERIAL_EMISSIVE_STRENGTH", NULL);
        if(material.sheen)
            addDefine("MATERIAL_SHEEN", NULL);
        if(material.specular)
            addDefine("MATERIAL_SPECULAR", NULL);
        if(material.specularGlossiness) {
            addDefine("MATERIAL_SPECULARGLOSSINESS", NULL);
            _MACRO_ADD_TEXTURE_DEFINES(
                material.specularGlossiness->diffuseTexture,
                "HAS_DIFFUSE_MAP", "HAS_DIFFUSE_UV_TRANSFORM");
            _MACRO_ADD_TEXTURE_DEFINES(
                material.specularGlossiness
                ->specularGlossinessTexture,
                "HAS_SPECULARGLOSSINESS_MAP",
                "HAS_SPECULARGLOSSINESS_UV_TRANSFORM");
        }
        if(material.transmission) {
            addDefine("MATERIAL_TRANSMISSION", NULL);
            addDefine("MATERIAL_DISPERSION", NULL);
            addDefine("MATERIAL_VOLUME", NULL);
            if(material.transmission->transmissionTexture
               .has_value())
                addDefine("HAS_TRANSMISSION_MAP", NULL);
            if(material.volume) {
                _MACRO_ADD_TEXTURE_DEFINES(
                    material.volume->thicknessTexture,
                    "HAS_THICKNESS_MAP",
                    "HAS_THICKNESS_UV_TRANSFORM");
            }
        }
        if(material.clearcoat) {
            addDefine("MATERIAL_CLEARCOAT", NULL);
            _MACRO_ADD_TEXTURE_DEFINES(
                material.clearcoat->clearcoatTexture,
                "HAS_CLEARCOAT_MAP",
                "HAS_CLEARCOAT_UV_TRANSFORM");
            _MACRO_ADD_TEXTURE_DEFINES(
                material.clearcoat->clearcoatRoughnessTexture,
                "HAS_CLEARCOAT_ROUGHNESS_MAP",
                "HAS_CLEARCOATROUGHNESS_UV_TRANSFORM");
            _MACRO_ADD_TEXTURE_DEFINES(
                material.clearcoat->clearcoatNormalTexture,
                "HAS_CLEARCOAT_NORMAL_MAP",
                "HAS_CLEARCOATNORMAL_UV_TRANSFORM");
        }
#ifdef FASTGLTF_DIFFUSE_TRANSMISSION_SUPPORT
        if(material.diffuseTransmission) {
            addDefine("MATERIAL_DIFFUSE_TRANSMISSION", NULL);
            if(material.diffuseTransmission
               ->diffuseTransmissionTexture.has_value())
                addDefine("HAS_DIFFUSE_TRANSMISSION_MAP", NULL);
            if(material.diffuseTransmission
               ->diffuseTransmissionColorTexture
               .has_value())
                addDefine("HAS_DIFFUSE_TRANSMISSION_COLOR_MAP",
                          NULL);
        }
#endif
    }
    _MACRO_ADD_DEFINE_IF_EXISTS(asset, it, "NORMAL", "HAS_NORMALfastgltf::math::fvec3");
    _MACRO_ADD_DEFINE_IF_EXISTS(asset, it, "TANGENT", "HAS_TANGENTfastgltf::math::fvec4");
    _MACRO_ADD_DEFINE_IF_EXISTS(asset, it, "TEXCOORD_0",
                                "HAS_TEXCOORD_0_VEC2");
    _MACRO_ADD_DEFINE_IF_EXISTS(asset, it, "TEXCOORD_1",
                                "HAS_TEXCOORD_1_VEC2");
    _MACRO_ADD_DEFINE_IF_EXISTS(asset, it, "JOINTS_0", "HAS_JOINTS_0fastgltf::math::fvec4");
    _MACRO_ADD_DEFINE_IF_EXISTS(asset, it, "JOINTS_1", "HAS_JOINTS_1fastgltf::math::fvec4");
    _MACRO_ADD_DEFINE_IF_EXISTS(asset, it, "WEIGHTS_0",
                                "HAS_WEIGHTS_0fastgltf::math::fvec4");
    _MACRO_ADD_DEFINE_IF_EXISTS(asset, it, "WEIGHTS_1",
                                "HAS_WEIGHTS_1fastgltf::math::fvec4");

    auto * joints0it = it->findAttribute("JOINTS_0");
    auto * weights0it = it->findAttribute("WEIGHTS_0");
    auto _node = (fastgltf::Node *)node;
    if((_node->skinIndex.has_value()) &&
       (joints0it != it->attributes.end()) &&
       (weights0it != it->attributes.end())) {
        std::cout << "*** SKINNING ENABLED FOR THIS MESH: SKIN-INDEX = "
                  << std::to_string(_node->skinIndex.value())
                  << " ***\n";
        std::cout
                << "*** SKIN HAS "
                << std::to_string(asset.skins[_node->skinIndex.value()]
                                  .joints.size())
                << " JOINTS ***\n";
        addDefine("USE_SKINNING", NULL);
    }
}

/**
 * @brief Set the initial bounds for the asset.
 *
 * @param ret_data Pointer to the GLTF data.
 * @param asset Pointer to the GLTF asset.
 * @param matrix The transformation matrix.
 * @param mesh Reference to the mesh.
 */

/**
 * @brief Grow the bounds to include the specified mesh.
 *
 * @param ret_data Pointer to the GLTF data.
 * @param asset Pointer to the GLTF asset.
 * @param matrix The transformation matrix.
 * @param mesh Reference to the mesh.
 */
void injest_grow_bounds_to_include(lv_gltf_data_t * data,
                                   const fastgltf::math::fmat4x4 & matrix,
                                   const fastgltf::Mesh & mesh)
{
    fastgltf::math::fvec3 v_min{ data->vertex_min[0], data->vertex_min[1],
                                 data->vertex_min[2] };

    fastgltf::math::fvec3 v_max{ data->vertex_max[0], data->vertex_max[1],
                                 data->vertex_max[2] };
    fastgltf::math::fvec3 v_cen{ data->vertex_cen[0], data->vertex_cen[1],
                                 data->vertex_cen[2] };

    float new_bound_radius = data->bound_radius;
    if(mesh.primitives.size() > 0) {
        set_bounds_info(data, v_min, v_max, v_cen, new_bound_radius);
        return;
    }
    size_t accessor_index =
        mesh.primitives[0].findAttribute("POSITION")->accessorIndex;
    const auto & accessor = data->asset.accessors[accessor_index];

    if(!accessor.bufferViewIndex.has_value() ||
       !(accessor.min.has_value() && accessor.max.has_value())) {
        set_bounds_info(data, v_min, v_max, v_cen, new_bound_radius);
        return;
    }

    fastgltf::math::fvec4 t_min{
        (float)(accessor.min.value().get<double>(0)),
        (float)(accessor.min.value().get<double>(1)),
        (float)(accessor.min.value().get<double>(2)), 1.f
    };
    fastgltf::math::fvec4 t_max{
        (float)(accessor.max.value().get<double>(0)),
        (float)(accessor.max.value().get<double>(1)),
        (float)(accessor.max.value().get<double>(2)), 1.f
    };

    t_min = matrix * t_min;
    t_max = matrix * t_max;
    v_max[0] = LV_MAX(LV_MAX(v_max[0], t_min.x()), t_max.x());
    v_max[1] = LV_MAX(LV_MAX(v_max[1], t_min.y()), t_max.y());
    v_max[2] = LV_MAX(LV_MAX(v_max[2], t_min.z()), t_max.z());
    v_min[0] = LV_MIN(LV_MIN(v_min[0], t_min.x()), t_max.x());
    v_min[1] = LV_MIN(LV_MIN(v_min[1], t_min.y()), t_max.y());
    v_min[2] = LV_MIN(LV_MIN(v_min[2], t_min.z()), t_max.z());
    v_cen[0] = (v_max[0] + v_min[0]) / 2.0f;
    v_cen[1] = (v_max[1] + v_min[1]) / 2.0f;
    v_cen[2] = (v_max[2] + v_min[2]) / 2.0f;

    float size_x = v_max[0] - v_min[0];
    float size_y = v_max[1] - v_min[1];
    float size_z = v_max[2] - v_min[2];
    new_bound_radius = std::sqrt((size_x * size_x) + (size_y * size_y) +
                                 (size_z * size_z)) /
                       2.0f;

    set_bounds_info(data, v_min, v_max, v_cen, new_bound_radius);
}
