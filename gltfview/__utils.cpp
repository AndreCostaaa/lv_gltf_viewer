#include <string>
#include <iostream>

#include <GL/glew.h>
#include "lvgl/src/drivers/glfw/lv_opengles_debug.h" /* GL_CALL */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-move"
#include "lib/fastgltf/include/fastgltf/core.hpp"
#include "lib/fastgltf/include/fastgltf/types.hpp"
#include "lib/fastgltf/include/fastgltf/tools.hpp"
#pragma GCC diagnostic pop

#include "lv_gltfview_internal.h"

#include "lib/mathc/mathc.h"
#include "lib/mathc/mathc.c"

GLboolean blendEnabled;
GLint blendSrc;
GLint blendDst;
GLint blendEquation;

_VEC3 get_cached_centerpoint(_DATA D, _UINT I, int32_t P, _MAT4 M);
_VEC3 get_cam_pos(_VIEW V);

inline void lv_gltf_opengl_state_push(void) {
    // retain state of blend enabled
    GL_CALL(glGetBooleanv(GL_BLEND, &blendEnabled));
    // retain src / dst blend functions
    GL_CALL(glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc));
    GL_CALL(glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst));
    // retain blend equation
    GL_CALL(glGetIntegerv(GL_BLEND_EQUATION, &blendEquation));
}

inline void lv_gltf_opengl_state_pop(void) {
    GL_CALL(glDisable(GL_CULL_FACE));  
    if (blendEnabled ) { GL_CALL(glEnable(GL_BLEND)); } else { GL_CALL(glDisable(GL_BLEND)); }
    GL_CALL(glBlendFunc(blendSrc, blendDst));
    GL_CALL(glBlendEquation(blendEquation));
    GL_CALL(glDepthMask(GL_TRUE));    
}

FVEC4 lv_gltf_get_primitive_centerpoint(pGltf_data_t ret_data, fastgltf::Mesh& mesh, uint32_t prim_num){
    FVEC4 _retval = FVEC4(0.0f);
    FVEC3 _vmin = FVEC3(999999999.f);
    FVEC3 _vmax = FVEC3(-999999999.f);
    FVEC3 _vcen = FVEC3(0.f);
    float _vrad = 0.f;

    if (mesh.primitives.size() > prim_num) {
        const auto& it = mesh.primitives[prim_num];
        const auto& asset = GET_ASSET(ret_data);    
        auto* positionIt = it.findAttribute("POSITION");
        auto& positionAccessor = asset->accessors[positionIt->accessorIndex];
        if (positionAccessor.bufferViewIndex.has_value()) {
            if (positionAccessor.min.has_value() && positionAccessor.max.has_value()) {
                FVEC4 _tmin = FVEC4(
                    (float)(positionAccessor.min.value().get<double>((size_t)0)),
                    (float)(positionAccessor.min.value().get<double>((size_t)1)),
                    (float)(positionAccessor.min.value().get<double>((size_t)2)),
                    0.f);
                FVEC4 _tmax = FVEC4(
                    (float)(positionAccessor.max.value().get<double>((size_t)0)),
                    (float)(positionAccessor.max.value().get<double>((size_t)1)),
                    (float)(positionAccessor.max.value().get<double>((size_t)2)),
                    0.f);

                _vmax[0] = std::max(_tmin.x(), _tmax.x());
                _vmax[1] = std::max(_tmin.y(), _tmax.y());
                _vmax[2] = std::max(_tmin.z(), _tmax.z());
                _vmin[0] = std::min(_tmin.x(), _tmax.x());
                _vmin[1] = std::min(_tmin.y(), _tmax.y());
                _vmin[2] = std::min(_tmin.z(), _tmax.z());
                _vcen[0] = (_vmax[0] + _vmin[0]) / 2.0f;
                _vcen[1] = (_vmax[1] + _vmin[1]) / 2.0f;
                _vcen[2] = (_vmax[2] + _vmin[2]) / 2.0f;
                float size_x = _vmax[0] - _vmin[0];
                float size_y = _vmax[1] - _vmin[1];
                float size_z = _vmax[2] - _vmin[2];
                _vrad = std::sqrt((size_x * size_x) + (size_y * size_y) + (size_z * size_z)) / 2.0f;
                _retval[0] = _vcen[0];
                _retval[1] = _vcen[1];
                _retval[2] = _vcen[2];
                _retval[3] = _vrad;
            } else {
                std::cout << "*** COULD NOT GET PRIMITIVE CENTER POINT - NO MIN/MAX DEFINED ***\n";
            }
        }
    }
    return _retval;
}

