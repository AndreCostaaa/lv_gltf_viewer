#include <string>
#include <iostream>

#include <GL/glew.h>
#include "lvgl/src/drivers/glfw/lv_opengles_debug.h" /* GL_CALL */

//#include <memory> // Include necessary headers for std::unique_ptr

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-move"
#include "lib/fastgltf/include/fastgltf/core.hpp"
#include "lib/fastgltf/include/fastgltf/types.hpp"
#include "lib/fastgltf/include/fastgltf/tools.hpp"
#pragma GCC diagnostic pop

#ifndef STB_HAS_BEEN_INCLUDED
#define STB_HAS_BEEN_INCLUDED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#pragma GCC diagnostic pop
#endif

#include "../lv_gltf_view_internal.h"
#include "../../data/lv_gltf_data_internal.h"
#include "../../../../../../lvgl_proto/src/others/opengl_shader_cache/lv_opengl_shader_cache.h"
#include "lib/mathc/mathc.h"

void set_matrix_view(_VIEW _viewer, FMAT4 _mat);
void set_matrix_proj(_VIEW _viewer, FMAT4 _mat);
void set_matrix_viewproj(_VIEW _viewer, FMAT4 _mat);

// Function to retrieve uniform locations from a shader program
void setup_uniform_locations(UniformLocs* uniforms, uint32_t _shader_prog_program) {
    auto _u = [&]( const char * _uniform ) -> GLint { return glGetUniformLocation(_shader_prog_program, _uniform); };
    // *** IMAGE QUALITY UNIFORMS ***********************************************************************
    uniforms->exposure =                            _u("u_Exposure");
    // *** CAMERA/VIEW/PROJECTION/MODEL MATRIX UNIFORMS *************************************************
    uniforms->camera =                              _u("u_Camera");
    uniforms->modelMatrixUniform =                  _u("u_ModelMatrix");
    uniforms->viewProjectionMatrixUniform =         _u("u_ViewProjectionMatrix");
    uniforms->viewMatrixUniform =                   _u("u_ViewMatrix");
    uniforms->projectionMatrixUniform =             _u("u_ProjectionMatrix");
    // *** IMAGE BASED LIGHTING (IBL) UNIFORMS **********************************************************
    uniforms->envIntensity =                        _u("u_EnvIntensity");
    uniforms->envDiffuseSampler =                   _u("u_LambertianEnvSampler");
    uniforms->envSpecularSampler =                  _u("u_GGXEnvSampler");
    uniforms->envSheenSampler =                     _u("u_CharlieEnvSampler");
    uniforms->envGgxLutSampler =                    _u("u_GGXLUT");
    uniforms->envCharlieLutSampler =                _u("u_CharlieLUT");
    uniforms->envMipCount =                         _u("u_MipCount");
    // *** BASE COLOR / TEXTURE UNIFORMS ****************************************************************
    uniforms->baseColorFactor =                     _u("u_BaseColorFactor");
    uniforms->baseColorSampler =                    _u("u_BaseColorSampler");
    uniforms->baseColorUVSet =                      _u("u_BaseColorUVSet");
    uniforms->baseColorUVTransform =                _u("u_BaseColorUVTransform");
    // *** CUTOFF / IOR / DISPERSION UNIFORMS ***********************************************************
    uniforms->alphaCutoff =                         _u("u_AlphaCutoff");
    uniforms->ior =                                 _u("u_Ior");
    uniforms->dispersion =                          _u("u_Dispersion");
    // *** METALLIC / ROUGHNESS UNIFORMS ****************************************************************
    uniforms->metallicFactor =                      _u("u_MetallicFactor");
    uniforms->roughnessFactor =                     _u("u_RoughnessFactor");
	uniforms->metallicRoughnessSampler =            _u("u_MetallicRoughnessSampler");
    uniforms->metallicRoughnessUVSet =              _u("u_MetallicRoughnessUVSet");
    uniforms->metallicRoughnessUVTransform =        _u("u_MetallicRoughnessUVTransform");
    // *** EMISSION UNIFORMS ****************************************************************************
    uniforms->emissiveFactor =                      _u("u_EmissiveFactor");
    uniforms->emissiveSampler =                     _u("u_EmissiveSampler");
    uniforms->emissiveUVSet =                       _u("u_EmissiveUVSet");
    uniforms->emissiveUVTransform =                 _u("u_EmissiveUVTransform");
    uniforms->emissiveStrength =                    _u("u_EmissiveStrength");
    // *** OCCLUSION UNIFORMS ***************************************************************************
    uniforms->occlusionStrength =                   _u("u_OcclusionStrength");
    uniforms->occlusionSampler =                    _u("u_OcclusionSampler");
    uniforms->occlusionUVSet =                      _u("u_OcclusionUVSet");
    uniforms->occlusionUVTransform =                _u("u_OcclusionUVTransform");
    // *** NORMAL MAP UNIFORMS **************************************************************************
    uniforms->normalSampler =                       _u("u_NormalSampler");
    uniforms->normalScale =                         _u("u_NormalScale");
    uniforms->normalUVSet =                         _u("u_NormalUVSet");
    uniforms->normalUVTransform =                   _u("u_NormalUVTransform");
    // *** VOLUME / TRANSMISSION UNIFORMS ***************************************************************
    uniforms->attenuationDistance =                 _u("u_AttenuationDistance");
    uniforms->attenuationColor =                    _u("u_AttenuationColor");
    uniforms->transmissionFactor =                  _u("u_TransmissionFactor");
	uniforms->transmissionSampler =                 _u("u_TransmissionSampler");
    uniforms->transmissionUVSet =                   _u("u_TransmissionUVSet");
    uniforms->transmissionUVTransform =             _u("u_TransmissionUVTransform");
    uniforms->transmissionFramebufferSampler =      _u("u_TransmissionFramebufferSampler");
    uniforms->transmissionFramebufferSize =         _u("u_TransmissionFramebufferSize");
    uniforms->screenSize =                          _u("u_ScreenSize");
    uniforms->thickness =                           _u("u_ThicknessFactor");
    uniforms->thicknessSampler =                    _u("u_ThicknessSampler");
    uniforms->thicknessUVSet =                      _u("u_ThicknessUVSet");
    uniforms->thicknessUVTransform =                _u("u_ThicknessUVTransform");
    // *** CLEARCOAT UNIFORMS ***************************************************************************
    uniforms->clearcoatFactor =                     _u("u_ClearcoatFactor");
    uniforms->clearcoatRoughnessFactor =            _u("u_ClearcoatRoughnessFactor");
    uniforms->clearcoatSampler =                    _u("u_ClearcoatSampler");
    uniforms->clearcoatUVSet =                      _u("u_ClearcoatUVSet");
    uniforms->clearcoatUVTransform =                _u("u_ClearcoatUVTransform");
    uniforms->clearcoatRoughnessSampler =           _u("u_ClearcoatRoughnessSampler");
    uniforms->clearcoatRoughnessUVSet =             _u("u_ClearcoatRoughnessUVSet");
    uniforms->clearcoatRoughnessUVTransform =       _u("u_ClearcoatRoughnessUVTransform");
    uniforms->clearcoatNormalScale =                _u("u_ClearcoatNormalScale");
    uniforms->clearcoatNormalSampler =              _u("u_ClearcoatNormalSampler");
    uniforms->clearcoatNormalUVSet =                _u("u_ClearcoatNormalUVSet");
    uniforms->clearcoatNormalUVTransform =          _u("u_ClearcoatNormalUVTransform");
    // *** DIFFUSE TRANSMISSION UNIFORMS ****************************************************************
    uniforms->diffuseTransmissionFactor =           _u("u_DiffuseTransmissionFactor");
    uniforms->diffuseTransmissionSampler =          _u("u_DiffuseTransmissionSampler");
    uniforms->diffuseTransmissionUVSet =            _u("u_DiffuseTransmissionUVSet");
    uniforms->diffuseTransmissionUVTransform =      _u("u_DiffuseTransmissionUVTransform");
    uniforms->diffuseTransmissionColorFactor =      _u("u_DiffuseTransmissionColorFactor");
    uniforms->diffuseTransmissionColorSampler =     _u("u_DiffuseTransmissionColorSampler");
    uniforms->diffuseTransmissionColorUVSet =       _u("u_DiffuseTransmissionColorUVSet");
    uniforms->diffuseTransmissionColorUVTransform = _u("u_DiffuseTransmissionColorUVTransform");
    // *** LEGACY SUPPORT - PBR_SPECULARGLOSS ***********************************************************
    uniforms->diffuseFactor =                       _u("u_DiffuseFactor");
    uniforms->specularFactor =                      _u("u_SpecularFactor");
    uniforms->glossinessFactor =                    _u("u_GlossinessFactor");
    uniforms->diffuseSampler =                      _u("u_DiffuseSampler");
    uniforms->diffuseUVSet =                        _u("u_DiffuseUVSet");
    uniforms->diffuseUVTransform =                  _u("u_DiffuseUVTransform");
    uniforms->specularGlossinessSampler =           _u("u_SpecularGlossinessSampler");
    uniforms->specularGlossinessUVSet =             _u("u_SpecularGlossinessUVSet");
    uniforms->specularGlossinessUVTransform =       _u("u_SpecularGlossinessUVTransform");
    // *** [PARTIALLY SUPPORTED / IN DEVELOPMENT] UNIFORMS **********************************************
    uniforms->sheenColorFactor =                    _u("u_SheenColorFactor");
    uniforms->sheenRoughnessFactor =                _u("u_SheenRoughnessFactor");
    //
    uniforms->specularColorFactor =                 _u("u_KHR_materials_specular_specularColorFactor");
    uniforms->specularFactor =                      _u("u_KHR_materials_specular_specularFactor");
    //
    uniforms->jointsSampler =                      _u("u_jointsSampler");

}

