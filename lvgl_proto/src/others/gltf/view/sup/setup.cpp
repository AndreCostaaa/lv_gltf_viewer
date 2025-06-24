#include <string>
#include <iostream>

#include <GL/glew.h>
#include <drivers/glfw/lv_opengles_debug.h> /* GL_CALL */

//#include <memory> // Include necessary headers for std::unique_ptr

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-move"
#include "../../data/deps/fastgltf/include/fastgltf/core.hpp"
#include "../../data/deps/fastgltf/include/fastgltf/types.hpp"
#include "../../data/deps/fastgltf/include/fastgltf/tools.hpp"
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
#include "../../data/deps/mathc/mathc.h"

void set_matrix_view(_VIEW _viewer, FMAT4 _mat);
void set_matrix_proj(_VIEW _viewer, FMAT4 _mat);
void set_matrix_viewproj(_VIEW _viewer, FMAT4 _mat);

/**
 * @brief Retrieve uniform locations from a shader program.
 *
 * This function sets up the uniform locations for the specified shader program,
 * allowing for easy access to the shader's uniform variables.
 *
 * @param uniforms Pointer to a UniformLocs structure where the uniform locations will be stored.
 * @param _shader_prog_program The shader program from which to retrieve the uniform locations.
 */
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

/**
 * @brief Construct a texture transformation matrix.
 *
 * This function creates a transformation matrix based on the provided texture transform
 * parameters, which can be used for texture mapping in rendering.
 *
 * @param transform The texture transform parameters to be used for constructing the matrix.
 * @return A FMAT3 matrix representing the texture transformation.
 */
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

/**
 * @brief Set up the background environment for rendering.
 *
 * This function initializes the background environment by setting up the necessary
 * vertex array object (VAO), index buffer, and vertex buffer for rendering.
 *
 * @param program The shader program to be used for rendering the background.
 * @param vao Pointer to a GLuint where the vertex array object ID will be stored.
 * @param indexBuffer Pointer to a GLuint where the index buffer ID will be stored.
 * @param vertexBuffer Pointer to a GLuint where the vertex buffer ID will be stored.
 */
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

/**
 * @brief Set up the tangent, bitangent, and normal matrix.
 *
 * This function constructs a matrix that combines the normal, tangent, and bitangent vectors.
 * Currently unused (handled on the GPU), but may be utilized in the future for optimization.
 *
 * @param normal The normal vector (FVEC3) to be used in the matrix.
 * @param tangent_and_w The tangent vector and its w component (FVEC4) to be included in the matrix.
 * @return A FMAT3 matrix representing the tangent, bitangent, and normal transformation.
 */
FMAT3 setup_tangent_bitangent_normal_matrix(FVEC3 normal, FVEC4 tangent_and_w){
    FVEC3 bitangent = fastgltf::math::cross(normal, FVEC3(tangent_and_w[0], tangent_and_w[1], tangent_and_w[2]));
    FMAT3 r = FMAT3(0.f);
    r[0][0] = tangent_and_w[0];r[0][1] = tangent_and_w[1];r[0][2] = tangent_and_w[2];
    r[1][0] = bitangent[0];r[1][1] = bitangent[1];r[1][2] = bitangent[2];
    r[2][0] = normal[0];r[2][1] = normal[1];r[2][2] = normal[2];
    return r;
}

/**
 * @brief Set the environment rotation matrix.
 *
 * This function initializes the rotation matrix for the environment based on the specified angle,
 * which can be used in rendering to adjust the orientation of the environment.
 *
 * @param env_rotation_angle The angle of rotation for the environment in radians.
 * @param shader_program The shader program to which the rotation matrix will be applied.
 */
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

/**
 * @brief Set a uniform color in the shader.
 *
 * This function sets a uniform color value in the specified shader program, allowing for
 * consistent color rendering in the graphics pipeline.
 *
 * @param uniform_loc The location of the uniform variable in the shader program.
 * @param color The color value to be set, represented as a nvec3.
 */