void lv_gltf_print_matrix_summary(FMAT4 matrix) {
    const auto& m = matrix.data();
    std::cout << std::to_string(m[0]) << ", " << std::to_string(m[1]) << ", " << std::to_string(m[2]) << ", " << std::to_string(m[3]) << "\n"; 
    std::cout << std::to_string(m[4]) << ", " << std::to_string(m[5]) << ", " << std::to_string(m[6]) << ", " << std::to_string(m[7]) << "\n"; 
    std::cout << std::to_string(m[8]) << ", " << std::to_string(m[9]) << ", " << std::to_string(m[10]) << ", " << std::to_string(m[11]) << "\n"; 
    std::cout << std::to_string(m[12]) << ", " << std::to_string(m[13]) << ", " << std::to_string(m[14]) << ", " << std::to_string(m[15]) << "\n"; 
}

void lv_gltf_get_isolated_filename(const char* filename, char* out_buffer, uint32_t max_out_length){
    std::string _filenamestr = filename;
    int32_t beginIdx = _filenamestr.rfind('/');
    std::string isofilename = _filenamestr.substr(beginIdx + 1);
    strncpy(out_buffer, isofilename.c_str(), max_out_length);
    out_buffer[max_out_length-1] = '\0';
}

void lv_gltf_debug_print_node(ASSET& asset, fastgltf::Node node, std::size_t depth) {
    std::size_t _tabwidth = 4;
    std::size_t _insetspaces = _tabwidth * depth;
    char _tabstr[_insetspaces + 1];
    memset(_tabstr, ' ', _insetspaces);
    _tabstr[_insetspaces] = '\0';

    if (depth == 1) {
        std::cout << _tabstr << "+ '" << node.name << "' (Root Node)\n"; 
    } else {
        std::cout << _tabstr << "+ '" << node.name << "'\n"; 
    }

    if (node.meshIndex.has_value()) {
        std::cout << _tabstr << ": Mesh Index: " << node.meshIndex.value() << " '" << asset.meshes[node.meshIndex.value()].name << "'\n"; 
    } else {
        std::cout << _tabstr << ": (this node does not render)\n"; 
    }
    
    if (const auto* pTRS = std::get_if<fastgltf::TRS>(&node.transform)) {
        std::cout << _tabstr << ": Translation X/Y/Z: " << pTRS->translation.x() << " / " << pTRS->translation.y() << " / " << pTRS->translation.z() << "\n"; 
        std::cout << _tabstr << ": Rotation X/Y/Z/W: " << pTRS->rotation.x() << " / " << pTRS->rotation.y() << " / " << pTRS->rotation.z() << " / " << pTRS->rotation.w() << "\n"; 
        std::cout << _tabstr << ": Scale X/Y/Z/W: " << pTRS->scale.x() << " / " << pTRS->scale.y() << " / " << pTRS->scale.z() << "\n"; 
    } else if (const auto* pMat4 = std::get_if<FMAT4>(&node.transform)) {
        std::cout << _tabstr << ": 4x4 Matrix [ " << pMat4->col(0)[0] << ", " << pMat4->col(0)[1] << ", " << pMat4->col(0)[2] << ", " << pMat4->col(0)[3] << " ]\n"; 
        std::cout << _tabstr << ":            [ " << pMat4->col(1)[0] << ", " << pMat4->col(1)[1] << ", " << pMat4->col(1)[2] << ", " << pMat4->col(1)[3] << " ]\n"; 
        std::cout << _tabstr << ":            [ " << pMat4->col(2)[0] << ", " << pMat4->col(2)[1] << ", " << pMat4->col(2)[2] << ", " << pMat4->col(2)[3] << " ]\n"; 
        std::cout << _tabstr << ":            [ " << pMat4->col(3)[0] << ", " << pMat4->col(3)[1] << ", " << pMat4->col(3)[2] << ", " << pMat4->col(3)[3] << " ]\n"; 
    }
    if (node.children.size() > 0) {
        std::cout << _tabstr << "+ ("<< node.children.size() << ") children\n"; 
        std::size_t cnum = 1;
        for (auto& child : node.children) {
            lv_gltf_debug_print_node(asset, asset.nodes[child], depth + 1);
            cnum++;
        }
    } 
}