// Function to construct a texture transformation matrix
FMAT3 setup_texture_transform_matrix(fastgltf::TextureTransform transform){
    FMAT3 rotation = FMAT3(0.f);
    FMAT3 scale = FMAT3(0.f);
    FMAT3 translation = FMAT3(0.f);
    FMAT3 result = FMAT3(0.f);

    float s =  std::sin(transform.rotation);
    float c =  std::cos(transform.rotation);
    rotation[0][0] = c;
    rotation[1][1] = c;
    rotation[0][1] = s;
    rotation[1][0] = -s;
    rotation[2][2] = 1.0f;

    scale[0][0] = transform.uvScale[0];
    scale[1][1] = transform.uvScale[1];
    scale[2][2] = 1.0f;

    translation[0][0] = 1.0f;
    translation[1][1] = 1.0f;
    translation[0][2] = transform.uvOffset[0];
    translation[1][2] = transform.uvOffset[1];
    translation[2][2] = 1.0f;

    result = translation * rotation;
    result = result * scale;
    return result;
}

void setup_background_environment(GLuint program, GLuint* vao, GLuint* indexBuffer, GLuint* vertexBuffer) {
    int32_t indices[] = {
        1, 2, 0,
        2, 3, 0,
        6, 2, 1,
        1, 5, 6,
        6, 5, 4,
        4, 7, 6,
        6, 3, 2,
        7, 3, 6,
        3, 7, 0,
        7, 4, 0,
        5, 1, 0,
        4, 5, 0
    };
    float verts[] = {
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f
    };
    GL_CALL(glUseProgram(program));
	// Create and bind the VAO
    GL_CALL(glCreateVertexArrays(1, vao));
    GL_CALL(glBindVertexArray(*vao));

    GL_CALL(glCreateBuffers(1, indexBuffer));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexBuffer));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    GL_CALL(glCreateBuffers(1, vertexBuffer));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW));

    GLint positionAttributeLocation = glGetAttribLocation( program, "a_position");

    // Specify the layout of the vertex data
    glVertexAttribPointer(positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(positionAttributeLocation);

    GL_CALL(glBindVertexArray(0));
    GL_CALL(glUseProgram(0));
	
}

