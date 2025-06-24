#ifndef GL_SHADERDEF_H
#define GL_SHADERDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "shader_includes.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

static const char *src_vertexShader = R"(
    uniform mat4 u_ViewProjectionMatrix;
    uniform mat4 u_ModelMatrix;
    uniform mat4 u_NormalMatrix;


    in vec3 a_position;
    out vec3 v_Position;

#include <animation.glsl>
    #ifdef HAS_NORMAL_VEC3
    in vec3 a_normal;
    #endif

    #ifdef HAS_NORMAL_VEC3
    #ifdef HAS_TANGENT_VEC4
    in vec4 a_tangent;
    out mat3 v_TBN;
    #else
    out vec3 v_Normal;
    #endif
    #endif

    #ifdef HAS_TEXCOORD_0_VEC2
    in vec2 a_texcoord_0;
    #endif

    #ifdef HAS_TEXCOORD_1_VEC2
    in vec2 a_texcoord_1;
    #endif

    out vec2 v_texcoord_0;
    out vec2 v_texcoord_1;

    #ifdef HAS_COLOR_0_VEC3
    in vec3 a_color_0;
    out vec3 v_Color;
    #endif

    #ifdef HAS_COLOR_0_VEC4
    in vec4 a_color_0;
    out vec4 v_Color;
    #endif

    #ifdef USE_INSTANCING
    in mat4 a_instance_model_matrix;
    #endif

    #ifdef HAS_VERT_NORMAL_UV_TRANSFORM
    uniform mat3 u_vertNormalUVTransform;
    #endif

    vec4 getPosition()
    {
        vec4 pos = vec4(a_position, 1.0);

    #ifdef USE_MORPHING
        pos += getTargetPosition(gl_VertexID);
    #endif

    #ifdef USE_SKINNING
        pos = getSkinningMatrix() * pos;
    #endif

        return pos;
    }


    #ifdef HAS_NORMAL_VEC3
    vec3 getNormal()
    {
        vec3 normal = a_normal;

    #ifdef USE_MORPHING
        normal += getTargetNormal(gl_VertexID);
    #endif

    #ifdef USE_SKINNING
        normal = mat3(getSkinningNormalMatrix()) * normal;
    #endif

        return normalize(normal);
    }
    #endif

    #ifdef HAS_NORMAL_VEC3
    #ifdef HAS_TANGENT_VEC4
    vec3 getTangent()
    {
        vec3 tangent = a_tangent.xyz;

    #ifdef USE_MORPHING
        tangent += getTargetTangent(gl_VertexID);
    #endif

    #ifdef USE_SKINNING
        tangent = mat3(getSkinningMatrix()) * tangent;
    #endif

        return normalize(tangent);
    }
    #endif
    #endif

    mat4 temp_makeNormalMatrixFromViewProj(mat4 _viewProjModelMatrix) {
        mat4 normMat = _viewProjModelMatrix ;
        normMat[0][0] = 1.0;
        normMat[0][1] = 0.0;
        normMat[0][2] = 0.0;
        normMat[0][3] = 0.0;
        normMat[1][0] = 0.0;
        normMat[1][3] = 0.0;
        normMat[2][0] = 0.0;
        normMat[2][3] = 0.0;
        normMat[3][0] = 0.0;
        normMat[3][1] = 0.0;
        normMat[3][2] = 0.0;
        normMat[3][3] = 1.0;
        return normMat;
    }

    void main()
    {
        gl_PointSize = 1.0f;
    #ifdef USE_INSTANCING
        mat4 modelMatrix = a_instance_model_matrix;
        mat4 normalMatrix = transpose(inverse(modelMatrix));
    #else
        mat4 modelMatrix = u_ModelMatrix;
        //mat4 normalMatrix =  u_NormalMatrix;
        mat4 normalMatrix =  transpose(inverse(modelMatrix));

    #endif
        vec4 pos = modelMatrix * getPosition();
        v_Position = vec3(pos.xyz) / pos.w;

    #ifdef HAS_NORMAL_VEC3
    #ifdef HAS_TANGENT_VEC4
        vec3 tangent = getTangent();
        vec3 normalW = normalize(vec3(normalMatrix * vec4(getNormal(), 0.0)));
        vec3 tangentW = vec3(modelMatrix * vec4(tangent, 0.0));
        vec3 bitangentW = cross(normalW, tangentW) * a_tangent.w;

    #ifdef HAS_VERT_NORMAL_UV_TRANSFORM
        tangentW = u_vertNormalUVTransform * tangentW;
        bitangentW = u_vertNormalUVTransform * bitangentW;
    #endif

        bitangentW = normalize(bitangentW);
        tangentW = normalize(tangentW);

        v_TBN = mat3(tangentW, bitangentW, normalW);
    #else
        v_Normal = normalize(vec3(normalMatrix * vec4(getNormal(), 0.0)));
    #endif
    #endif

        v_texcoord_0 = vec2(0.0, 0.0);
        v_texcoord_1 = vec2(0.0, 0.0);

    #ifdef HAS_TEXCOORD_0_VEC2
        v_texcoord_0 = a_texcoord_0;
    #endif

    #ifdef HAS_TEXCOORD_1_VEC2
        v_texcoord_1 = a_texcoord_1;
    #endif

    #ifdef USE_MORPHING
        v_texcoord_0 += getTargetTexCoord0(gl_VertexID);
        v_texcoord_1 += getTargetTexCoord1(gl_VertexID);
    #endif


    #if defined(HAS_COLOR_0_VEC3) 
        v_Color = a_color_0;
    #if defined(USE_MORPHING)
        v_Color = clamp(v_Color + getTargetColor0(gl_VertexID).xyz, 0.0f, 1.0f);
    #endif
    #endif

    #if defined(HAS_COLOR_0_VEC4) 
        v_Color = a_color_0;
    #if defined(USE_MORPHING)
        v_Color = clamp(v_Color + getTargetColor0(gl_VertexID), 0.0f, 1.0f);
    #endif
    #endif

        gl_Position = u_ViewProjectionMatrix * pos;
    }
)";