void lv_gltf_copy_viewer_desc(gl_viewer_desc_t* from, gl_viewer_desc_t* to) {
    to->pitch = from->pitch;
    to->yaw = from->yaw;
    to->distance = from->distance;
    to->recenter_flag = from->recenter_flag;
    to->focal_x = from->focal_x;
    to->focal_y = from->focal_y;
    to->focal_z = from->focal_z;
    to->width = from->width;
    to->height = from->height;
    to->camera = from->camera;
    to->anim = from->anim;
    to->timestep = from->timestep;
    to->error_frames = from->error_frames;
    to->aa_mode = from->aa_mode;
    to->bg_mode = from->bg_mode;
    to->blur_bg = from->blur_bg;
    to->exposure = from->exposure;
    to->env_pow = from->env_pow;
    to->spin_degree_offset = from->spin_degree_offset;
}

bool lv_gltf_compare_viewer_desc(gl_viewer_desc_t* from, gl_viewer_desc_t* to){
    if ( (to->pitch       != from->pitch) || 
       (to->yaw           != from->yaw) || 
       (to->distance      != from->distance) || 
       (to->recenter_flag != from->recenter_flag) || 
       (to->focal_x       != from->focal_x) || 
       (to->focal_y       != from->focal_y) || 
       (to->focal_z       != from->focal_z) || 
       (to->camera        != from->camera) || 
       (to->anim          != from->anim) || 
       (to->aa_mode       != from->aa_mode) || 
       (to->bg_mode       != from->bg_mode) || 
       (to->blur_bg       != from->blur_bg) || 
       (to->exposure      != from->exposure) || 
       (to->spin_degree_offset != from->spin_degree_offset) || 
       (to->env_pow       != from->env_pow) ) { return true; }    
    // These will be handled elsewhere
    //if (to->width        != from->width) { printf("width diff\n"); return true; }
    //if (to->height       != from->height) { printf("height diff\n"); return true; }
    // These are intentionally not compared
    //if (to->timestep     != from->timestep) return true;
    //if (to->error_frames != from->error_frames) return true;
    //if (to->frame_was_cached != from->frame_was_cached) return true;
    return false;
}

// Function to multiply a matrix by a vector
FVEC3 __multiplyMatrixByVector(const FMAT4 mat, const FVEC3 vec) {
    return FVEC3 (
        mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2] + mat[0][3],
        mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2] + mat[1][3],
        mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2] + mat[2][3] );
}