// This is currently unused  (handled on the GPU) but that may change in the future, for optimization
FMAT3 setup_tangent_bitangent_normal_matrix(FVEC3 normal, FVEC4 tangent_and_w){
    FVEC3 bitangent = fastgltf::math::cross(normal, FVEC3(tangent_and_w[0], tangent_and_w[1], tangent_and_w[2]));
    FMAT3 r = FMAT3(0.f);
    r[0][0] = tangent_and_w[0];r[0][1] = tangent_and_w[1];r[0][2] = tangent_and_w[2];
    r[1][0] = bitangent[0];r[1][1] = bitangent[1];r[1][2] = bitangent[2];
    r[2][0] = normal[0];r[2][1] = normal[1];r[2][2] = normal[2];
    return r;
}

// Function to set the environment rotation matrix
void setup_environment_rotation_matrix(float env_rotation_angle, uint32_t shader_program) {
    mfloat_t rot[MAT4_SIZE];
    mat4_identity(rot);
    mat4_rotation_z(rot, to_radians(180.0));
    mat4_rotation_y(rot, to_radians(env_rotation_angle));

    mfloat_t ret[MAT3_SIZE];
    uint32_t _sz = 3 * sizeof(mfloat_t);
    memcpy(ret, &rot[0], _sz);     // Copy first three elements
    memcpy(&ret[3], &rot[4], _sz); // Copy next three elements
    memcpy(&ret[6], &rot[8], _sz); // Copy last three elements

    int32_t u_loc;
    GL_CALL(u_loc = glGetUniformLocation(shader_program, "u_EnvRotation"));
    GL_CALL(glUniformMatrix3fv(u_loc, 1, false, ret ));
}