void setup_uniform_color(GLint uniform_loc, fastgltf::math::nvec3 color) {
    GL_CALL(glUniform3f(uniform_loc, static_cast<float>(color[0]), static_cast<float>(color[1]), static_cast<float>(color[2]) ) ); 
}

/**
 * @brief Set a uniform color with alpha in the shader.
 *
 * This function sets a uniform color value with an alpha component in the specified shader program,
 * enabling transparency effects in rendering.
 *
 * @param uniform_loc The location of the uniform variable in the shader program.
 * @param color The color value to be set, represented as a nvec4, including the alpha component.
 */
void setup_uniform_color_alpha(GLint uniform_loc, fastgltf::math::nvec4 color){
    GL_CALL(glUniform4f(uniform_loc, static_cast<float>(color[0]), static_cast<float>(color[1]), static_cast<float>(color[2]), static_cast<float>(color[3]) ) ); 
}


/**
 * @brief Set up a texture for rendering.
 *
 * This function initializes a texture based on the provided parameters, including texture number,
 * texture unit, texture coordinate index, and transformation settings. It prepares the texture for
 * use in rendering operations.
 *
 * @param tex_num The texture number to be set up.
 * @param tex_unit The texture unit to which the texture will be bound.
 * @param tex_coord_index The index of the texture coordinates to be used.
 * @param tex_transform A unique pointer to a TextureTransform object that defines the texture transformation.
 * @param sampler The sampler object to be used for sampling the texture.
 * @param uv_set The UV set index for the texture coordinates.
 * @param uv_transform The transformation to be applied to the UV coordinates.
 * @return The texture ID generated for the setup texture.
 */
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

/**
 * @brief OpenGL message callback function.
 *
 * This function is called when an OpenGL error or notification occurs. It provides information
 * about the source, type, severity, and message of the OpenGL event, which can be useful for debugging.
 *
 * @param source The source of the OpenGL message.
 * @param type The type of the OpenGL message.
 * @param id The identifier for the message.
 * @param severity The severity level of the message.
 * @param length The length of the message string.
 * @param message The message string containing the details of the OpenGL event.
 * @param userParam User-defined parameter passed to the callback.
 */
void glMessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam) {
    LV_UNUSED(source);
    LV_UNUSED(type);
    LV_UNUSED(id);
    LV_UNUSED(length);
    LV_UNUSED(userParam);
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        std::cerr << message << '\n';
    } else {
        std::cout << message << '\n';
    }
}


/**
 * @brief Clean up OpenGL output resources.
 *
 * This function performs cleanup operations for the OpenGL output, releasing any resources
 * associated with the rendering window state.
 *
 * @param state Pointer to the rendering window state structure that holds OpenGL output information.
 */
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

/**
 * @brief Set up the opaque output for rendering.
 *
 * This function initializes the opaque output buffer, which is used to render the portions of the
 * scene that are opaque and may distort due to refractive elements in front of them. It prepares
 * the buffer for rendering operations based on the specified texture dimensions.
 *
 * @param texture_width The width of the texture for the opaque output.
 * @param texture_height The height of the texture for the opaque output.
 * @return A gl_renwin_state_t structure containing the state of the opaque output.
 */
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

/**
 * @brief Prepare OpenGL output for rendering.
 *
 * This function initializes the primary OpenGL output buffer based on the specified texture dimensions
 * and whether mipmaps are enabled. It sets up the necessary resources for rendering the primary scene.
 *
 * @param texture_width The width of the texture for the OpenGL output.
 * @param texture_height The height of the texture for the OpenGL output.
 * @param mipmaps_enabled A boolean indicating whether mipmaps should be generated for the texture.
 * @return A gl_renwin_state_t structure containing the state of the primary output.
 */
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

/**
 * @brief Restore the opaque output for rendering.
 *
 * This function attempts to restore the opaque output buffer, reinitializing it based on the provided
 * dimensions and background preparation flag. If an error occurs during the restoration process,
 * rendering for the current frame should be canceled.
 *
 * @param view_desc Pointer to the gl_viewer_desc_t structure containing view description parameters.
 * @param _ret The current state of the opaque output to be restored.
 * @param texture_w The width of the texture for the opaque output.
 * @param texture_h The height of the texture for the opaque output.
 * @param prepare_bg A boolean indicating whether to prepare the background during restoration.
 * @return A boolean indicating whether rendering should be canceled due to an error.
 */