// Function to compute the ray from the camera through the mouse position
bool __computeRayToGround( lv_gltfview_t * viewer, float norm_mouseX, float norm_mouseY, double groundHeight, FVEC3* collisionPoint) {
    const auto& _viewmat = GET_VIEW_MAT(viewer);
    FMAT4 __projmat;
    float aspectRatio = 256.0f / 192.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float fov = 3.14159263 * 0.25f; // 45 degrees
    float f = 1.0f / std::tan(fov / 2.0f);
    float nf = 1.0f / (nearPlane - farPlane);

    __projmat[0][0] = f / aspectRatio;
    __projmat[1][1] = f;
    __projmat[2][2] = (farPlane + nearPlane) * nf;
    __projmat[2][3] = (2.0f * farPlane * nearPlane) * nf;
    __projmat[3][2] = -1.0f;
    __projmat[3][3] = 0.0f;

    mfloat_t _projmat[MAT4_SIZE];
    for (int32_t i=0; i<16; i++) {
        _projmat[i] = __projmat.data()[i];
    }
    mfloat_t _invprojmat[MAT4_SIZE];
    mat4_inverse(_invprojmat, _projmat);
    __projmat = FMAT4( 
        _invprojmat[0], _invprojmat[1], _invprojmat[2], _invprojmat[3],  
        _invprojmat[4], _invprojmat[5], _invprojmat[6], _invprojmat[7],  
        _invprojmat[8], _invprojmat[9], _invprojmat[10], _invprojmat[11],  
        _invprojmat[12], _invprojmat[13], _invprojmat[14], _invprojmat[15]);

    // Step 1: Convert mouse coordinates to normalized device coordinates (NDC)
    float _x = norm_mouseX * 2.0f - 1.0f;
    float _y = 1.0f - (norm_mouseY * 2.0f);
    float _z = -1.0f; // Clip space z
    FVEC4 clipSpacePos = FVEC4( _x, _y, _z, 1.f );
    auto rayEye = (__projmat) * clipSpacePos  ;
    rayEye[2] = -1.0f;    
    rayEye[3] = 0.0f;
    FVEC4 t_rayWorld = fastgltf::math::invert(*_viewmat) * rayEye;
    FVEC3 rayDirection = fastgltf::math::normalize(FVEC3(t_rayWorld[0], t_rayWorld[1], t_rayWorld[2]));

    FVEC3  _tpos = get_cam_pos(viewer);
    FVEC3  rayOrigin = FVEC3( _tpos[0],  _tpos[1],  _tpos[2] ); 
    float t = ((float)groundHeight - rayOrigin[1]) / rayDirection[1];

    if (t < 0.f) {
        return false; // The ray is pointing away from the ground
    }

    // Calculate the collision point
    (*collisionPoint)[0]= (rayOrigin[0] + t * rayDirection[0]);
    (*collisionPoint)[1] = groundHeight; // y is the ground height
    (*collisionPoint)[2] = (rayOrigin[2] + t * rayDirection[2]);

    return true; // Collision point found
}

bool lv_gltfview_raycast_ground_position( lv_gltfview_t * viewer, int32_t _mouseX, int32_t _mouseY, int32_t _winWidth, int32_t _winHeight, double groundHeight, float* outPos) {
    float norm_mouseX = (float)_mouseX / (float)(_winWidth);
    float norm_mouseY = (float)_mouseY / (float)(_winHeight);
    FVEC3 _rayres;
    bool validres = __computeRayToGround(viewer, norm_mouseX, norm_mouseY, groundHeight, &_rayres);
    if (validres) {
        outPos[0] = _rayres[0]; outPos[1] = _rayres[1]; outPos[2] = _rayres[2];
    }
    return validres;
}

FVEC3 lv_gltf_get_centerpoint(pGltf_data_t gltf_data, FMAT4 matrix, uint32_t meshIndex, int32_t elem) {
    if (!centerpoint_cache_contains(gltf_data, meshIndex, elem)) recache_centerpoint(gltf_data, meshIndex, elem);
    return get_cached_centerpoint(gltf_data, meshIndex, elem, matrix);
}

void lv_gltfview_utils_save_png( lv_gltfview_t * viewer, const char * filename, bool alpha_enabled, uint32_t compression_level ) {
    const auto& vstate = get_viewer_state(viewer);
    const auto& vdesc = lv_gltfview_get_desc(viewer);
    GL_CALL(glBindTexture(GL_TEXTURE_2D, vstate->render_state.texture));
    stbi_write_png_compression_level = compression_level;
    stbi_flip_vertically_on_write(true);
    uint32_t mipmapnum = lv_gltfview_check_frame_was_antialiased(viewer) ? 1 : 0;
    if (alpha_enabled) {
        char * pixels = (char *)lv_malloc(vdesc->height * vdesc->width * 4);
        glGetTexImage(GL_TEXTURE_2D, mipmapnum, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_write_png(filename, vdesc->width, vdesc->height, 4, pixels, vdesc->width * 4);
        lv_free(pixels);
    } else {
        char * pixels = (char *)lv_malloc(vdesc->height * vdesc->width * 3);
        glGetTexImage(GL_TEXTURE_2D, mipmapnum, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        stbi_write_png(filename, vdesc->width, vdesc->height, 3, pixels, vdesc->width * 3);
        lv_free(pixels);
    }
}