// Function to set a uniform color
void setup_uniform_color(GLint uniform_loc, fastgltf::math::nvec3 color) {
    GL_CALL(glUniform3f(uniform_loc, static_cast<float>(color[0]), static_cast<float>(color[1]), static_cast<float>(color[2]) ) ); }


// Function to set a uniform color with alpha
void setup_uniform_color_alpha(GLint uniform_loc, fastgltf::math::nvec4 color){
    GL_CALL(glUniform4f(uniform_loc, static_cast<float>(color[0]), static_cast<float>(color[1]), static_cast<float>(color[2]), static_cast<float>(color[3]) ) ); }


// Function to set up a texture
uint32_t setup_texture(uint32_t tex_num, uint32_t tex_unit, int32_t tex_coord_index, 
                                    std::unique_ptr<fastgltf::TextureTransform>& tex_transform, 
                                    GLint sampler, GLint uv_set, GLint uv_transform) {
    GL_CALL(glBindTextureUnit(tex_num, tex_unit));
    GL_CALL(glUniform1i(sampler, tex_num));
    GL_CALL(glUniform1i(uv_set, tex_coord_index));
    if (tex_transform != NULL) GL_CALL(glUniformMatrix3fv(uv_transform, 1, GL_FALSE, &(setup_texture_transform_matrix(*tex_transform)[0][0])));
    tex_num++;
    return tex_num;
}

void glMessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam) {
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        std::cerr << message << '\n';
    } else {
        std::cout << message << '\n';
    }
}

// Function to clean up OpenGL output resources
void setup_cleanup_opengl_output(gl_renwin_state_t *state) {
    if (state) {
        // Delete the framebuffer
        if (state->framebuffer) {
            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0)); // Unbind the framebuffer
            GL_CALL(glDeleteFramebuffers(1, &state->framebuffer));
            state->framebuffer = 0; // Reset to avoid dangling pointer
        }

        // Delete the color texture
        if (state->texture) {
            GL_CALL(glDeleteTextures(1, &state->texture));
            state->texture = 0; // Reset to avoid dangling pointer
        }

        // Delete the depth texture
        if (state->renderbuffer) {
            GL_CALL(glDeleteTextures(1, &state->renderbuffer));
            state->renderbuffer = 0; // Reset to avoid dangling pointer
        }
    }
}

// Function to prepare opaque output for rendering
gl_renwin_state_t setup_opaque_output(uint32_t texture_width, uint32_t texture_height) {

    gl_renwin_state_t _ret;

    GLuint rtex;
    GL_CALL(glGenTextures(1, &rtex));
    _ret.texture = rtex;
    //const auto& metrics = get_viewer_metrics(viewer);

    GL_CALL(glBindTexture(GL_TEXTURE_2D, _ret.texture));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    GLuint rdepth;
    GL_CALL(glGenTextures(1, &rdepth));
    _ret.renderbuffer = rdepth;
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _ret.renderbuffer));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, texture_width, texture_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));

    GL_CALL(glGenFramebuffers(1, &_ret.framebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _ret.framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ret.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _ret.renderbuffer, 0));
    
    return _ret;
}