bool setup_restore_opaque_output( gl_viewer_desc_t *view_desc, gl_renwin_state_t _ret, uint32_t texture_w, uint32_t texture_h, bool prepare_bg) {

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _ret.framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ret.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _ret.renderbuffer, 0));
    GL_CALL(glViewport(0, 0, texture_w, texture_h));
    if (prepare_bg) {
        GL_CALL(glClearColor(view_desc->bg_r/255.0f, view_desc->bg_g/255.0f, view_desc->bg_b/255.0f, view_desc->bg_a/255.0f));
        //GL_CALL(glClearColor(208.0/255.0, 220.0/255.0, 230.0/255.0, 0.0f));
        GL_CALL(glClearDepth(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    if (glGetError() != GL_NO_ERROR) return true;
    return false;
}

/**
 * @brief Check for OpenGL errors.
 *
 * This function checks for any OpenGL errors that may have occurred during rendering or setup.
 * It returns true if an error is detected, allowing for appropriate error handling.
 *
 * @return A boolean indicating whether an OpenGL error has occurred.
 */
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

/**
 * @brief Restore the primary output for rendering.
 *
 * This function rebinds any necessary buffers and performs any preparation work required for
 * repeat draws to an already existing OpenGL output. If an error occurs during the restoration,
 * rendering for the current frame should be canceled.
 *
 * @param view_desc Pointer to the gl_viewer_desc_t structure containing view description parameters.
 * @param _ret The current state of the primary output to be restored.
 * @param texture_w The width of the texture for the primary output.
 * @param texture_h The height of the texture for the primary output.
 * @param texture_offset_w The horizontal offset for the texture.
 * @param texture_offset_h The vertical offset for the texture.
 * @param prepare_bg A boolean indicating whether to prepare the background during restoration.
 * @return A boolean indicating whether rendering should be canceled due to an error.
 */
bool setup_restore_primary_output( gl_viewer_desc_t *view_desc, gl_renwin_state_t _ret, uint32_t texture_w, uint32_t texture_h, 
                                  uint32_t texture_offset_w, uint32_t texture_offset_h, bool prepare_bg) {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _ret.framebuffer));
    if (checkOpenGLError()) { std::cout << "AAA "; return true; }
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ret.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _ret.renderbuffer, 0));
    GL_CALL(glViewport(texture_offset_w, texture_offset_h, texture_w, texture_h));
    if (prepare_bg) {
        GL_CALL(glClearColor(view_desc->bg_r/255.0f, view_desc->bg_g/255.0f, view_desc->bg_b/255.0f, view_desc->bg_a/255.0f));
        GL_CALL(glClearDepth(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    if (glGetError() != GL_NO_ERROR) return true;
    return false;

}

/**
 * @brief Finish the OpenGL frame.
 *
 * This function finalizes the current OpenGL frame, performing any necessary operations
 * to complete rendering before presenting the frame to the display.
 */
void setup_finish_frame(void) {    
    GL_CALL(glDisable(GL_DEPTH_TEST));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    GL_CALL(glUseProgram(0));
}

//void setup_view_proj_matrix_from_link(lv_gltf_view_t * viewer, pGltf_data_t link_data){
//    { auto _t = view;         set_matrix_view(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
//    { auto _t = perspective;  set_matrix_proj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
//   { auto _t = viewProj; set_matrix_viewproj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
//    set_cam_pos(viewer, view_pos[0], view_pos[1], view_pos[2]);
//}


/**
 * @brief Create a view-projection matrix from the camera.
 *
 * This function constructs a view-projection matrix based on the specified camera parameters,
 * allowing for proper rendering of the scene from the camera's perspective. The transmission
 * pass flag indicates whether the matrix is being set up for the opaque render buffer.
 *
 * @param viewer Pointer to the lv_gltf_view_t structure representing the viewer.
 * @param _cur_cam_num The current camera number to be used for the view-projection matrix.
 * @param view_desc Pointer to the gl_viewer_desc_t structure containing view description parameters.
 * @param view_mat The current view matrix (FMAT4) to be used in the calculation.
 * @param view_pos The position of the viewer in the scene (FVEC3).
 * @param gltf_data Pointer to the GLTF data structure containing scene information.
 * @param transmission_pass A boolean indicating whether this setup is for the transmission pass.
 */
void setup_view_proj_matrix_from_camera(lv_gltf_view_t *viewer, int32_t _cur_cam_num, 
                                         gl_viewer_desc_t *view_desc, const FMAT4 view_mat, 
                                         const FVEC3 view_pos, pGltf_data_t gltf_data, 
                                         bool transmission_pass) {

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
    mfloat_t projection[MAT4_SIZE];
    auto _bradius = lv_gltf_data_get_radius(gltf_data);
    float _mindist = _bradius * 0.05f;
    float _maxdist = _bradius * std::max(2.0, 4.0 * view_desc->distance);
    const auto& asset = GET_ASSET(gltf_data);   
    float fov = view_desc->fov; 
    if (_cur_cam_num > -1) {
        const fastgltf::Camera::Perspective * _perspcam = std::get_if<fastgltf::Camera::Perspective> (&(asset->cameras[_cur_cam_num].camera));
        if (_perspcam != NULL) {
            _mindist = _perspcam->znear;
            fov = _perspcam->yfov;
            if ( _perspcam->zfar.has_value()) {
                _maxdist = _perspcam->zfar.value();
            } else {
                _maxdist = 5000.0f; } 
            if (transmission_pass) {
                mat4_perspective_fov(projection,(fov), 256, 256, _mindist, _maxdist);
            } else {
                mat4_perspective_fov(projection,(fov), view_desc->render_width, view_desc->render_height, _mindist, _maxdist);
            }
        } else {
            const fastgltf::Camera::Orthographic * _orthocam = std::get_if<fastgltf::Camera::Orthographic> (&(asset->cameras[_cur_cam_num].camera));
            if (_orthocam != NULL) {
                fov = 0;
                _mindist = _orthocam->znear;
                _maxdist = _orthocam->zfar;
                // Isometric view: create an orthographic projection
                float orthoSize = _orthocam->ymag; 
                float aspect = (float)view_desc->render_width / (float)view_desc->render_height;
                mat4_ortho(projection, -(orthoSize * aspect), (orthoSize * aspect), -orthoSize, orthoSize, _bradius * 0.05f, _bradius * std::max(4.0, 8.0 * view_desc->distance));
            }
        }
    }
    mfloat_t viewProj[MAT4_SIZE];
    mat4_multiply(viewProj, projection, view);

    { auto _t = view;         set_matrix_view(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    { auto _t = projection;  set_matrix_proj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    { auto _t = viewProj; set_matrix_viewproj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    set_cam_pos(viewer, view_pos[0], view_pos[1], view_pos[2]);
}

/**
 * @brief Create a view-projection matrix from pitch/yaw/distance.
 *
 * This function constructs a view-projection matrix based on the pitch, yaw, and distance
 * described within the view_desc parameter. It allows for flexible camera positioning and
 * orientation in the scene. The transmission pass flag indicates whether the matrix is being
 * set up for the opaque render buffer.
 *
 * @param viewer Pointer to the lv_gltf_view_t structure representing the viewer.
 * @param view_desc Pointer to the gl_viewer_desc_t structure containing view description parameters.
 * @param gltf_data Pointer to the GLTF data structure containing scene information.
 * @param transmission_pass A boolean indicating whether this setup is for the transmission pass.
 */
void setup_view_proj_matrix(lv_gltf_view_t *viewer, gl_viewer_desc_t *view_desc, 
                            pGltf_data_t gltf_data, bool transmission_pass) {
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

    // Create Projection Matrix
    mfloat_t projection[MAT4_SIZE];
    float fov = view_desc->fov;
    if (fov <= 0.0f) {
        // Isometric view: create an orthographic projection
        float orthoSize =  view_desc->distance * _bradius; // Adjust as needed
        float aspect = (float)view_desc->render_width / (float)view_desc->render_height;
        mat4_ortho(projection, -(orthoSize * aspect), (orthoSize * aspect), -orthoSize, orthoSize, _bradius * 0.05f, _bradius * std::max(4.0, 8.0 * view_desc->distance));
    } else {
        // Perspective view
        if (transmission_pass) {
            mat4_perspective_fov(projection, to_radians(fov), 256, 256, _bradius * 0.05f, _bradius * std::max(4.0, 8.0 * view_desc->distance));
        } else {
            mat4_perspective_fov(projection, to_radians(fov), view_desc->render_width, view_desc->render_height, _bradius * 0.05f, _bradius * std::max(4.0, 8.0 * view_desc->distance));
        }
    }

    mfloat_t viewProj[MAT4_SIZE];
    mat4_multiply(viewProj, projection, view);

    { auto _t = view;         set_matrix_view(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    { auto _t = projection;   set_matrix_proj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    { auto _t = viewProj; set_matrix_viewproj(viewer, FMAT4(_t[0], _t[1], _t[2], _t[3], _t[4], _t[5], _t[6], _t[7], _t[8], _t[9], _t[10], _t[11], _t[12], _t[13], _t[14], _t[15] ) ); }
    set_cam_pos(viewer, cam_position[0], cam_position[1], cam_position[2]);

}

/**
 * @brief Compile and load shaders.
 *
 * This function compiles and loads the shaders from the specified shader cache, preparing them
 * for use in rendering operations. It returns a structure containing the shader set information.
 *
 * @param shaders Pointer to the lv_opengl_shader_cache_t structure containing the shader cache.
 * @return A gl_renwin_shaderset_t structure representing the compiled and loaded shaders.
 */
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

/**
 * @brief Compile and load the background shader.
 *
 * This function compiles and loads the background shader from the specified shader cache,
 * preparing it for rendering the environment background.
 *
 * @param shaders Pointer to the lv_opengl_shader_cache_t structure containing the shader cache.
 */
void setup_compile_and_load_bg_shader(lv_opengl_shader_cache_t * shaders) {
    lv_shader_key_value_t empty_defs[0] = {};
    lv_shader_key_value_t frag_defs[1] = {{"TONEMAP_KHR_PBR_NEUTRAL", NULL}};
    auto bg_program = shaders->get_shader_program(shaders, 
        shaders->select_shader(shaders, "cubemap.frag", frag_defs, 1), 
        shaders->select_shader(shaders, "cubemap.vert", empty_defs, 0) );
    shaders->bg_program = bg_program->program;
    setup_background_environment(shaders->bg_program, &shaders->bg_vao, &shaders->bg_indexBuf, &shaders->bg_vertexBuf);
}

/**
 * @brief Draw the environment background.
 *
 * This function renders the environment background using the specified shaders and viewer parameters.
 * It allows for optional blurring effects to be applied to the background rendering.
 *
 * @param shaders Pointer to the lv_opengl_shader_cache_t structure containing the shader cache.
 * @param viewer Pointer to the lv_gltf_view_t structure representing the viewer.
 * @param blur The amount of blur to be applied to the background rendering.
 */
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

/**
 * @brief Link a view to GLTF data.
 *
 * This function links a target GLTF data structure to a source GLTF data structure, allowing for
 * shared access and manipulation of the linked data.
 *
 * @param link_target Pointer to the lv_gltf_data_t structure that will be linked to the source.
 * @param link_source Pointer to the lv_gltf_data_t structure that serves as the source for linking.
 */
void lv_gltf_data_link_view_to( lv_gltf_data_t * link_target,  lv_gltf_data_t * link_source) {
    link_target->view_is_linked = true;
    link_target->linked_view_source = link_source;
}