static const char *src_fragmentShader = R"(

    //#define LIGHT_COUNT 0

    //
    // This fragment shader defines a reference implementation for Physically Based Shading of
    // a microfacet surface material defined by a glTF model.
    //
    // References:
    // [1] Real Shading in Unreal Engine 4
    //     http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
    // [2] Physically Based Shading at Disney
    //     http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
    // [3] README.md - Environment Maps
    //     https://github.com/KhronosGroup/glTF-WebGL-PBR/#environment-maps
    // [4] "An Inexpensive BRDF Model for Physically based Rendering" by Christophe Schlick
    //     https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf
    // [5] "KHR_materials_clearcoat"
    //     https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_clearcoat

    precision highp float;

#include <tonemapping.glsl>
#include <textures.glsl>
#include <functions.glsl>
#include <brdf.glsl>
#include <punctual.glsl>
#include <ibl.glsl>
#include <material_info.glsl>

    #ifdef MATERIAL_IRIDESCENCE
#include <iridescence.glsl>
    #endif


    out vec4 g_finalColor;


    void main()
    {
        
    vec4 baseColor = getBaseColor();

    #if ALPHAMODE == _OPAQUE
        baseColor.a = 1.0;
    #endif

        vec4 temp_origBaseColor = baseColor;

        vec3 color = vec3(0);

        vec3 v = normalize(u_Camera - v_Position);

        NormalInfo normalInfo = getNormalInfo(v);
        vec3 n = normalInfo.n;
        vec3 t = normalInfo.t;
        vec3 b = normalInfo.b;

        float NdotV = clampedDot(n, v);
        float TdotV = clampedDot(t, v);
        float BdotV = clampedDot(b, v);

        MaterialInfo materialInfo;
        materialInfo.baseColor = baseColor.rgb;
        
        // The default index of refraction of 1.5 yields a dielectric normal incidence reflectance of 0.04.
        materialInfo.ior = 1.5;
        materialInfo.f0_dielectric = vec3(0.04);
        materialInfo.specularWeight = 1.0;

        // Anything less than 2% is physically impossible and is instead considered to be shadowing. Compare to "Real-Time-Rendering" 4th editon on page 325.
        materialInfo.f90 = vec3(1.0);
        materialInfo.f90_dielectric = materialInfo.f90;

    #ifdef MATERIAL_IOR
        materialInfo = getIorInfo(materialInfo);
    #endif

    #ifdef MATERIAL_METALLICROUGHNESS
        materialInfo = getMetallicRoughnessInfo(materialInfo);
    #endif

    #ifdef MATERIAL_SHEEN
        materialInfo = getSheenInfo(materialInfo);
    #endif

    #ifdef MATERIAL_CLEARCOAT
        materialInfo = getClearCoatInfo(materialInfo, normalInfo);
    #endif

    #ifdef MATERIAL_SPECULAR
        materialInfo = getSpecularInfo(materialInfo);
    #endif

    #ifdef MATERIAL_TRANSMISSION
        materialInfo = getTransmissionInfo(materialInfo);
    #endif

    #ifdef MATERIAL_VOLUME
        materialInfo = getVolumeInfo(materialInfo);
    #endif

    #ifdef MATERIAL_IRIDESCENCE
        materialInfo = getIridescenceInfo(materialInfo);
    #endif

    #ifdef MATERIAL_DIFFUSE_TRANSMISSION
        materialInfo = getDiffuseTransmissionInfo(materialInfo);
    #endif

    #ifdef MATERIAL_ANISOTROPY
        materialInfo = getAnisotropyInfo(materialInfo, normalInfo);
    #endif

        materialInfo.perceptualRoughness = clamp(materialInfo.perceptualRoughness, 0.0, 1.0);
        materialInfo.metallic = clamp(materialInfo.metallic, 0.0, 1.0);

        // Roughness is authored as perceptual roughness; as is convention,
        // convert to material roughness by squaring the perceptual roughness.
        materialInfo.alphaRoughness = materialInfo.perceptualRoughness * materialInfo.perceptualRoughness;


        // LIGHTING
        vec3 f_specular_dielectric = vec3(0.0);
        vec3 f_specular_metal = vec3(0.0);
        vec3 f_diffuse = vec3(0.0);
        vec3 f_dielectric_brdf_ibl = vec3(0.0);
        vec3 f_metal_brdf_ibl = vec3(0.0);
        vec3 f_emissive = vec3(0.0);
        vec3 clearcoat_brdf = vec3(0.0);
        vec3 f_sheen = vec3(0.0);
        vec3 f_specular_transmission = vec3(0.0);
        vec3 f_diffuse_transmission = vec3(0.0);

        float clearcoatFactor = 0.0;
        vec3 clearcoatFresnel = vec3(0);

        float albedoSheenScaling = 1.0;
        float diffuseTransmissionThickness = 1.0;

    #ifdef MATERIAL_IRIDESCENCE
        vec3 iridescenceFresnel_dielectric = evalIridescence(1.0, materialInfo.iridescenceIor, NdotV, materialInfo.iridescenceThickness, materialInfo.f0_dielectric);
        vec3 iridescenceFresnel_metallic = evalIridescence(1.0, materialInfo.iridescenceIor, NdotV, materialInfo.iridescenceThickness, baseColor.rgb);

        if (materialInfo.iridescenceThickness == 0.0) {
            materialInfo.iridescenceFactor = 0.0;
        }
    #endif

    #ifdef MATERIAL_DIFFUSE_TRANSMISSION
    #ifdef MATERIAL_VOLUME
        diffuseTransmissionThickness = materialInfo.thickness *
            (length(vec3(u_ModelMatrix[0].xyz)) + length(vec3(u_ModelMatrix[1].xyz)) + length(vec3(u_ModelMatrix[2].xyz))) / 3.0;
    #endif
    #endif

    #ifdef MATERIAL_CLEARCOAT
        clearcoatFactor = materialInfo.clearcoatFactor;
        clearcoatFresnel = F_Schlick(materialInfo.clearcoatF0, materialInfo.clearcoatF90, clampedDot(materialInfo.clearcoatNormal, v));
    #endif

        // Calculate lighting contribution from image based lighting source (IBL)

    #if defined(USE_IBL) || defined(MATERIAL_TRANSMISSION)

        f_diffuse = getDiffuseLight(n) * baseColor.rgb ;

    #ifdef MATERIAL_DIFFUSE_TRANSMISSION
        vec3 diffuseTransmissionIBL = getDiffuseLight(-n) * materialInfo.diffuseTransmissionColorFactor;
    #ifdef MATERIAL_VOLUME
            diffuseTransmissionIBL = applyVolumeAttenuation(diffuseTransmissionIBL, diffuseTransmissionThickness, materialInfo.attenuationColor, materialInfo.attenuationDistance);
    #endif
        f_diffuse = mix(f_diffuse, diffuseTransmissionIBL, materialInfo.diffuseTransmissionFactor);
    #endif


    #if defined(MATERIAL_TRANSMISSION)
        f_specular_transmission = getIBLVolumeRefraction(
            n, v,
            materialInfo.perceptualRoughness,
            baseColor.rgb, v_Position, u_ModelMatrix, u_ViewMatrix, u_ProjectionMatrix,
            materialInfo.ior, materialInfo.thickness, materialInfo.attenuationColor, materialInfo.attenuationDistance, materialInfo.dispersion);
        f_diffuse = mix(f_diffuse, f_specular_transmission, materialInfo.transmissionFactor);
    #endif

    #ifdef MATERIAL_ANISOTROPY
        f_specular_metal = getIBLRadianceAnisotropy(n, v, materialInfo.perceptualRoughness, materialInfo.anisotropyStrength, materialInfo.anisotropicB);
        f_specular_dielectric = f_specular_metal;
    #else
        f_specular_metal = getIBLRadianceGGX(n, v, materialInfo.perceptualRoughness);
        f_specular_dielectric = f_specular_metal;
    #endif

        // Calculate fresnel mix for IBL  

        vec3 f_metal_fresnel_ibl = getIBLGGXFresnel(n, v, materialInfo.perceptualRoughness, baseColor.rgb, 1.0);
        f_metal_brdf_ibl = f_metal_fresnel_ibl * f_specular_metal;
    
        vec3 f_dielectric_fresnel_ibl = getIBLGGXFresnel(n, v, materialInfo.perceptualRoughness, materialInfo.f0_dielectric, materialInfo.specularWeight);
        f_dielectric_brdf_ibl = mix(f_diffuse, f_specular_dielectric,  f_dielectric_fresnel_ibl);

    #ifdef MATERIAL_IRIDESCENCE
        f_metal_brdf_ibl = mix(f_metal_brdf_ibl, f_specular_metal * iridescenceFresnel_metallic, materialInfo.iridescenceFactor);
        f_dielectric_brdf_ibl = mix(f_dielectric_brdf_ibl, rgb_mix(f_diffuse, f_specular_dielectric, iridescenceFresnel_dielectric), materialInfo.iridescenceFactor);
    #endif

    #ifdef MATERIAL_CLEARCOAT
        clearcoat_brdf = getIBLRadianceGGX(materialInfo.clearcoatNormal, v, materialInfo.clearcoatRoughness);
    #endif

    #ifdef MATERIAL_SHEEN
        f_sheen = getIBLRadianceCharlie(n, v, materialInfo.sheenRoughnessFactor, materialInfo.sheenColorFactor);
        albedoSheenScaling = 1.0 - max3(materialInfo.sheenColorFactor) * albedoSheenScalingLUT(NdotV, materialInfo.sheenRoughnessFactor);
    #endif

        color = mix(f_dielectric_brdf_ibl, f_metal_brdf_ibl, materialInfo.metallic);
        color = f_sheen + color * albedoSheenScaling;
        color = mix(color, clearcoat_brdf, clearcoatFactor * clearcoatFresnel);

    #ifdef HAS_OCCLUSION_MAP
        float ao = 1.0;
        ao = texture(u_OcclusionSampler,  getOcclusionUV()).r;
        color = color * (1.0 + u_OcclusionStrength * (ao - 1.0)); 
        //temp_origBaseColor.rgb *= (1.0 + u_OcclusionStrength * (ao - 1.0));

        //color = vec4(1.0, 0.0, 0.5, 1.0);
    #endif

    //#else  // Temporary addition to enable occlusion maps in non-IBL mode, which isn't physically accurate and should eventually be removed.
    //#ifdef HAS_OCCLUSION_MAP
    //    float ao = 1.0;
    //    ao = texture(u_OcclusionSampler,  getOcclusionUV()).r;
    //    //color = vec3(1.0, 0.0, 0.5);
    //    color = color * (1.0 + u_OcclusionStrength * (ao - 1.0)); 
    //#endif
    #endif //end USE_IBL


        f_diffuse = vec3(0.0);



        f_specular_dielectric = vec3(0.0);
        f_specular_metal = vec3(0.0);
        vec3 f_dielectric_brdf = vec3(0.0);
        vec3 f_metal_brdf = vec3(0.0);

    #ifdef USE_PUNCTUAL
        /*
        Light temp_keylight = Light( 
            normalize(vec3(-0.1, -0.75, -0.45)),    //vec3 direction
            -1.0,                  //float range
            vec3(1.0, 1.0, 1.0),    //vec3 color
            1.0,                    //float intensity
            vec3(0.0, 0.0, 0.0),    //vec3 position
            0.0,                    //float innerConeCos
            0.0,                    //float outerConeCos
            0                       //int type;
                                    //  const Type_Directional = 0;
                                    //  const Type_Point = 1;
                                    //  const Type_Spot = 2;
            );
        Light temp_filllight = Light( 
            normalize(-vec3(-0.2, -0.65, -0.35)),    //vec3 direction
            -1.0,                  //float range
            vec3(1.0, 1.0, 1.0),    //vec3 color
            0.5,                    //float intensity
            vec3(0.0, 0.0, 0.0),    //vec3 position
            0.0,                    //float innerConeCos
            0.0,                    //float outerConeCos
            0                       //int type;
                                    //  const Type_Directional = 0;
                                    //  const Type_Point = 1;
                                    //  const Type_Spot = 2;
            );

        u_Lights[1] = temp_keylight;
        u_Lights[2] = temp_filllight;
        */
        for (int i = 0; i < LIGHT_COUNT; ++i)
        {
            Light light = u_Lights[i+1];

            vec3 pointToLight;
            if (light.type != LightType_Directional)
            {
                pointToLight = light.position - v_Position;
            }
            else
            {
                pointToLight = -light.direction;
            }

            // BSTF

            vec3 l = normalize(pointToLight);   // Direction from surface point to light
            vec3 h = normalize(l + v);          // Direction of the vector between l and v, called halfway vector
            float NdotL = clampedDot(n, l);
            float NdotV = clampedDot(n, v);
            float NdotH = clampedDot(n, h);
            float LdotH = clampedDot(l, h);
            float VdotH = clampedDot(v, h);

            vec3 dielectric_fresnel = F_Schlick(materialInfo.f0_dielectric * materialInfo.specularWeight, materialInfo.f90_dielectric, abs(VdotH));
            vec3 metal_fresnel = F_Schlick(baseColor.rgb, vec3(1.0), abs(VdotH));
            
            vec3 lightIntensity = getLighIntensity(light, pointToLight);
            
            vec3 l_diffuse = lightIntensity * NdotL * BRDF_lambertian(baseColor.rgb);
            vec3 l_specular_dielectric = vec3(0.0);
            vec3 l_specular_metal = vec3(0.0);
            vec3 l_dielectric_brdf = vec3(0.0);
            vec3 l_metal_brdf = vec3(0.0);
            vec3 l_clearcoat_brdf = vec3(0.0);
            vec3 l_sheen = vec3(0.0);
            float l_albedoSheenScaling = 1.0;
          
    #ifdef MATERIAL_DIFFUSE_TRANSMISSION
            l_diffuse = l_diffuse * (1.0 - materialInfo.diffuseTransmissionFactor);
            if (dot(n, l) < 0.0) {
                float diffuseNdotL = clampedDot(-n, l);
                vec3 diffuse_btdf = lightIntensity * diffuseNdotL * BRDF_lambertian(materialInfo.diffuseTransmissionColorFactor);

                vec3 l_mirror = normalize(l + 2.0 * n * dot(-l, n)); // Mirror light reflection vector on surface
                float diffuseVdotH = clampedDot(v, normalize(l_mirror + v));
                dielectric_fresnel = F_Schlick(materialInfo.f0_dielectric * materialInfo.specularWeight, materialInfo.f90_dielectric, abs(diffuseVdotH));

    #ifdef MATERIAL_VOLUME
                diffuse_btdf = applyVolumeAttenuation(diffuse_btdf, diffuseTransmissionThickness, materialInfo.attenuationColor, materialInfo.attenuationDistance);
    #endif
                l_diffuse += diffuse_btdf * materialInfo.diffuseTransmissionFactor;
            }
    #endif // MATERIAL_DIFFUSE_TRANSMISSION

    //temp_origBaseColor.rgb = vec3(l_diffuse);

            // BTDF (Bidirectional Transmittance Distribution Function)
    #ifdef MATERIAL_TRANSMISSION
            // If the light ray travels through the geometry, use the point it exits the geometry again.
            // That will change the angle to the light source, if the material refracts the light ray.
            vec3 transmissionRay = getVolumeTransmissionRay(n, v, materialInfo.thickness, materialInfo.ior, u_ModelMatrix);
            pointToLight -= transmissionRay;
            l = normalize(pointToLight);

            vec3 transmittedLight = lightIntensity * getPunctualRadianceTransmission(n, v, l, materialInfo.alphaRoughness, baseColor.rgb, materialInfo.ior);

    #ifdef MATERIAL_VOLUME
            transmittedLight = applyVolumeAttenuation(transmittedLight, length(transmissionRay), materialInfo.attenuationColor, materialInfo.attenuationDistance);
    #endif
            l_diffuse = mix(l_diffuse, transmittedLight, materialInfo.transmissionFactor);
    #endif
            // Calculation of analytical light
            // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
            vec3 intensity = getLighIntensity(light, pointToLight);

    #ifdef MATERIAL_ANISOTROPY
            l_specular_metal = intensity * NdotL * BRDF_specularGGXAnisotropy(materialInfo.alphaRoughness, materialInfo.anisotropyStrength, n, v, l, h, materialInfo.anisotropicT, materialInfo.anisotropicB);
            l_specular_dielectric = l_specular_metal;
    #else
            l_specular_metal = intensity * NdotL * BRDF_specularGGX(materialInfo.alphaRoughness, NdotL, NdotV, NdotH);
            l_specular_dielectric = l_specular_metal;
    #endif

            l_metal_brdf = metal_fresnel * l_specular_metal;
            l_dielectric_brdf = mix(l_diffuse, l_specular_dielectric, dielectric_fresnel); // Do we need to handle vec3 fresnel here?


    #ifdef MATERIAL_IRIDESCENCE
            l_metal_brdf = mix(l_metal_brdf, l_specular_metal * iridescenceFresnel_metallic, materialInfo.iridescenceFactor);
            l_dielectric_brdf = mix(l_dielectric_brdf, rgb_mix(l_diffuse, l_specular_dielectric, iridescenceFresnel_dielectric), materialInfo.iridescenceFactor);
    #endif

    #ifdef MATERIAL_CLEARCOAT
            l_clearcoat_brdf = intensity * getPunctualRadianceClearCoat(materialInfo.clearcoatNormal, v, l, h, VdotH,
                materialInfo.clearcoatF0, materialInfo.clearcoatF90, materialInfo.clearcoatRoughness);
    #endif

    #ifdef MATERIAL_SHEEN
            l_sheen = intensity * getPunctualRadianceSheen(materialInfo.sheenColorFactor, materialInfo.sheenRoughnessFactor, NdotL, NdotV, NdotH);
            l_albedoSheenScaling = min(1.0 - max3(materialInfo.sheenColorFactor) * albedoSheenScalingLUT(NdotV, materialInfo.sheenRoughnessFactor),
                1.0 - max3(materialInfo.sheenColorFactor) * albedoSheenScalingLUT(NdotL, materialInfo.sheenRoughnessFactor));
    #endif

            //temp_origBaseColor.rgb = (l_metal_brdf + l_dielectric_brdf).xyz;
            
            vec3 l_color = mix(l_dielectric_brdf, l_metal_brdf, materialInfo.metallic);
            l_color = l_sheen + l_color * l_albedoSheenScaling;
            l_color = mix(l_color, l_clearcoat_brdf, clearcoatFactor * clearcoatFresnel);
            color += l_color;
        }
    #endif // USE_PUNCTUAL

        f_emissive = u_EmissiveFactor;
    #ifdef MATERIAL_EMISSIVE_STRENGTH
        f_emissive *= u_EmissiveStrength;
    #endif
    #ifdef HAS_EMISSIVE_MAP
        f_emissive *= texture(u_EmissiveSampler, getEmissiveUV()).rgb;
    #endif


    #ifdef MATERIAL_UNLIT
        //#ifdef HAS_EMISSIVE_MAP
        //    color = texture(u_EmissiveSampler, getEmissiveUV()).rgb;
        //#else
            color = baseColor.rgb;
        //#endif
    #elif defined(NOT_TRIANGLE) && !defined(HAS_NORMAL_VEC3)
        //Points or Lines with no NORMAL attribute SHOULD be rendered without lighting and instead use the sum of the base color value and the emissive value.
        color = f_emissive + baseColor.rgb;
    #else
        color = f_emissive * (1.0 - clearcoatFactor * clearcoatFresnel) + color;
    #endif

    //#if DEBUG == DEBUG_NONE

    #if ALPHAMODE == _MASK
        // Late discard to avoid sampling artifacts. See https://github.com/KhronosGroup/glTF-Sample-Viewer/issues/267
        if (baseColor.a < u_AlphaCutoff)
        {
            discard;
        }
        baseColor.a = 1.0;
    #endif

    #ifdef LINEAR_OUTPUT
        g_finalColor = vec4(color.rgb, baseColor.a);
    #else
        g_finalColor = vec4(toneMap(color), baseColor.a);
    #endif


        /*
    #else
        // In case of missing data for a debug view, render a checkerboard.
        g_finalColor = vec4(1.0);
        {
            float frequency = 0.02;
            float gray = 0.9;

            vec2 v1 = step(0.5, fract(frequency * gl_FragCoord.xy));
            vec2 v2 = step(0.5, vec2(1.0) - fract(frequency * gl_FragCoord.xy));
            g_finalColor.rgb *= gray + v1.x * v1.y + v2.x * v2.y;
        }
    #endif
    

    
        // Debug views:

        // Generic:

    #if DEBUG == DEBUG_UV_0 && defined(HAS_TEXCOORD_0_VEC2)
        g_finalColor.rgb = vec3(v_texcoord_0, 0);
    #endif
    #if DEBUG == DEBUG_UV_1 && defined(HAS_TEXCOORD_1_VEC2)
        g_finalColor.rgb = vec3(v_texcoord_1, 0);
    #endif
    #if DEBUG == DEBUG_NORMAL_TEXTURE && defined(HAS_NORMAL_MAP)
        g_finalColor.rgb = (normalInfo.ntex + 1.0) / 2.0;
    #endif
    #if DEBUG == DEBUG_NORMAL_SHADING
        g_finalColor.rgb = (n + 1.0) / 2.0;
    #endif
    #if DEBUG == DEBUG_NORMAL_GEOMETRY
        g_finalColor.rgb = (normalInfo.ng + 1.0) / 2.0;
    #endif
    #if DEBUG == DEBUG_TANGENT
        g_finalColor.rgb = (normalInfo.t + 1.0) / 2.0;
    #endif
    #if DEBUG == DEBUG_BITANGENT
        g_finalColor.rgb = (normalInfo.b + 1.0) / 2.0;
    #endif
    #if DEBUG == DEBUG_ALPHA
        g_finalColor.rgb = vec3(baseColor.a);
    #endif
    #if DEBUG == DEBUG_OCCLUSION && defined(HAS_OCCLUSION_MAP)
        g_finalColor.rgb = vec3(ao);
    #endif
    #if DEBUG == DEBUG_EMISSIVE
        g_finalColor.rgb = linearTosRGB(f_emissive);
    #endif


    #if DEBUG == DEBUG_METALLIC
        g_finalColor.rgb = vec3(materialInfo.metallic);
    #endif
    #if DEBUG == DEBUG_ROUGHNESS
        g_finalColor.rgb = vec3(materialInfo.perceptualRoughness);
    #endif
    #if DEBUG == DEBUG_BASE_COLOR
        g_finalColor.rgb = linearTosRGB(materialInfo.baseColor);
    #endif

        // Clearcoat:
    #ifdef MATERIAL_CLEARCOAT
    #if DEBUG == DEBUG_CLEARCOAT_FACTOR
        g_finalColor.rgb = vec3(materialInfo.clearcoatFactor);
    #endif
    #if DEBUG == DEBUG_CLEARCOAT_ROUGHNESS
        g_finalColor.rgb = vec3(materialInfo.clearcoatRoughness);
    #endif
    #if DEBUG == DEBUG_CLEARCOAT_NORMAL
        g_finalColor.rgb = (materialInfo.clearcoatNormal + vec3(1)) / 2.0;
    #endif
    #endif

        // Sheen:
    #ifdef MATERIAL_SHEEN
    #if DEBUG == DEBUG_SHEEN_COLOR
        g_finalColor.rgb = materialInfo.sheenColorFactor;
    #endif
    #if DEBUG == DEBUG_SHEEN_ROUGHNESS
        g_finalColor.rgb = vec3(materialInfo.sheenRoughnessFactor);
    #endif
    #endif

        // Specular:
    #ifdef MATERIAL_SPECULAR
    #if DEBUG == DEBUG_SPECULAR_FACTOR
        g_finalColor.rgb = vec3(materialInfo.specularWeight);
    #endif

    #if DEBUG == DEBUG_SPECULAR_COLOR
    vec3 specularTexture = vec3(1.0);
    #ifdef HAS_SPECULAR_COLOR_MAP
        specularTexture.rgb = texture(u_SpecularColorSampler, getSpecularColorUV()).rgb;
    #endif
        g_finalColor.rgb = u_KHR_materials_specular_specularColorFactor * specularTexture.rgb;
    #endif
    #endif

        // Transmission, Volume:
    #ifdef MATERIAL_TRANSMISSION
    #if DEBUG == DEBUG_TRANSMISSION_FACTOR
        g_finalColor.rgb = vec3(materialInfo.transmissionFactor);
    #endif
    #endif
    #ifdef MATERIAL_VOLUME
    #if DEBUG == DEBUG_VOLUME_THICKNESS
        g_finalColor.rgb = vec3(materialInfo.thickness / u_ThicknessFactor);
    #endif
    #endif

        // Iridescence:
    #ifdef MATERIAL_IRIDESCENCE
    #if DEBUG == DEBUG_IRIDESCENCE_FACTOR
        g_finalColor.rgb = vec3(materialInfo.iridescenceFactor);
    #endif
    #if DEBUG == DEBUG_IRIDESCENCE_THICKNESS
        g_finalColor.rgb = vec3(materialInfo.iridescenceThickness / 1200.0);
    #endif
    #endif

        // Anisotropy:
    #ifdef MATERIAL_ANISOTROPY
    #if DEBUG == DEBUG_ANISOTROPIC_STRENGTH
        g_finalColor.rgb = vec3(materialInfo.anisotropyStrength);
    #endif
    #if DEBUG == DEBUG_ANISOTROPIC_DIRECTION
        vec2 direction = vec2(1.0, 0.0);
    #ifdef HAS_ANISOTROPY_MAP
        direction = texture(u_AnisotropySampler, getAnisotropyUV()).xy;
        direction = direction * 2.0 - vec2(1.0); // [0, 1] -> [-1, 1]
    #endif
        vec2 directionRotation = u_Anisotropy.xy; // cos(theta), sin(theta)
        mat2 rotationMatrix = mat2(directionRotation.x, directionRotation.y, -directionRotation.y, directionRotation.x);
        direction = (direction + vec2(1.0)) * 0.5; // [-1, 1] -> [0, 1]

        g_finalColor.rgb = vec3(direction, 0.0);
    #endif
    #endif

        // Diffuse Transmission:
    #ifdef MATERIAL_DIFFUSE_TRANSMISSION
    #if DEBUG == DEBUG_DIFFUSE_TRANSMISSION_FACTOR
        g_finalColor.rgb = linearTosRGB(vec3(materialInfo.diffuseTransmissionFactor));
    #endif
    #if DEBUG == DEBUG_DIFFUSE_TRANSMISSION_COLOR_FACTOR
        g_finalColor.rgb = linearTosRGB(materialInfo.diffuseTransmissionColorFactor);
    #endif
    #endif
    */
    }
)";

#include <stdio.h>
inline static char* src_vertex(void) { 
    printf("Requesting vertex shader. Override: %s\n", shader_vertex_is_overridden() ? "true" : "false"); 
    return PREPROCESS(shader_vertex_is_overridden() ? get_shader_vertex_override() : src_vertexShader); 
}

inline static char* src_frag(void) {
    printf("Requesting fragment shader. Override: %s\n", shader_fragment_is_overridden() ? "true" : "false");
    return PREPROCESS(shader_fragment_is_overridden() ? get_shader_fragment_override() : src_fragmentShader); 
}

#pragma GCC diagnostic pop

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*GL_SHADERDEF_H*/