// Function to prepare OpenGL output for rendering
gl_renwin_state_t setup_primary_output(uint32_t texture_width, uint32_t texture_height, bool mipmaps_enabled) {

    gl_renwin_state_t _ret;

    GLuint rtex;
    GL_CALL(glGenTextures(1, &rtex));
    _ret.texture = rtex;
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _ret.texture));
    //GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmaps_enabled ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmaps_enabled ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    GLuint rdepth;
    GL_CALL(glGenTextures(1, &rdepth));
    _ret.renderbuffer = rdepth;
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _ret.renderbuffer));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, texture_width, texture_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));

    GL_CALL(glGenFramebuffers(1, &_ret.framebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _ret.framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ret.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _ret.renderbuffer, 0));
    
    return _ret;
}


//return value = cancel this frame?
bool setup_restore_opaque_output(gl_renwin_state_t _ret, uint32_t texture_w, uint32_t texture_h, bool prepare_bg) {

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _ret.framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ret.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _ret.renderbuffer, 0));
    GL_CALL(glViewport(0, 0, texture_w, texture_h));
    if (prepare_bg) {
        GL_CALL(glClearColor(208.0/255.0, 220.0/255.0, 230.0/255.0, 0.0f));
        GL_CALL(glClearDepth(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    if (glGetError() != GL_NO_ERROR) return true;
    return false;
}

bool checkOpenGLError() {
    bool had_error = false;
    GLenum error = glGetError();
    while (error != GL_NO_ERROR) {
        had_error = true;
        switch (error) {
            case GL_INVALID_ENUM:
                printf("OpenGL Error: GL_INVALID_ENUM\n");
                break;
            case GL_INVALID_VALUE:
                printf("OpenGL Error: GL_INVALID_VALUE\n");
                break;
            case GL_INVALID_OPERATION:
                printf("OpenGL Error: GL_INVALID_OPERATION\n");
                break;
            case GL_OUT_OF_MEMORY:
                printf("OpenGL Error: GL_OUT_OF_MEMORY\n");
                break;
            default:
                printf("OpenGL Error: Unknown error\n");
                break;
        }
        error = glGetError(); // Check for more errors
    }
    return had_error;
}

//return value = cancel this frame?
bool setup_restore_primary_output(gl_renwin_state_t _ret, uint32_t texture_w, uint32_t texture_h, uint32_t texture_offset_w, uint32_t texture_offset_h, bool prepare_bg) {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _ret.framebuffer));
    if (checkOpenGLError()) { std::cout << "AAA "; return true; }
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ret.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _ret.renderbuffer, 0));
    GL_CALL(glViewport(texture_offset_w, texture_offset_h, texture_w, texture_h));
    if (prepare_bg) {
        GL_CALL(glClearColor(208.0/255.0, 220.0/255.0, 230.0/255.0, 0.0f));
        GL_CALL(glClearDepth(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    if (glGetError() != GL_NO_ERROR) return true;
    return false;

}

// Function to finish the OpenGL frame
void setup_finish_frame( void ) {    
    GL_CALL(glDisable(GL_DEPTH_TEST));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    GL_CALL(glUseProgram(0));
}

void setup_view_proj_matrix_from_link(lv_gltf_view_t * viewer, pGltf_data_t link_data){
//    { auto _t = view;         set_matrix_view(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
//    { auto _t = perspective;  set_matrix_proj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
//   { auto _t = viewProj; set_matrix_viewproj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
//    set_cam_pos(viewer, view_pos[0], view_pos[1], view_pos[2]);
}


// Function to create a view-projection matrix from the camera
void setup_view_proj_matrix_from_camera( lv_gltf_view_t * viewer, int32_t _cur_cam_num, gl_viewer_desc_t * view_desc, const FMAT4 view_mat,  const FVEC3 view_pos, pGltf_data_t gltf_data, bool transmission_pass) {

    mfloat_t view[MAT4_SIZE];

    view[0] = view_mat[0][0];
    view[1] = view_mat[0][1];
    view[2] = view_mat[0][2];
    view[3] = view_mat[0][3];

    view[4] = view_mat[1][0];
    view[5] = view_mat[1][1];
    view[6] = view_mat[1][2];
    view[7] = view_mat[1][3];

    view[8] = view_mat[2][0];
    view[9] = view_mat[2][1];
    view[10] = view_mat[2][2];
    view[11] = view_mat[2][3];

    view[12] = view_mat[3][0];
    view[13] = view_mat[3][1];
    view[14] = view_mat[3][2];
    view[15] = view_mat[3][3];
    
    // Create Perspective Matrix
    mfloat_t perspective[MAT4_SIZE];
    auto _bradius = lv_gltf_data_get_radius(gltf_data);
    float _mindist = _bradius * 0.05f;
    float _maxdist = _bradius * std::max(2.0, 4.0 * view_desc->distance);
    const auto& asset = GET_ASSET(gltf_data);    
    if (_cur_cam_num > -1) {
        const fastgltf::Camera::Perspective * _perspcam = std::get_if<fastgltf::Camera::Perspective> (&(asset->cameras[_cur_cam_num].camera));
        if (_perspcam != NULL) {
            _mindist = _perspcam->znear;
            if ( _perspcam->zfar.has_value()) {
                _maxdist = _perspcam->zfar.value();
            } else {
                _maxdist = 5000.0f; } } }
    if (transmission_pass) {
        mat4_perspective_fov(perspective,to_radians(45.0), 256, 256, _mindist, _maxdist);
    } else {
        mat4_perspective_fov(perspective,to_radians(45.0), view_desc->render_width, view_desc->render_height, _mindist, _maxdist);
    }

    mfloat_t viewProj[MAT4_SIZE];
    mat4_multiply(viewProj, perspective, view);

    { auto _t = view;         set_matrix_view(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    { auto _t = perspective;  set_matrix_proj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    { auto _t = viewProj; set_matrix_viewproj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    set_cam_pos(viewer, view_pos[0], view_pos[1], view_pos[2]);
}

/**
* Function to create a view-projection matrix from a given pitch/yaw/distance 
* described within the view_desc parameter.
*/
void setup_view_proj_matrix( lv_gltf_view_t * viewer, gl_viewer_desc_t * view_desc, pGltf_data_t gltf_data, bool transmission_pass) {
    // Create Look-At Matrix
    const auto& _cenpos = FVEC3(view_desc->focal_x, view_desc->focal_y, view_desc->focal_z);
    float cen_x = _cenpos[0];
    float cen_y = _cenpos[1];
    float cen_z = _cenpos[2];
    if (view_desc->recenter_flag) {
        view_desc->recenter_flag = false;
        const auto& _autocenpos = lv_gltf_data_get_center(gltf_data);
        view_desc->focal_x = cen_x = _autocenpos[0];
        view_desc->focal_y = cen_y = _autocenpos[1];
        view_desc->focal_z = cen_z = _autocenpos[2];
    }

    auto _bradius = lv_gltf_data_get_radius(gltf_data);

    float radius = _bradius * 2.5;

    radius *= view_desc->distance;

    mfloat_t cam_position[VEC3_SIZE];
    mfloat_t cam_target[VEC3_SIZE];
    mfloat_t up[VEC3_SIZE];
    mfloat_t view[MAT4_SIZE];

    mfloat_t rcam_dir[VEC3_SIZE];
    vec3(rcam_dir, 0.0, 0.0, 1.0);

    mfloat_t rotation1[MAT3_SIZE];
    mfloat_t rotation2[MAT3_SIZE];
    mat3_identity(rotation1);
    mat3_identity(rotation2);
    mat3_rotation_x(rotation1, to_radians(view_desc->pitch));
    mat3_rotation_y(rotation2, to_radians(view_desc->yaw + view_desc->spin_degree_offset));
    vec3_multiply_mat3(rcam_dir, rcam_dir, rotation1);
    vec3_multiply_mat3(rcam_dir, rcam_dir, rotation2);

    mfloat_t ncam_dir[VEC3_SIZE];
    vec3_normalize(ncam_dir, rcam_dir);

    mat4_look_at(view,
        vec3(cam_position, cen_x + (ncam_dir[0]*radius), cen_y + (ncam_dir[1]*radius) , cen_z + (ncam_dir[2]*radius)),
        vec3(cam_target, cen_x, cen_y, cen_z),
        vec3(up, 0.0, 1.0, 0.0));
    // Create Perspective Matrix
    mfloat_t perspective[MAT4_SIZE];
    if (transmission_pass) {
        mat4_perspective_fov(perspective,to_radians(45.0), 256, 256, _bradius * 0.05f, _bradius * std::max(4.0, 8.0 * view_desc->distance));
    } else {
        mat4_perspective_fov(perspective,to_radians(45.0), view_desc->render_width, view_desc->render_height, _bradius * 0.05f, _bradius * std::max(4.0, 8.0 * view_desc->distance));
    }

    mfloat_t viewProj[MAT4_SIZE];
    mat4_multiply(viewProj, perspective, view);

    { auto _t = view;         set_matrix_view(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    { auto _t = perspective;  set_matrix_proj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    { auto _t = viewProj; set_matrix_viewproj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    set_cam_pos(viewer, cam_position[0], cam_position[1], cam_position[2]);

}

// Function to compile and load shaders
gl_renwin_shaderset_t setup_compile_and_load_shaders(lv_opengl_shader_cache_t * shaders) {
    lv_shader_key_value_t* all_defs = all_defines();
    auto _program = shaders->get_shader_program(shaders, 
        shaders->select_shader(shaders, "__MAIN__.frag", all_defs, all_defines_count()), 
        shaders->select_shader(shaders, "__MAIN__.vert", all_defs, all_defines_count()) );
    GL_CALL(glUseProgram(_program->program));
    gl_renwin_shaderset_t _shader_prog;
    _shader_prog.program = _program->program;
    _shader_prog.ready = true;

    return _shader_prog;
}

// Function to compile and load background shader
void setup_compile_and_load_bg_shader(lv_opengl_shader_cache_t * shaders) {
    lv_shader_key_value_t empty_defs[0] = {};
    lv_shader_key_value_t frag_defs[1] = {{"TONEMAP_KHR_PBR_NEUTRAL", NULL}};
    auto bg_program = shaders->get_shader_program(shaders, 
        shaders->select_shader(shaders, "cubemap.frag", frag_defs, 1), 
        shaders->select_shader(shaders, "cubemap.vert", empty_defs, 0) );
    shaders->bg_program = bg_program->program;
    setup_background_environment(shaders->bg_program, &shaders->bg_vao, &shaders->bg_indexBuf, &shaders->bg_vertexBuf);
}

// Function to draw the environment background
void setup_draw_environment_background(lv_opengl_shader_cache_t * shaders, lv_gltf_view_t * viewer, float blur) {
    GL_CALL(glBindVertexArray(shaders->bg_vao));
    GL_CALL(glUseProgram(shaders->bg_program));
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glDisable(GL_BLEND));
    GL_CALL(glDisable(GL_DEPTH_TEST));
    GL_CALL(glUniformMatrix4fv(glGetUniformLocation(shaders->bg_program, "u_ViewProjectionMatrix"), 1, false, GET_VIEWPROJ_MAT(viewer)->data() ));
    GL_CALL(glBindTextureUnit(0, shaders->lastEnv->specular));
    GL_CALL(glUniform1i(glGetUniformLocation(shaders->bg_program, "u_GGXEnvSampler"), 0));
    
    GL_CALL(glUniform1i(glGetUniformLocation(shaders->bg_program, "u_MipCount"), shaders->lastEnv->mipCount));
    GL_CALL(glUniform1f(glGetUniformLocation(shaders->bg_program, "u_EnvBlurNormalized"), blur));
    GL_CALL(glUniform1f(glGetUniformLocation(shaders->bg_program, "u_EnvIntensity"), 1.0f));
    GL_CALL(glUniform1f(glGetUniformLocation(shaders->bg_program, "u_Exposure"), 1.0f));

    setup_environment_rotation_matrix(shaders->lastEnv->angle, shaders->bg_program);

    // Bind the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaders->bg_indexBuf);

    // Bind the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, shaders->bg_vertexBuf);

    // Draw the elements
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

    GL_CALL(glBindVertexArray(0));
    return;

}

void lv_gltf_data_link_view_to( lv_gltf_data_t * link_target,  lv_gltf_data_t * link_source) {
    link_target->view_is_linked = true;
    link_target->linked_view_source = link_source;
}
