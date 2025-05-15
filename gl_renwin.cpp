#include "lvgl/lvgl.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lvgl/src/drivers/glfw/lv_opengles_debug.h" /* GL_CALL */

#include "gltf_loader.h"
#include "shader_cache.h"
#include "ibl_sampler.h"
#include "gl_renwin.h"

//#include "lib/mathc/mathc.h"

#include <iostream>
#include "lib/fastgltf/include/fastgltf/core.hpp"
#include "lib/fastgltf/include/fastgltf/types.hpp"
#include "lib/fastgltf/include/fastgltf/tools.hpp"


#include <math.h> /* fabsf, fmaxf */
#include <stdlib.h> /* exit */
#define STRINGIFY(x) #x

#include <vector>

#ifndef STB_HAS_BEEN_INCLUDED
#define STB_HAS_BEEN_INCLUDED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#pragma GCC diagnostic pop
#endif

void addDefine( const char* defsymbol, const char* defvalue_or_null );

// It's simpler here to just declare the functions as part of the fastgltf::math namespace.
namespace fastgltf::math {
	/** Creates a right-handed view matrix */
	[[nodiscard]] auto lookAtRH(const fvec3& eye, const fvec3& center, const fvec3& up) noexcept {
		auto dir = normalize(center - eye);
		auto lft = normalize(cross(dir, up));
		auto rup = cross(lft, dir);

		mat<float, 4, 4> ret(1.f);
		ret.col(0) = { lft.x(), rup.x(), -dir.x(), 0.f };
		ret.col(1) = { lft.y(), rup.y(), -dir.y(), 0.f };
		ret.col(2) = { lft.z(), rup.z(), -dir.z(), 0.f };
		ret.col(3) = { -dot(lft, eye), -dot(rup, eye), dot(dir, eye), 1.f };
		return ret;
	}

	/**
	 * Creates a right-handed perspective matrix, with the near and far clips at -1 and +1, respectively.
	 * @param fov The FOV in radians
	 */
	[[nodiscard]] auto perspectiveRH(float fov, float ratio, float zNear, float zFar) noexcept {
		mat<float, 4, 4> ret(0.f);
		auto tanHalfFov = std::tan(fov / 2.f);
		ret.col(0).x() = 1.f / (ratio * tanHalfFov);
		ret.col(1).y() = 1.f / tanHalfFov;
		ret.col(2).z() = -(zFar + zNear) / (zFar - zNear);
		ret.col(2).w() = -1.f;
		ret.col(3).z() = -(2.f * zFar * zNear) / (zFar - zNear);
		return ret;
	}
} // namespace fastgltf::math

struct IndirectDrawCommand {
	std::uint32_t count;
	std::uint32_t instanceCount;
	std::uint32_t firstIndex;
	std::int32_t baseVertex;
	std::uint32_t baseInstance;
};

struct Vertex {
	fastgltf::math::fvec3 position;
	fastgltf::math::fvec3 normal;
	fastgltf::math::fvec4 tangent;
	fastgltf::math::fvec2 uv;
	fastgltf::math::fvec2 uv2;
};

struct Primitive {
    IndirectDrawCommand draw;
    GLenum primitiveType;
    GLenum indexType;
    GLuint vertexArray;

	GLuint vertexBuffer;
	GLuint indexBuffer;

    std::size_t materialUniformsIndex;
    GLuint albedoTexture;
    GLuint metalRoughTexture;
    GLuint occlusionTexture;
    GLuint normalTexture;
    GLuint diffuseTransmissionTexture;
    GLuint diffuseTransmissionColorTexture;
    
    GLint baseColorTexcoordIndex;
    GLint metallicRoughnessTexcoordIndex;
    GLint occlusionTexcoordIndex;
    GLint normalTexcoordIndex;
    GLint diffuseTransmissionTexcoordIndex;
    GLint diffuseTransmissionColorTexcoordIndex;
    
};

struct Mesh {
    GLuint drawsBuffer;
    std::vector<Primitive> primitives;
};

struct Texture {
    GLuint texture;
};

enum MaterialUniformFlags : std::uint32_t {
    None = 0 << 0,
    HasBaseColorTexture = 1 << 0,
    HasMetallicRoughnessTexture = 1 << 1,
    HasOcclusionTexture = 1 << 2,
    HasNormalTexture = 1 << 3,
};

struct gltf_data_t {
    
    fastgltf::Asset asset;
    bool load_success;

    float vertex_max[3];
    float vertex_min[3];
    float vertex_cen[3];
    float bound_radius;
    uint32_t vertex_count;
    uint32_t index_count;
    uint32_t __prim_type;
    uint32_t ebo;
    uint32_t vbo1;
    uint32_t vbo2;
    bool has_positions;
    bool has_normals;
    bool has_colors;
    bool has_uv1;
    bool has_uv2;
    int color_bytes;

    const char* filename;
    gl_renwin_state_t * render_state;
    bool render_state_ready;
};

unsigned int get_gltf_datastruct_datasize(void) {
    return sizeof(gltf_data_t);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
void __init_gltf_datastruct(pGltf_data_t _DataStructMem, const char * gltf_path) {
    gltf_data_t _newDataStruct;
    _newDataStruct.asset = fastgltf::Asset();
    _newDataStruct.filename = gltf_path;
    _newDataStruct.load_success = false;  
    _newDataStruct.render_state_ready = false;
    memcpy (_DataStructMem, &_newDataStruct, sizeof (gltf_data_t));
}
#pragma GCC diagnostic pop

struct UniformLocs {
    GLint viewProjectionMatrixUniform = GL_NONE;
    GLint modelMatrixUniform = GL_NONE;

    GLint envDiffuseSampler = GL_NONE;
    GLint envSpecularSampler = GL_NONE;
    GLint envSheenSampler = GL_NONE;
    GLint envGgxLutSampler = GL_NONE;
    GLint envCharlieLutSampler = GL_NONE;
    GLint envMipCount = GL_NONE;
    
    GLint exposure = GL_NONE;
	GLint roughnessFactor = GL_NONE;

    GLint baseColorFactor = GL_NONE;
	GLint baseColorSampler = GL_NONE;
	GLint baseColorUVSet = GL_NONE;
	GLint baseColorUVTransform = GL_NONE;

	GLint metallicFactor = GL_NONE;
    GLint metallicRoughnessSampler = GL_NONE;
	GLint metallicRoughnessUVSet = GL_NONE;
	GLint metallicRoughnessUVTransform = GL_NONE;

	GLint occlusionStrength = GL_NONE;
    GLint occlusionSampler = GL_NONE;
	GLint occlusionUVSet = GL_NONE;
	GLint occlusionUVTransform = GL_NONE;

	GLint normalScale = GL_NONE;
    GLint normalSampler = GL_NONE;
	GLint normalUVSet = GL_NONE;
	GLint normalUVTransform = GL_NONE;

    GLint diffuseTransmissionSampler = GL_NONE;
	GLint diffuseTransmissionUVSet = GL_NONE;
	GLint diffuseTransmissionUVTransform = GL_NONE;    

    GLint diffuseTransmissionColorSampler = GL_NONE;
	GLint diffuseTransmissionColorUVSet = GL_NONE;
	GLint diffuseTransmissionColorUVTransform = GL_NONE;

    GLint sheenColorFactor = GL_NONE;
    GLint sheenRoughnessFactor = GL_NONE;

    GLint specularColorFactor = GL_NONE;
    GLint specularFactor = GL_NONE;

    GLint diffuseTransmissionColorFactor = GL_NONE;
    GLint diffuseTransmissionFactor = GL_NONE;

};

struct _Viewer {
    fastgltf::Asset asset;

    std::vector<GLuint> bufferAllocations;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;
	std::vector<fastgltf::math::fmat4x4> cameras;
    std::vector<GLuint> materialBuffers;
    std::vector<UniformLocs> shaderUniforms;
    std::vector<gl_renwin_shaderset_t> shaderSets;

    bool overrideBaseColor = false;
    fastgltf::math::fvec4 overrideBaseColorFactor = fastgltf::math::fvec4(1.0f);
    
	fastgltf::math::ivec2 windowDimensions = fastgltf::math::ivec2(0);
    fastgltf::math::fmat4x4 viewMatrix = fastgltf::math::fmat4x4(1.0f);
    fastgltf::math::fmat4x4 projectionMatrix = fastgltf::math::fmat4x4(1.0f);

    fastgltf::math::fvec3 direction = fastgltf::math::fvec3(0.0f, 0.0f, -1.0f);
    fastgltf::math::fvec3 position = fastgltf::math::fvec3(0.0f, 0.0f, 0.0f);

	std::size_t sceneIndex = 0;
	std::size_t materialVariant = 0;
	fastgltf::Optional<std::size_t> cameraIndex = std::nullopt;
} ;

unsigned int get_viewer_datasize(void) {
    return sizeof(_Viewer);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
void __init_viewer_struct(pViewer _ViewerMem) {
    _Viewer _newViewer;
    _newViewer.windowDimensions = fastgltf::math::ivec2(0);
    _newViewer.viewMatrix = fastgltf::math::fmat4x4(1.0f);
    _newViewer.projectionMatrix = fastgltf::math::fmat4x4(1.0f);
    _newViewer.sceneIndex = 0;
	_newViewer.materialVariant = 0;
	_newViewer.cameraIndex = std::nullopt;
    memcpy (_ViewerMem, &_newViewer, sizeof (_Viewer));
}
#pragma GCC diagnostic pop

void glMessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam) {
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        std::cerr << message << '\n';
    } else {
        std::cout << message << '\n';
    }
}

gl_renwin_shaderset_t __compile_and_load_shaders(pShaderCache shaders) {
    key_value* all_defs = all_defines();
    auto _program = shaders->getShaderProgram(shaders, 
        shaders->selectShader(shaders, "__MAIN__.frag", all_defs, all_defines_count()), 
        shaders->selectShader(shaders, "__MAIN__.vert", all_defs, all_defines_count()) );
    GL_CALL(glUseProgram(_program->program));
    gl_renwin_shaderset_t _shader_prog;
    _shader_prog.program = _program->program;
    return _shader_prog;
}

#define __GLFW_SAMPLES 0x0002100D
gl_renwin_state_t  __prepare_opengl_output(uint32_t texture_w, uint32_t texture_h) {

    gl_renwin_state_t _ret;
    //const auto *glRenderer = glGetString(GL_RENDERER);
    //const auto *glVersion = glGetString(GL_VERSION);
    //std::cout << "GL Renderer: " << glRenderer << "\nGL Version: " << glVersion << '\n';

    GL_CALL(glEnable(GL_DEBUG_OUTPUT));
    GL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
    GL_CALL(glDebugMessageCallback(glMessageCallback, nullptr));

    GL_CALL(glGenTextures(1, &_ret.texture));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _ret.texture));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_w, texture_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 20));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _ret.texture));
    GL_CALL(glGenRenderbuffers(1, &_ret.renderbuffer));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, _ret.renderbuffer));
    GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, texture_w, texture_h));

    GL_CALL(glGenFramebuffers(1, &_ret.framebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _ret.framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ret.texture, 0));
    GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _ret.renderbuffer));

    GL_CALL(glViewport(0, 0, texture_w, texture_h));

    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glDepthFunc(GL_LESS));
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GL_CALL(glClearDepth(1.0f));
    GL_CALL(glDepthMask(true));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    return _ret;
}

//return value = cancel this frame?
bool  __restore_opengl_output(gl_renwin_state_t _ret, uint32_t texture_w, uint32_t texture_h) {

    GL_CALL(glBindTexture(GL_TEXTURE_2D, _ret.texture));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_w, texture_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 20));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
    glBindTexture(GL_TEXTURE_2D, _ret.texture);
    if (glGetError() != GL_NO_ERROR) return true;
    glBindRenderbuffer(GL_RENDERBUFFER, _ret.renderbuffer);
    if (glGetError() != GL_NO_ERROR) return true;
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, texture_w, texture_h);
    if (glGetError() != GL_NO_ERROR) return true;
    glBindFramebuffer(GL_FRAMEBUFFER, _ret.framebuffer);
    if (glGetError() != GL_NO_ERROR) return true;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ret.texture, 0);
    if (glGetError() != GL_NO_ERROR) return true;
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _ret.renderbuffer);
    if (glGetError() != GL_NO_ERROR) return true;

    GL_CALL(glViewport(0, 0, texture_w, texture_h));

    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glDepthFunc(GL_LESS));
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GL_CALL(glClearDepth(1.0f));
    GL_CALL(glDepthMask(true));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (glGetError() != GL_NO_ERROR) return true;

    return false;
}

fastgltf::math::fmat3x3 __construct_tangent_bitangent_normal_matrix(fastgltf::math::fvec3 normal, fastgltf::math::fvec4 tangentW){
    fastgltf::math::fvec3 bitangent = fastgltf::math::cross(normal, fastgltf::math::fvec3(tangentW[0], tangentW[1], tangentW[2]));
    fastgltf::math::fmat3x3 r = fastgltf::math::fmat3x3(0.f);
    r[0][0] = tangentW[0];r[0][1] = tangentW[1];r[0][2] = tangentW[2];
    r[0][0] = bitangent[0];r[0][1] = bitangent[1];r[0][2] = bitangent[2];
    r[0][0] = normal[0];r[0][1] = normal[1];r[0][2] = normal[2];
    return r;
}

fastgltf::math::fmat3x3 __construct_texture_transform_matrix(fastgltf::TextureTransform transform){
    fastgltf::math::fmat3x3 rotation = fastgltf::math::fmat3x3(0.f);
    fastgltf::math::fmat3x3 scale = fastgltf::math::fmat3x3(0.f);
    fastgltf::math::fmat3x3 translation = fastgltf::math::fmat3x3(0.f);
    fastgltf::math::fmat3x3 result = fastgltf::math::fmat3x3(0.f);

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

void __discover_defines(pGltf_data_t _dataobj, fastgltf::Mesh& mesh) {
    auto& asset = _dataobj->asset;
    //MaterialUniforms uniforms = {};
    for (auto it = mesh.primitives.begin(); it != mesh.primitives.end(); ++it) {
        _clearDefines();
        addDefine("USE_IBL", NULL ); 
        addDefine("USE_PUNCTUAL", NULL ); 
        //addDefine("LIGHT_COUNT", "1" );
        addDefine("MATERIAL_METALLICROUGHNESS", NULL );
        //addDefine("LINEAR_OUTPUT", NULL );
        addDefine("TONEMAP_KHR_PBR_NEUTRAL", NULL);
        auto* positionIt = it->findAttribute("POSITION");
        assert(positionIt != it->attributes.end()); // A mesh primitive is required to hold the POSITION attribute.
        assert(it->indicesAccessor.has_value()); // We specify GenerateMeshIndices, so we should always have indices
        if (it->materialIndex.has_value()) {
            auto& material = asset.materials[it->materialIndex.value()];
            auto& baseColorTexture = material.pbrData.baseColorTexture;
            if (baseColorTexture.has_value()) {
                auto& texture = asset.textures[baseColorTexture->textureIndex];
                if (texture.imageIndex.has_value()) {
                    if (baseColorTexture->transform ) {
                        addDefine("HAS_BASECOLOR_UV_TRANSFORM", NULL); } } }

            auto& metallicRoughnessTexture = material.pbrData.metallicRoughnessTexture;
            if (metallicRoughnessTexture.has_value()) {
                auto& texture = asset.textures[metallicRoughnessTexture->textureIndex];
                if (texture.imageIndex.has_value()) {
                    if (metallicRoughnessTexture->transform ) {
                        addDefine("HAS_METALLICROUGHNESS_UV_TRANSFORM", NULL); } } }

            auto& occlusionTexture = material.occlusionTexture;
            if (occlusionTexture.has_value()) {
                auto& texture = asset.textures[occlusionTexture->textureIndex];
                if (texture.imageIndex.has_value()) {
                    if (occlusionTexture->transform ) {
                        addDefine("HAS_OCCLUSION_UV_TRANSFORM", NULL); } } }

            auto& normalTexture = material.normalTexture;
            if (normalTexture.has_value()) {
                auto& texture = asset.textures[normalTexture->textureIndex];
                if (texture.imageIndex.has_value()) {
                    if (normalTexture->transform ) {
                        addDefine("HAS_NORMAL_UV_TRANSFORM", NULL); } } }

            if (material.sheen) {
                addDefine("MATERIAL_SHEEN", NULL); }

            if (material.specular) {
                addDefine("MATERIAL_SPECULAR", NULL); }

            /* commented out until fastgltf approves one of the diffuse transmission PR's 
            if (material.diffuseTransmission) {
                addDefine("MATERIAL_DIFFUSE_TRANSMISSION", NULL); 
                if (material.diffuseTransmission->diffuseTransmissionTexture.has_value()) {
                    addDefine("HAS_DIFFUSE_TRANSMISSION_MAP", NULL); }
                if (material.diffuseTransmission->diffuseTransmissionColorTexture.has_value()) {
                    addDefine("HAS_DIFFUSE_TRANSMISSION_COLOR_MAP", NULL);}
            }
            */

            if (material.occlusionTexture.has_value()) {
                //addDefine("USE_IBL", NULL);
                // To-do, check if ( material.occlusionTexture.value().strength != 0.0 )
                addDefine("HAS_OCCLUSION_MAP", NULL);
                //uniforms.flags |= MaterialUniformFlags::HasOcclusionTexture;
                if (material.occlusionTexture.value().transform) {
                    addDefine("HAS_OCCLUSION_UV_TRANSFORM", NULL); } }
        
            //uniforms.normalScale = 0.f;
            if (material.normalTexture.has_value()) {
                addDefine("HAS_NORMAL_MAP", NULL);
                //uniforms.flags |= MaterialUniformFlags::HasNormalTexture; 
            }
        
            //uniforms.baseColorFactor = material.pbrData.baseColorFactor;
            //uniforms.baseColor = fastgltf::math::fvec3(uniforms.baseColorFactor.x(), uniforms.baseColorFactor.y(), uniforms.baseColorFactor.z());
            if (material.pbrData.baseColorTexture.has_value()) {
                addDefine("HAS_BASE_COLOR_MAP", NULL);
                //uniforms.flags |= MaterialUniformFlags::HasBaseColorTexture; 
            }
        
            if (material.pbrData.metallicRoughnessTexture.has_value()) {
                addDefine("HAS_METALLIC_ROUGHNESS_MAP", NULL);
                //uniforms.flags |= MaterialUniformFlags::HasMetallicRoughnessTexture; 
            }
        
            if (material.transmission) {
                addDefine("MATERIAL_TRANSMISSION", NULL); }
        
            
            /* commented out until fastgltf approves one of the diffuse transmission PR's 
            if (material.diffuseTransmission) {
                addDefine("MATERIAL_DIFFUSE_TRANSMISSION", NULL); } */

        }
        {
            auto* normalIt = it->findAttribute("NORMAL");
            if (normalIt != it->attributes.end()) {
                assert(it->indicesAccessor.has_value()); // We specify GenerateMeshIndices, so we should always have indices
                // Normal
                auto& normalAccessor = asset.accessors[normalIt->accessorIndex];
                if (!normalAccessor.bufferViewIndex.has_value())
                    continue;
                addDefine("HAS_NORMAL_VEC3", NULL); } 
        }
        {
            auto* tangentIt = it->findAttribute("TANGENT");
            if (tangentIt != it->attributes.end()) {
                assert(it->indicesAccessor.has_value()); // We specify GenerateMeshIndices, so we should always have indices
                auto& tangentAccessor = asset.accessors[tangentIt->accessorIndex];
                if (!tangentAccessor.bufferViewIndex.has_value())
                    continue;
                addDefine("HAS_TANGENT_VEC4", NULL); } 
        }
        auto texcoordAttribute = std::string("TEXCOORD_0");
        if (const auto* texcoord = it->findAttribute(texcoordAttribute); texcoord != it->attributes.end()) {
            // Tex coord
            auto& texCoordAccessor = asset.accessors[texcoord->accessorIndex];
            if (!texCoordAccessor.bufferViewIndex.has_value())
                continue;
            //auto _tstr = (std::string("HAS_") + texcoordAttribute + std::string("_VEC2"));
            //char* _tccstr = (char*)malloc(_tstr.length()+1);
            //_tccstr[0] = '\0';
            //strcat(_tccstr, _tstr.c_str());
            //addDefine(_tccstr, NULL);
            addDefine("HAS_TEXCOORD_0_VEC2", NULL);
            //free(_tccstr);  // freeing this here causes segfault, the defines vector owns it now.
        }

        auto texcoordAttribute2 = std::string("TEXCOORD_1");
        if (const auto* texcoord = it->findAttribute(texcoordAttribute2); texcoord != it->attributes.end()) {
            // Tex coord
            auto& texCoordAccessor = asset.accessors[texcoord->accessorIndex];
            if (!texCoordAccessor.bufferViewIndex.has_value())
                continue;
            //auto _tstr = (std::string("HAS_") + texcoordAttribute2 + std::string("_VEC2"));
            //char* _tccstr = (char*)malloc(_tstr.length()+1);
            //_tccstr[0] = '\0';
            //strcat(_tccstr, _tstr.c_str());
            //addDefine(_tccstr, NULL);
            addDefine("HAS_TEXCOORD_0_VEC2", NULL);
            //free(_tccstr);  // freeing this here causes segfault, the defines vector owns it now.
        }
    }
    //std::cout << getDefineId() << "\n";
}

bool loadMesh(pViewer viewer, pGltf_data_t _dataobj, fastgltf::Mesh& mesh) {
    //auto& asset = viewer->asset;

    //__discover_defines(_dataobj, mesh);
    auto& asset = _dataobj->asset;
    Mesh outMesh = {};
    outMesh.primitives.resize(mesh.primitives.size());

    for (auto it = mesh.primitives.begin(); it != mesh.primitives.end(); ++it) {
        if (it->dracoCompression != NULL) {
            std::cout << "***DRACO COMPRESSION ***\n";
        }
		auto* positionIt = it->findAttribute("POSITION");
		assert(positionIt != it->attributes.end());     // A mesh primitive is required to hold the POSITION attribute.
		assert(it->indicesAccessor.has_value());        // We specify GenerateMeshIndices, so we should always have indices

        auto index = std::distance(mesh.primitives.begin(), it);
        auto& primitive = outMesh.primitives[index];

        // Generate the VAO
        GLuint vao;

        glCreateVertexArrays(1, &vao);
		
        primitive.primitiveType = fastgltf::to_underlying(it->type);
        primitive.vertexArray = vao;
        
        if (it->materialIndex.has_value()) {
            primitive.materialUniformsIndex = it->materialIndex.value() + 1; // Adjust for default material
            auto& material = asset.materials[it->materialIndex.value()];
			auto& baseColorTexture = material.pbrData.baseColorTexture;
            if (baseColorTexture.has_value()) {
                auto& texture = asset.textures[baseColorTexture->textureIndex];
                if (texture.imageIndex.has_value()) {
                    if (texture.name != "") 
                        std::cout << "baseColorTexture set to: " << texture.name << " (Image #" << texture.imageIndex.value() << ")\n";
                    else
                        std::cout << "baseColorTexture set to: (Image #" << texture.imageIndex.value() << ")\n";
                    primitive.albedoTexture = viewer->textures[texture.imageIndex.value()].texture;
                    if (baseColorTexture->transform ) {
                        if (baseColorTexture->transform->texCoordIndex.has_value()) {
                            primitive.baseColorTexcoordIndex = baseColorTexture->transform->texCoordIndex.value();
                        } else {
                            primitive.baseColorTexcoordIndex = material.pbrData.baseColorTexture->texCoordIndex;
                        }
                    } else {
                        primitive.baseColorTexcoordIndex = material.pbrData.baseColorTexture->texCoordIndex;
                    }
                }
            }
            auto& metallicRoughnessTexture = material.pbrData.metallicRoughnessTexture;
            if (metallicRoughnessTexture.has_value()) {
                auto& texture = asset.textures[metallicRoughnessTexture->textureIndex];
                if (texture.imageIndex.has_value()) {
                    if (texture.name != "") 
                        std::cout << "metallicRoughnessTexture set to: " << texture.name << " (Image #" << texture.imageIndex.value() << ")\n";
                    else
                        std::cout << "metallicRoughnessTexture set to: (Image #" << texture.imageIndex.value() << ")\n";
                    primitive.metalRoughTexture = viewer->textures[texture.imageIndex.value()].texture;
                    if (metallicRoughnessTexture->transform ) {
                        if (metallicRoughnessTexture->transform->texCoordIndex.has_value()) {
                            primitive.metallicRoughnessTexcoordIndex = metallicRoughnessTexture->transform->texCoordIndex.value();
                        } else {
                            primitive.metallicRoughnessTexcoordIndex = material.pbrData.metallicRoughnessTexture->texCoordIndex;
                        }
                    } else {
                        primitive.metallicRoughnessTexcoordIndex = material.pbrData.metallicRoughnessTexture->texCoordIndex;
                    }
                }
            }
            auto& occlusionTexture = material.occlusionTexture;
            if (occlusionTexture.has_value()) {
                auto& texture = asset.textures[occlusionTexture->textureIndex];
                if (texture.imageIndex.has_value()) {
                    if (texture.name != "") 
                        std::cout << "occlusionTexture set to: " << texture.name << " (Image #" << texture.imageIndex.value() << ")\n";
                    else
                        std::cout << "occlusionTexture set to: (Image #" << texture.imageIndex.value() << ")\n";
                    primitive.occlusionTexture = viewer->textures[texture.imageIndex.value()].texture;
                    if (occlusionTexture->transform ) {
                        if (occlusionTexture->transform->texCoordIndex.has_value()) {
                            primitive.occlusionTexcoordIndex = occlusionTexture->transform->texCoordIndex.value();
                        } else {
                            primitive.occlusionTexcoordIndex = material.occlusionTexture->texCoordIndex;
                        }
                    } else {
                        primitive.occlusionTexcoordIndex = material.occlusionTexture->texCoordIndex;
                    }
                }
            }
            //viewer->baseColorUVSet = glGetUniformLocation(_shader_prog.program, "u_BaseColorUVSet");
            auto& normalTexture = material.normalTexture;
            if (normalTexture.has_value()) {
                auto& texture = asset.textures[normalTexture->textureIndex];
                if (texture.imageIndex.has_value()) {
                    if (texture.name != "") 
                        std::cout << "normalTexture set to: " << texture.name << " (Image #" << texture.imageIndex.value() << ")\n";
                    else
                        std::cout << "normalTexture set to: (Image #" << texture.imageIndex.value() << ")\n";

                    primitive.normalTexture = viewer->textures[texture.imageIndex.value()].texture;

                    if (normalTexture->transform ) {
                        if (normalTexture->transform->texCoordIndex.has_value()) {
                            primitive.normalTexcoordIndex = normalTexture->transform->texCoordIndex.value();
                        } else {
                            primitive.normalTexcoordIndex = material.normalTexture->texCoordIndex;
                        }
                    } else {
                        primitive.normalTexcoordIndex = material.normalTexture->texCoordIndex;
                    }
                }
            }
            
            /* commented out until fastgltf approves one of the diffuse transmission PR's 
            if (material.diffuseTransmission) {
                auto& diffuseTransmissionTexture = material.diffuseTransmission->diffuseTransmissionTexture;
                if (diffuseTransmissionTexture.has_value()) {
                    auto& texture = asset.textures[diffuseTransmissionTexture.value().textureIndex];
                    if (texture.imageIndex.has_value()) {
                        if (texture.name != "") 
                            std::cout << "diffuseTransmissionTexture set to: " << texture.name << " (Image #" << texture.imageIndex.value() << ")\n";
                        else
                            std::cout << "diffuseTransmissionTexture set to: (Image #" << texture.imageIndex.value() << ")\n";
                        primitive.diffuseTransmissionTexture = viewer->textures[texture.imageIndex.value()].texture;
                        if (diffuseTransmissionTexture.value().transform ) {
                            if (diffuseTransmissionTexture.value().transform->texCoordIndex.has_value()) {
                                primitive.diffuseTransmissionTexcoordIndex = diffuseTransmissionTexture.value().transform->texCoordIndex.value();
                            } else {
                                primitive.diffuseTransmissionTexcoordIndex = diffuseTransmissionTexture.value().texCoordIndex;
                            }
                        } else {
                            primitive.diffuseTransmissionTexcoordIndex = diffuseTransmissionTexture.value().texCoordIndex;
                        }
                    }
                }
                auto& diffuseTransmissionColorTexture = material.diffuseTransmission->diffuseTransmissionColorTexture;
                if (diffuseTransmissionColorTexture.has_value()) {
                    auto& texture = asset.textures[diffuseTransmissionColorTexture.value().textureIndex];
                    if (texture.imageIndex.has_value()) {
                        if (texture.name != "") 
                            std::cout << "diffuseTransmissionColorTexture set to: " << texture.name << " (Image #" << texture.imageIndex.value() << ")\n";
                        else
                            std::cout << "diffuseTransmissionColorTexture set to: (Image #" << texture.imageIndex.value() << ")\n";
                        primitive.diffuseTransmissionColorTexture = viewer->textures[texture.imageIndex.value()].texture;
                        if (diffuseTransmissionColorTexture.value().transform ) {
                            if (diffuseTransmissionColorTexture.value().transform->texCoordIndex.has_value()) {
                                primitive.diffuseTransmissionColorTexcoordIndex = diffuseTransmissionColorTexture.value().transform->texCoordIndex.value();
                            } else {
                                primitive.diffuseTransmissionColorTexcoordIndex = diffuseTransmissionColorTexture.value().texCoordIndex;
                            }
                        } else {
                            primitive.diffuseTransmissionColorTexcoordIndex = diffuseTransmissionColorTexture.value().texCoordIndex;
                        }
                    }
                }
            }  */          
        } else {
			primitive.materialUniformsIndex = 0;
		}

        int _AN = 0;
        {
            // Position
            auto& positionAccessor = asset.accessors[positionIt->accessorIndex];
            if (!positionAccessor.bufferViewIndex.has_value())
                continue;

			// Create the vertex buffer for this primitive, and use the accessor tools to copy directly into the mapped buffer.
			glCreateBuffers(1, &primitive.vertexBuffer);
			glNamedBufferData(primitive.vertexBuffer, positionAccessor.count * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
			auto* vertices = static_cast<Vertex*>(glMapNamedBuffer(primitive.vertexBuffer, GL_WRITE_ONLY));
			fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, positionAccessor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
				vertices[idx].position = fastgltf::math::fvec3(pos.x(), pos.y(), pos.z());
				vertices[idx].normal = fastgltf::math::fvec3( );
				vertices[idx].tangent = fastgltf::math::fvec4();
				vertices[idx].uv = fastgltf::math::fvec2();
				vertices[idx].uv2 = fastgltf::math::fvec2();
			});

			glUnmapNamedBuffer(primitive.vertexBuffer);
            glEnableVertexArrayAttrib(vao, 0);
            glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vao, 0, 0);
			glVertexArrayVertexBuffer(vao, 0, primitive.vertexBuffer, 0, sizeof(Vertex));
            _AN++;
        }
        {
            auto* normalIt = it->findAttribute("NORMAL");
            if (normalIt != it->attributes.end()) {
                assert(it->indicesAccessor.has_value()); // We specify GenerateMeshIndices, so we should always have indices
    
                // Normal
                auto& normalAccessor = asset.accessors[normalIt->accessorIndex];
                if (!normalAccessor.bufferViewIndex.has_value())
                    continue;

                // Create the vertex buffer for this primitive, and use the accessor tools to copy directly into the mapped buffer.
                auto* vertices = static_cast<Vertex*>(glMapNamedBuffer(primitive.vertexBuffer, GL_WRITE_ONLY));
                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, normalAccessor, [&](fastgltf::math::fvec3 norm, std::size_t idx) {
                    vertices[idx].normal = fastgltf::math::fvec3(norm.x(), norm.y(), norm.z()); });
                glUnmapNamedBuffer(primitive.vertexBuffer);
                glEnableVertexArrayAttrib(vao, _AN);
                glVertexArrayAttribFormat(vao, _AN, 3, GL_FLOAT, GL_FALSE, 0);
                glVertexArrayAttribBinding(vao, _AN, _AN);
                glVertexArrayVertexBuffer(vao, _AN, primitive.vertexBuffer, offsetof(Vertex, normal), sizeof(Vertex));
                _AN++;
            }
        } 
        {
            auto* tangentIt = it->findAttribute("TANGENT");
            if (tangentIt != it->attributes.end()) {
                assert(it->indicesAccessor.has_value()); // We specify GenerateMeshIndices, so we should always have indices

                // Tangent
                auto& tangentAccessor = asset.accessors[tangentIt->accessorIndex];
                if (!tangentAccessor.bufferViewIndex.has_value())
                    continue;

                auto* vertices = static_cast<Vertex*>(glMapNamedBuffer(primitive.vertexBuffer, GL_WRITE_ONLY));
                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(asset, tangentAccessor, [&](fastgltf::math::fvec4 tang, std::size_t idx) {
                    vertices[idx].tangent = fastgltf::math::fvec4(tang.x(), tang.y(), tang.z(), tang.w());  });
                glUnmapNamedBuffer(primitive.vertexBuffer);
                glEnableVertexArrayAttrib(vao, _AN);
                glVertexArrayAttribFormat(vao, _AN, 4, GL_FLOAT, GL_FALSE, 0);
                glVertexArrayAttribBinding(vao, _AN, _AN);
                glVertexArrayVertexBuffer(vao, _AN, primitive.vertexBuffer, offsetof(Vertex, tangent), sizeof(Vertex));
                _AN++;
            } 
        }
        {
            auto texcoordAttribute = std::string("TEXCOORD_0");
            if (const auto* texcoord = it->findAttribute(texcoordAttribute); texcoord != it->attributes.end()) {
                // Tex coord 0
                auto& texCoordAccessor = asset.accessors[texcoord->accessorIndex];
                if (!texCoordAccessor.bufferViewIndex.has_value())
                    continue;

                auto* vertices = static_cast<Vertex*>(glMapNamedBuffer(primitive.vertexBuffer, GL_WRITE_ONLY));
                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset, texCoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx) {
                    vertices[idx].uv = fastgltf::math::fvec2(uv.x(), uv.y()); });
                glUnmapNamedBuffer(primitive.vertexBuffer);
                glEnableVertexArrayAttrib(vao, _AN);
                glVertexArrayAttribFormat(vao, _AN, 2, GL_FLOAT, GL_FALSE, 0);
                glVertexArrayAttribBinding(vao, _AN, _AN);
                glVertexArrayVertexBuffer(vao, _AN, primitive.vertexBuffer, offsetof(Vertex, uv), sizeof(Vertex));
                _AN++;
            }
        }
        {
            auto texcoordAttribute2 = std::string("TEXCOORD_1");
            if (const auto* texcoord = it->findAttribute(texcoordAttribute2); texcoord != it->attributes.end()) {
                // Tex coord 1
                auto& texCoordAccessor = asset.accessors[texcoord->accessorIndex];
                if (!texCoordAccessor.bufferViewIndex.has_value())
                    continue;
                
                auto* vertices = static_cast<Vertex*>(glMapNamedBuffer(primitive.vertexBuffer, GL_WRITE_ONLY));
                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset, texCoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx) {
                    vertices[idx].uv2 = fastgltf::math::fvec2(uv.x(), uv.y());
                });
                glUnmapNamedBuffer(primitive.vertexBuffer);
                glEnableVertexArrayAttrib(vao, _AN);
                glVertexArrayAttribFormat(vao, _AN, 2, GL_FLOAT, GL_FALSE, 0);
                glVertexArrayAttribBinding(vao, _AN, _AN);
                glVertexArrayVertexBuffer(vao, _AN, primitive.vertexBuffer, offsetof(Vertex, uv2), sizeof(Vertex));
                _AN++;
            }
        }

        // Generate the indirect draw command
        auto& draw = primitive.draw;
        draw.instanceCount = 1;
        draw.baseInstance = 0;
        draw.baseVertex = 0;
		draw.firstIndex = 0;

        auto& indexAccessor = asset.accessors[it->indicesAccessor.value()];
        if (!indexAccessor.bufferViewIndex.has_value())
            return false;
        draw.count = static_cast<std::uint32_t>(indexAccessor.count);

		// Create the index buffer and copy the indices into it.
		glCreateBuffers(1, &primitive.indexBuffer);
		if (indexAccessor.componentType == fastgltf::ComponentType::UnsignedByte || indexAccessor.componentType == fastgltf::ComponentType::UnsignedShort) {
        	primitive.indexType = GL_UNSIGNED_SHORT;
			glNamedBufferData(primitive.indexBuffer,
							  static_cast<GLsizeiptr>(indexAccessor.count * sizeof(std::uint16_t)), nullptr,
							  GL_STATIC_DRAW);
			auto* indices = static_cast<std::uint16_t*>(glMapNamedBuffer(primitive.indexBuffer, GL_WRITE_ONLY));
			fastgltf::copyFromAccessor<std::uint16_t>(asset, indexAccessor, indices);
			glUnmapNamedBuffer(primitive.indexBuffer);
		} else {
        	primitive.indexType = GL_UNSIGNED_INT;
			glNamedBufferData(primitive.indexBuffer,
							  static_cast<GLsizeiptr>(indexAccessor.count * sizeof(std::uint32_t)), nullptr,
							  GL_STATIC_DRAW);
			auto* indices = static_cast<std::uint32_t*>(glMapNamedBuffer(primitive.indexBuffer, GL_WRITE_ONLY));
			fastgltf::copyFromAccessor<std::uint32_t>(asset, indexAccessor, indices);
			glUnmapNamedBuffer(primitive.indexBuffer);
		}
        glVertexArrayElementBuffer(vao, primitive.indexBuffer);
    }

    // Create the buffer holding all of our primitive structs.
    glCreateBuffers(1, &outMesh.drawsBuffer);
    glNamedBufferData(outMesh.drawsBuffer, static_cast<GLsizeiptr>(outMesh.primitives.size() * sizeof(Primitive)),
                      outMesh.primitives.data(), GL_STATIC_DRAW);

    
    viewer->meshes.emplace_back(outMesh);
    return true;
}

bool loadImage(pShaderCache shaders, pViewer viewer, pGltf_data_t _dataobj, fastgltf::Image& image, unsigned int index) {

    auto& asset = _dataobj->asset;
    auto getLevelCount = [](int width, int height) -> GLsizei {
        return static_cast<GLsizei>(1 + floor(log2(width > height ? width : height)));
    };

    std::string _tex_id =  std::string(_dataobj->filename) + "_IMG" + std::to_string(index);
    unsigned long int hash = c_stringHash(_tex_id.c_str(), 0);

    GLuint texture = shaders->getCachedTexture(shaders, hash);
    if (texture == GL_NONE) {
        std::cout << "Image: (" << image.name << ") [" << _tex_id << "] -> " << std::to_string(hash) << "\n";
        // ----
        //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        //glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        //glBindTexture(GL_TEXTURE_2D, texture);
        std::visit(fastgltf::visitor {
            [](auto& arg) {},
            [&](fastgltf::sources::URI& filePath) {
                assert(filePath.fileByteOffset == 0); // We don't support offsets with stbi.
                assert(filePath.uri.isLocalPath()); // We're only capable of loading local files.
                int width, height, nrChannels;
                std::cout << "Loading image: " << image.name << "\n";
                const std::string path(filePath.uri.path().begin(), filePath.uri.path().end()); // Thanks C++.
                unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
                glTextureStorage2D(texture, getLevelCount(width, height), GL_RGBA8, width, height);
                glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            },
            [&](fastgltf::sources::Array& vector) {
                int width, height, nrChannels;
                std::cout << "Unpacking image data: " << image.name << "\n";
                unsigned char *data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(vector.bytes.data()), static_cast<int>(vector.bytes.size()), &width, &height, &nrChannels, 4);
                glTextureStorage2D(texture, getLevelCount(width, height), GL_RGBA8, width, height);
                glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            },
            [&](fastgltf::sources::BufferView& view) {
                auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                auto& buffer = asset.buffers[bufferView.bufferIndex];
                // Yes, we've already loaded every buffer into some GL buffer. However, with GL it's simpler
                // to just copy the buffer data again for the texture. Besides, this is just an example.

                std::cout << "Unpacking image bufferView: " << image.name << " from " << bufferView.byteLength << " bytes.\n";
                std::visit(fastgltf::visitor {
                    // We only care about VectorWithMime here, because we specify LoadExternalBuffers, meaning
                    // all buffers are already loaded into a vector.
                    [](auto& arg) {},
                    [&](fastgltf::sources::Array& vector) {
                        int width, height, nrChannels;
                        unsigned char* data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(vector.bytes.data() + bufferView.byteOffset),
                                                                    static_cast<int>(bufferView.byteLength), &width, &height, &nrChannels, 4);
                        std::cout << "width / height: " << width << ", " << height << "\n";
                        glTextureStorage2D(texture, getLevelCount(width, height), GL_RGBA8, width, height);
                        glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
                        stbi_image_free(data);
                    }
                }, buffer.data);
            },
        }, image.data);

        glGenerateTextureMipmap(texture);
        shaders->setTextureCacheItem(shaders, hash, texture);
    }
    // ----

    viewer->textures.emplace_back(Texture { texture });
    return true;
}

bool loadCamera(pViewer viewer, fastgltf::Camera& camera) {
    return true;
	// The following matrix math is for the projection matrices as defined by the glTF spec:
	// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#projection-matrices
	std::visit(fastgltf::visitor {
		[&](fastgltf::Camera::Perspective& perspective) {
			fastgltf::math::fmat4x4 mat(0.0f);

			assert(viewer->windowDimensions[0] != 0 && viewer->windowDimensions[1] != 0);
			auto aspectRatio = perspective.aspectRatio.value_or(
				static_cast<float>(viewer->windowDimensions[0]) / static_cast<float>(viewer->windowDimensions[1]));
			mat[0][0] = 1.f / (aspectRatio * tan(0.5f * perspective.yfov));
			mat[1][1] = 1.f / (tan(0.5f * perspective.yfov));
			mat[2][3] = -1;

			if (perspective.zfar.has_value()) {
				// Finite projection matrix
				mat[2][2] = (*perspective.zfar + perspective.znear) / (perspective.znear - *perspective.zfar);
				mat[3][2] = (2 * *perspective.zfar * perspective.znear) / (perspective.znear - *perspective.zfar);
			} else {
				// Infinite projection matrix
				mat[2][2] = -1;
				mat[3][2] = -2 * perspective.znear;
			}
			viewer->cameras.emplace_back(mat);
		},
		[&](fastgltf::Camera::Orthographic& orthographic) {
			fastgltf::math::fmat4x4 mat(1.0f);
			mat[0][0] = 1.f / orthographic.xmag;
			mat[1][1] = 1.f / orthographic.ymag;
			mat[2][2] = 2.f / (orthographic.znear - orthographic.zfar);
			mat[3][2] = (orthographic.zfar + orthographic.znear) / (orthographic.znear - orthographic.zfar);
			viewer->cameras.emplace_back(mat);
		},
	}, camera.camera);
	return true;
}

void (*load_progress_callback)(const char*, const char* , float, float, float, float) = NULL;

bool setLoadPhaseCallback(void (*_load_progress_callback)(const char*, const char* , float, float, float, float)) {
    load_progress_callback= _load_progress_callback;
}

void set_envrotation_matrix(int _shader_prog_program) {
    float _envRotationDegrees = 0.f;//to_degrees(TEMP_viewYaw);

    mfloat_t rot[MAT4_SIZE];
    mat4_identity(rot);
    mat4_rotation_z(rot, to_radians(180.0));
    mat4_rotation_y(rot, to_radians(_envRotationDegrees));
    mfloat_t ret[MAT3_SIZE];
    ret[0] = rot[0];
    ret[1] = rot[1];
    ret[2] = rot[2];
    ret[3] = rot[4];
    ret[4] = rot[5];
    ret[5] = rot[6];
    ret[6] = rot[8];
    ret[7] = rot[9];
    ret[8] = rot[10];

    int u_loc;
    GL_CALL(u_loc = glGetUniformLocation(_shader_prog_program, "u_EnvRotation"));
    GL_CALL(glUniformMatrix3fv(u_loc, 1, false, ret ));
}

void make_viewProj_matrix(gl_viewer_desc_t view_desc, float temp_elevation, pGltf_data_t gltf_data, int _shader_prog_program ) {
    // Create Look-At Matrix
    float cen_x = gltf_data->vertex_cen[0];
    float cen_y = gltf_data->vertex_cen[1];
    float cen_z = gltf_data->vertex_cen[2];
    float radius = gltf_data->bound_radius * 2.5;

    radius *= view_desc.distance;

    mfloat_t cam_position[VEC3_SIZE];
    mfloat_t cam_target[VEC3_SIZE];
    mfloat_t up[VEC3_SIZE];
    mfloat_t view[MAT4_SIZE];

    mfloat_t rcam_dir[VEC3_SIZE];
    vec3(rcam_dir, 0.0, 0.0, 1.0);

    mfloat_t rotation[MAT3_SIZE];
    mat3_identity(rotation);
    mat3_rotation_x(rotation, to_radians(view_desc.pitch));
    mat3_rotation_y(rotation, to_radians(view_desc.yaw));
    vec3_multiply_mat3(rcam_dir, rcam_dir, rotation);

    mfloat_t ncam_dir[VEC3_SIZE];
    vec3_normalize(ncam_dir, rcam_dir);

    mat4_look_at(view,
        vec3(cam_position, cen_x + (ncam_dir[0]*radius), cen_y + (ncam_dir[1]*radius) + (temp_elevation * radius), cen_z + (ncam_dir[2]*radius)),
        vec3(cam_target, cen_x, cen_y+ (temp_elevation * radius), cen_z),
        vec3(up, 0.0, -1.0, 0.0));
    
    // Create Perspective Matrix
    mfloat_t perspective[MAT4_SIZE];
    mat4_perspective_fov(perspective,to_radians(45.0), 256, 192, 0.001f, gltf_data->bound_radius * std::max(2.0, 4.0 * view_desc.distance));
    mfloat_t viewProj[MAT4_SIZE];
    mat4_multiply(viewProj, perspective, view);

    int viewmat_loc;
    GL_CALL(viewmat_loc = glGetUniformLocation(_shader_prog_program, "u_ViewProjectionMatrix"));
    GL_CALL(glUniformMatrix4fv(viewmat_loc, 1, false, viewProj ));
    int camera_loc;
    GL_CALL(camera_loc = glGetUniformLocation(_shader_prog_program, "u_Camera"));
    GL_CALL(glUniform3f(camera_loc,  cam_position[0], cam_position[1], cam_position[2])); 
}

size_t get_mesh_total_vertex_count(fastgltf::Asset* asset, fastgltf::Mesh& mesh) {
    size_t _outcount = 0;
    if (mesh.primitives.size() > 0) {
        auto it = mesh.primitives.begin();
        auto* positionIt = it->findAttribute("POSITION");
        auto& positionAccessor = asset->accessors[positionIt->accessorIndex];
        if (positionAccessor.bufferViewIndex.has_value()) {
            _outcount += positionAccessor.count;
        }
    }
    return _outcount;
}

void __set_initial_bounds(pGltf_data_t _retdata, fastgltf::Asset* asset, fastgltf::math::fmat4x4 matrix, fastgltf::Mesh& mesh) {
    if (mesh.primitives.size() > 0) {
        auto it = mesh.primitives.begin();
        auto* positionIt = it->findAttribute("POSITION");
        auto& positionAccessor = asset->accessors[positionIt->accessorIndex];
        if (positionAccessor.bufferViewIndex.has_value()) {
            if (positionAccessor.min.has_value() && positionAccessor.max.has_value()) {
                fastgltf::math::fvec4 _tmin = fastgltf::math::fvec4(
                    (float)(positionAccessor.min.value().get<double>((size_t)0)),
                    (float)(positionAccessor.min.value().get<double>((size_t)1)),
                    (float)(positionAccessor.min.value().get<double>((size_t)2)),
                    0.f);
                _tmin = matrix * _tmin;

                fastgltf::math::fvec4 _tmax = fastgltf::math::fvec4(
                    (float)(positionAccessor.max.value().get<double>((size_t)0)),
                    (float)(positionAccessor.max.value().get<double>((size_t)1)),
                    (float)(positionAccessor.max.value().get<double>((size_t)2)),
                    0.f);
                _tmax = matrix * _tmax;

                _retdata->vertex_max[0] = std::max(_tmin.x(), _tmax.x());
                _retdata->vertex_max[1] = std::max(_tmin.y(), _tmax.y());
                _retdata->vertex_max[2] = std::max(_tmin.z(), _tmax.z());
                _retdata->vertex_min[0] = std::min(_tmin.x(), _tmax.x());
                _retdata->vertex_min[1] = std::min(_tmin.y(), _tmax.y());
                _retdata->vertex_min[2] = std::min(_tmin.z(), _tmax.z());
                _retdata->vertex_cen[0] = (_retdata->vertex_max[0] + _retdata->vertex_min[0]) / 2.0f;
                _retdata->vertex_cen[1] = (_retdata->vertex_max[1] + _retdata->vertex_min[1]) / 2.0f;
                _retdata->vertex_cen[2] = (_retdata->vertex_max[2] + _retdata->vertex_min[2]) / 2.0f;
                float size_x = _retdata->vertex_max[0] - _retdata->vertex_min[0];
                float size_y = _retdata->vertex_max[1] - _retdata->vertex_min[1];
                float size_z = _retdata->vertex_max[2] - _retdata->vertex_min[2];
                _retdata->bound_radius = std::sqrt((size_x * size_x) + (size_y * size_y) + (size_z * size_z)) / 2.0f;
            }
        }
    }
    return;
}

void __grow_bounds_to_include(pGltf_data_t _retdata, fastgltf::Asset* asset, fastgltf::math::fmat4x4 matrix, fastgltf::Mesh& mesh) {
    if (mesh.primitives.size() > 0) {
        auto it = mesh.primitives.begin();
        auto* positionIt = it->findAttribute("POSITION");
        auto& positionAccessor = asset->accessors[positionIt->accessorIndex];
        if (positionAccessor.bufferViewIndex.has_value()) {
            if (positionAccessor.min.has_value() && positionAccessor.max.has_value()) {
                fastgltf::math::fvec4 _tmin = fastgltf::math::fvec4(
                    (float)(positionAccessor.min.value().get<double>((size_t)0)),
                    (float)(positionAccessor.min.value().get<double>((size_t)1)),
                    (float)(positionAccessor.min.value().get<double>((size_t)2)),
                    0.f);
                _tmin = matrix * _tmin;
                fastgltf::math::fvec4 _tmax = fastgltf::math::fvec4(
                    (float)(positionAccessor.max.value().get<double>((size_t)0)),
                    (float)(positionAccessor.max.value().get<double>((size_t)1)),
                    (float)(positionAccessor.max.value().get<double>((size_t)2)),
                    0.f);
                _tmax = matrix * _tmax;
                _retdata->vertex_max[0] = std::max(std::max(_retdata->vertex_max[0], _tmin.x()), _tmax.x());
                _retdata->vertex_max[1] = std::max(std::max(_retdata->vertex_max[1], _tmin.y()), _tmax.y());
                _retdata->vertex_max[2] = std::max(std::max(_retdata->vertex_max[2], _tmin.z()), _tmax.z());
                _retdata->vertex_min[0] = std::min(std::min(_retdata->vertex_min[0], _tmin.x()), _tmax.x());
                _retdata->vertex_min[1] = std::min(std::min(_retdata->vertex_min[1], _tmin.y()), _tmax.y());
                _retdata->vertex_min[2] = std::min(std::min(_retdata->vertex_min[2], _tmin.z()), _tmax.z());
                _retdata->vertex_cen[0] = (_retdata->vertex_max[0] + _retdata->vertex_min[0]) / 2.0f;
                _retdata->vertex_cen[1] = (_retdata->vertex_max[1] + _retdata->vertex_min[1]) / 2.0f;
                _retdata->vertex_cen[2] = (_retdata->vertex_max[2] + _retdata->vertex_min[2]) / 2.0f;
                float size_x = _retdata->vertex_max[0] - _retdata->vertex_min[0];
                float size_y = _retdata->vertex_max[1] - _retdata->vertex_min[1];
                float size_z = _retdata->vertex_max[2] - _retdata->vertex_min[2];
                _retdata->bound_radius = std::sqrt((size_x * size_x) + (size_y * size_y) + (size_z * size_z)) / 2.0f;
            }
        }
    }
}

void __discover_attributes(pGltf_data_t _retdata, fastgltf::Asset* asset, fastgltf::Mesh& mesh) {
    if (mesh.primitives.size() > 0) {
        auto it = mesh.primitives.begin();

        auto* positionIt = it->findAttribute("POSITION");
        auto& positionAccessor = asset->accessors[positionIt->accessorIndex];
        _retdata->has_positions = positionAccessor.bufferViewIndex.has_value();

        auto* normalIt = it->findAttribute("NORMAL");
        auto& normalAccessor = asset->accessors[normalIt->accessorIndex];
        _retdata->has_normals = normalAccessor.bufferViewIndex.has_value();

        auto* color0It = it->findAttribute("COLOR_0");
        if (color0It != it->attributes.end()) {
            std::cout << "COLOR_0 Defined\n";
            auto& color0Accessor = asset->accessors[color0It->accessorIndex];
            fastgltf::AccessorType _ctype = asset->accessors[color0It->accessorIndex].type;
            if (_ctype == fastgltf::AccessorType::Vec3) {
                std::cout << "COLOR_0 Defined with 3 channels (RGB) per vertex\n";
                _retdata->color_bytes = 3;
                _retdata->has_colors = true;
            } else if (_ctype == fastgltf::AccessorType::Vec4) {
                std::cout << "COLOR_0 Defined with 4 channels (RGB) per vertex\n";
                _retdata->color_bytes = 4;
                _retdata->has_colors = true;
            }
        } else {
            _retdata->has_colors = false;
        }
        auto* uv1It = it->findAttribute("TEXCOORD_0");
        auto& uv1Accessor = asset->accessors[uv1It->accessorIndex];
        _retdata->has_uv1 = uv1Accessor.bufferViewIndex.has_value();

        _retdata->index_count = 0;
        auto& indexAccessor = asset->accessors[it->indicesAccessor.value()];
        if (indexAccessor.bufferViewIndex.has_value()) {
            _retdata->index_count = (uint32_t)indexAccessor.count;
        }
        _retdata->__prim_type = (uint32_t)it->type;
    }
}

void __debug_print_node(fastgltf::Asset& asset, fastgltf::Node node, std::size_t _depth) {
    std::size_t _tabwidth = 4;
    std::size_t _insetspaces = _tabwidth * _depth;
    char _tabstr[_insetspaces + 1];
    memset(_tabstr, ' ', _insetspaces);
    _tabstr[_insetspaces] = '\0';

    if (_depth == 1) {
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
    } else if (const auto* pMat4 = std::get_if<fastgltf::math::fmat4x4>(&node.transform)) {
        std::cout << _tabstr << ": 4x4 Matrix [ " << pMat4->col(0)[0] << ", " << pMat4->col(0)[1] << ", " << pMat4->col(0)[2] << ", " << pMat4->col(0)[3] << " ]\n"; 
        std::cout << _tabstr << ":            [ " << pMat4->col(1)[0] << ", " << pMat4->col(1)[1] << ", " << pMat4->col(1)[2] << ", " << pMat4->col(1)[3] << " ]\n"; 
        std::cout << _tabstr << ":            [ " << pMat4->col(2)[0] << ", " << pMat4->col(2)[1] << ", " << pMat4->col(2)[2] << ", " << pMat4->col(2)[3] << " ]\n"; 
        std::cout << _tabstr << ":            [ " << pMat4->col(3)[0] << ", " << pMat4->col(3)[1] << ", " << pMat4->col(3)[2] << ", " << pMat4->col(3)[3] << " ]\n"; 
    }
    if (node.children.size() > 0) {
        std::cout << _tabstr << "+ ("<< node.children.size() << ") children\n"; 
        std::size_t cnum = 1;
        for (auto& child : node.children) {
            __debug_print_node(asset, asset.nodes[child], _depth + 1);
            cnum++;
        }
    } 
}

const char* getIsolatedFilename(const char* filename) {
    std::string _filenamestr = filename;
    int beginIdx = _filenamestr.rfind('/');
    std::string isofilename = _filenamestr.substr(beginIdx + 1);
    return strdup( isofilename.c_str());
}

void load_gltf(const char * gltf_path, pGltf_data_t _retdata, pViewer viewer, pShaderCache shaders) {
    __init_gltf_datastruct(_retdata, gltf_path);

    std::filesystem::path gltfFilePath = std::string_view { gltf_path };
    static constexpr auto supportedExtensions =
        //fastgltf::Extensions::KHR_draco_mesh_compression |
        //fastgltf::Extensions::EXT_meshopt_compression |
        fastgltf::Extensions::KHR_mesh_quantization |
        fastgltf::Extensions::KHR_texture_transform |
        fastgltf::Extensions::KHR_lights_punctual |
        fastgltf::Extensions::KHR_materials_anisotropy |
        fastgltf::Extensions::KHR_materials_clearcoat |
        fastgltf::Extensions::KHR_materials_dispersion |
        fastgltf::Extensions::KHR_materials_emissive_strength |
        fastgltf::Extensions::KHR_materials_ior |
        fastgltf::Extensions::KHR_materials_iridescence |
        fastgltf::Extensions::KHR_materials_sheen |
        //fastgltf::Extensions::KHR_materials_specular |
        fastgltf::Extensions::KHR_materials_transmission |
        fastgltf::Extensions::KHR_materials_unlit |
        //fastgltf::Extensions::KHR_materials_diffuse_transmission |
        fastgltf::Extensions::KHR_materials_variants;

    fastgltf::Parser parser(supportedExtensions);

    if (load_progress_callback != NULL) { load_progress_callback("Initializing...", "SUBTEST1234", 2.0f, 5.f, 23.0f, 100.f); }

    auto gltfFile = fastgltf::MappedGltfFile::FromPath(gltfFilePath);
    if (!bool(gltfFile)) {
        std::cerr << "Failed to open glTF file: " << gltfFilePath << " -> " << fastgltf::getErrorMessage(gltfFile.error()) << '\n';
        return; }

    constexpr auto gltfOptions =
        fastgltf::Options::DontRequireValidAssetMember |
        fastgltf::Options::AllowDouble |
        fastgltf::Options::LoadExternalBuffers |
        fastgltf::Options::LoadExternalImages |
        fastgltf::Options::GenerateMeshIndices;

    auto __asset = parser.loadGltf(gltfFile.get(), gltfFilePath.parent_path(), gltfOptions);
    if (__asset.error() != fastgltf::Error::None) {
        std::cerr << "Failed to load glTF: " << fastgltf::getErrorMessage(__asset.error()) << '\n';
        return; }

    __init_viewer_struct(viewer);

    _retdata->asset = std::move(__asset.get());

    if (load_progress_callback != NULL) { load_progress_callback("Loading glTF file...", "SUBTEST1234", 2.0f, 5.f, 23.0f, 100.f); }

    // We load images first.
    auto& asset = _retdata->asset;
    int _imageCount     = asset.images.size();
    int _textureCount   = asset.textures.size();
    int _materialCount  = asset.materials.size();
    int _cameraCount    = asset.cameras.size();
    int _nodeCount      = asset.nodes.size();
    int _meshCount      = asset.meshes.size();
    int _sceneCount      = asset.scenes.size();

    _retdata->vertex_count = 0;
    if (_meshCount > 0) {
        _retdata->vertex_count = 0;
        __discover_attributes(_retdata, &asset, asset.meshes.front() );
        for (auto& mesh : asset.meshes) {
            _retdata->vertex_count += get_mesh_total_vertex_count(&asset, mesh);
        } }  

    // Parse the visible node structure to get a world transform matrix for each mesh component 
    // instance per node, and apply that matrix to the min/max of the untransformed mesh, then 
    // grow a bounding volume to include those transformed points

    int _sceneIndex = 0;
    bool _firstVisibleMesh = true;
    fastgltf::iterateSceneNodes(_retdata->asset, _sceneIndex, fastgltf::math::fmat4x4(), [&](fastgltf::Node& node, fastgltf::math::fmat4x4 matrix) {
        if (_firstVisibleMesh) {
            if (node.meshIndex.has_value()) {
                _firstVisibleMesh = false;
                __set_initial_bounds(_retdata, &asset, matrix, asset.meshes[node.meshIndex.value()] ); } } 
        else {
            if (node.meshIndex.has_value()) {
                __grow_bounds_to_include(_retdata, &asset, matrix, asset.meshes[node.meshIndex.value()] ); } } } );

    // ---------
    
    { unsigned int i = 0; for (auto& image : asset.images) { loadImage(shaders, viewer, _retdata, image, i); i++; 
        if (load_progress_callback != NULL) { load_progress_callback("Loading Images", "SUBTEST1234", 2.f + (((float)i / (float)_imageCount ) * 3.0f), 5.f, 23.0f, 100.f); }
    } }
    //for (auto& material : asset.materials)  { loadMaterial(viewer, material); }
    for (auto& mesh : asset.meshes)         { loadMesh(viewer, _retdata, mesh); }
    for (auto& camera : asset.cameras)      { loadCamera(viewer, camera); }      // Loading the cameras (possibly) requires knowing the viewport size, which we get using glfwGetWindowSize above.

    if (load_progress_callback != NULL) { load_progress_callback("Done.", "SUBTEST1234", 5.0, 5.f, 23.0f, 100.f); }

    _retdata->load_success = true;

    // -----------

    std::cout << "[ Opened glTF file: " << gltfFilePath << " ]\n";
    if (_meshCount > 0) {
        std::cout << "   + Meshes: " << _meshCount << "\n";
        for (auto& mesh : asset.meshes) {
            std::cout << "   |   + '" << mesh.name << "' (" << get_mesh_total_vertex_count(&asset, mesh) << " vertices)" << "\n"; 
            auto _ptlbl = "(Unrecognized)";
            if (_retdata->__prim_type == 4) {
                _ptlbl = "Triangles";
            } else if (_retdata->__prim_type == 5)  {
                _ptlbl = "Triangle Strip";
            } else if (_retdata->__prim_type == 6) {
                _ptlbl = "Triangle Fan";
            }
            std::cout << "   |     Type: " << _ptlbl << " ( #" << _retdata->__prim_type << " )\n"; 
        }}
    if (_materialCount > 0) {
        std::cout << "   + Materials: " << _materialCount << "\n";
        for (auto& material : asset.materials) {
            std::cout << "   |   + " << material.name << "\n"; }}
    if (_imageCount > 0) {
        std::cout << "   + Images: " << _imageCount << "\n";
        for (auto& material : asset.images) {
            std::cout << "   |   + " << material.name << "\n"; }}
    if (_textureCount > 0) {
        std::cout << "   + Textures: " << _textureCount << "\n";
        for (auto& texture : asset.textures) {
            if (texture.name != "") { std::cout << "   |   + " << texture.name << "\n"; }}}
    if (_cameraCount > 0) {
        std::cout << "   + Cameras: " << _cameraCount << "\n";
        for (auto& camera : asset.cameras) {
            std::cout << "       + " << camera.name << "\n"; }}
    if (_sceneCount > 0) {
        std::cout << "   + Scenes: " << _sceneCount << "\n";
        std::size_t snum = 1;
        for (auto& scene : asset.scenes) {
            std::cout << "   : #" << snum << " Rootnode Indexs: [ ";
            for (auto& rootnode : scene.nodeIndices) {
                if (snum > 1) {
                    std::cout << ", ";
                }
                std::cout << rootnode ;
            }
            std::cout << " ]\n";
            snum++;
        }
    }

    int _defaultScene = 0;
    if (asset.defaultScene.has_value()) {
        _defaultScene = asset.defaultScene.value();
        std::cout << "     (Default Scene = #" << (_defaultScene + 1)<< ") " << "\n"; }

    if (_nodeCount > 0) {
        std::cout << "   + Nodes: " << _nodeCount << "\n";
        for (auto& node : asset.nodes) {
            std::cout << "   : '" << node.name << "'\n"; }}

    if (_sceneCount > _defaultScene) {
        std::cout << "Visible Heirarchy: \n";
        auto& scene = asset.scenes[_defaultScene];
        for (auto& rootnode : scene.nodeIndices) {
            __debug_print_node(asset, asset.nodes[rootnode], 1); } }

    return;
}

void __finish_opengl_frame(unsigned int _ebo, unsigned int _vbo1, unsigned int _vbo2) {
    
    GL_CALL(glDisable(GL_DEPTH_TEST));

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    GL_CALL(glUseProgram(0));
}

void DestroyDataStructs(pViewer _viewer, pGltf_data_t _data, pShaderCache _shaders){
    _viewer->meshes.erase(_viewer->meshes.begin(), _viewer->meshes.end());_viewer->meshes.clear(); _viewer->meshes.shrink_to_fit();
    _viewer->textures.erase(_viewer->textures.begin(), _viewer->textures.end());_viewer->textures.clear();_viewer->textures.shrink_to_fit();
    _viewer->bufferAllocations.erase(_viewer->bufferAllocations.begin(), _viewer->bufferAllocations.end());_viewer->bufferAllocations.clear();_viewer->bufferAllocations.shrink_to_fit();
    _viewer->cameras.erase(_viewer->cameras.begin(), _viewer->cameras.end());_viewer->cameras.clear();_viewer->cameras.shrink_to_fit();
    _viewer->materialBuffers.erase(_viewer->materialBuffers.begin(), _viewer->materialBuffers.end());_viewer->materialBuffers.clear();_viewer->materialBuffers.shrink_to_fit();
    _viewer->shaderSets.erase(_viewer->shaderSets.begin(), _viewer->shaderSets.end());_viewer->shaderSets.clear();_viewer->shaderSets.shrink_to_fit();
    _viewer->shaderUniforms.erase(_viewer->shaderUniforms.begin(), _viewer->shaderUniforms.end());_viewer->shaderUniforms.clear();_viewer->shaderUniforms.shrink_to_fit();
    
    _clearDefines();
    destroy_ShaderCache(_shaders);
}

void drawMesh(gl_viewer_desc_t view_desc, float temp_elevation, pViewer viewer, unsigned long int visible_index, pGltf_data_t gltf_data, std::size_t meshIndex, fastgltf::math::fmat4x4 matrix, gl_environment_textures _envTex) {
    auto& mesh = viewer->meshes[meshIndex];
    GL_CALL(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mesh.drawsBuffer));

    for (auto i = 0U; i < mesh.primitives.size(); ++i) {
        auto& prim = mesh.primitives[i];
		auto& gltfPrimitive = gltf_data->asset.meshes[meshIndex].primitives[i];

		std::size_t materialIndex;
		auto& mappings = gltfPrimitive.mappings;
		if (!mappings.empty() && mappings[viewer->materialVariant].has_value()) {
			materialIndex = mappings[viewer->materialVariant].value() + 1; // Adjust for default material
		} else {
			materialIndex = prim.materialUniformsIndex;
		}

        auto program = viewer->shaderSets[visible_index].program;
        GL_CALL(glUseProgram(program));

        auto uniforms = viewer->shaderUniforms[visible_index];
    
        GL_CALL(glUniformMatrix4fv(uniforms.modelMatrixUniform, 1, GL_FALSE, &matrix[0][0]));
        make_viewProj_matrix( view_desc, temp_elevation, gltf_data, program );

        GL_CALL(glBindVertexArray(prim.vertexArray));
		GL_CALL(glUniform1f(uniforms.exposure, 1.25f));
        GL_CALL(glBindTextureUnit(0, prim.albedoTexture));
        GL_CALL(glBindTextureUnit(1, prim.metalRoughTexture));
        GL_CALL(glBindTextureUnit(2, prim.occlusionTexture));
        GL_CALL(glBindTextureUnit(3, prim.normalTexture));

        //if (gltfMaterial.diffuseTransmission && gltfMaterial.diffuseTransmission->diffuseTransmissionTexture.has_value()) {
            GL_CALL(glBindTextureUnit(4, prim.diffuseTransmissionTexture));
            GL_CALL(glBindTextureUnit(5, prim.diffuseTransmissionColorTexture));
        //}
        GL_CALL(glBindTextureUnit(6, _envTex.diffuse));
        GL_CALL(glBindTextureUnit(7, _envTex.specular));
        GL_CALL(glBindTextureUnit(8, _envTex.sheen));
        GL_CALL(glBindTextureUnit(9, _envTex.ggxLut));
        GL_CALL(glBindTextureUnit(10, _envTex.charlieLut));
        GL_CALL(glUniform1i(uniforms.envDiffuseSampler, 6));
        GL_CALL(glUniform1i(uniforms.envSpecularSampler, 7));
        GL_CALL(glUniform1i(uniforms.envSheenSampler, 8));
        GL_CALL(glUniform1i(uniforms.envGgxLutSampler, 9));
        GL_CALL(glUniform1i(uniforms.envCharlieLutSampler, 10));

        GL_CALL(glUniform1i(uniforms.envMipCount, (int)_envTex.mipCount));
        set_envrotation_matrix(program);

        auto& gltfMaterial = gltf_data->asset.materials[materialIndex - 1];
        
        //if (viewer->overrideBaseColor){
        //    GL_CALL(glUniform4f(uniforms.baseColorFactor, viewer->overrideBaseColorFactor[0], viewer->overrideBaseColorFactor[1], viewer->overrideBaseColorFactor[2], viewer->overrideBaseColorFactor[3]));
        //} else {
            GL_CALL(glUniform4f(uniforms.baseColorFactor, gltfMaterial.pbrData.baseColorFactor[0], gltfMaterial.pbrData.baseColorFactor[1], gltfMaterial.pbrData.baseColorFactor[2], gltfMaterial.pbrData.baseColorFactor[3]));
        //}
        GL_CALL(glUniform1f(uniforms.roughnessFactor, gltfMaterial.pbrData.roughnessFactor));
        GL_CALL(glUniform1f(uniforms.metallicFactor,  gltfMaterial.pbrData.metallicFactor));
        if (gltfMaterial.pbrData.baseColorTexture.has_value() ) {
            GL_CALL(glUniform1i(uniforms.baseColorSampler, 0));
            GL_CALL(glUniform1i(uniforms.baseColorUVSet, prim.baseColorTexcoordIndex));
            if (gltfMaterial.pbrData.baseColorTexture->transform) {
                auto& transform = gltfMaterial.pbrData.baseColorTexture->transform;
                if (transform) {
                    GL_CALL(glUniformMatrix3fv(uniforms.baseColorUVTransform, 1, GL_FALSE, &(__construct_texture_transform_matrix(*transform)[0][0])));
                }
            }
        }
        if (gltfMaterial.pbrData.metallicRoughnessTexture.has_value() ) {
            GL_CALL(glUniform1i(uniforms.metallicRoughnessSampler, 1));
            GL_CALL(glUniform1i(uniforms.metallicRoughnessUVSet, prim.metallicRoughnessTexcoordIndex));
            if (gltfMaterial.pbrData.metallicRoughnessTexture->transform) {
                auto& transform = gltfMaterial.pbrData.metallicRoughnessTexture->transform;
                if (transform) {
                    GL_CALL(glUniformMatrix3fv(uniforms.metallicRoughnessUVTransform, 1, GL_FALSE, &(__construct_texture_transform_matrix(*transform)[0][0])));
                }
            }
        }
        if (gltfMaterial.occlusionTexture.has_value() ) {
            GL_CALL(glUniform1i(uniforms.occlusionSampler, 2));
            GL_CALL(glUniform1f(uniforms.occlusionStrength, static_cast<float>(gltfMaterial.occlusionTexture.value().strength) ));
            GL_CALL(glUniform1i(uniforms.occlusionUVSet, prim.occlusionTexcoordIndex));
            if (gltfMaterial.occlusionTexture->transform) {
                auto& transform = gltfMaterial.occlusionTexture->transform;
                if (transform) {
                    GL_CALL(glUniformMatrix3fv(uniforms.occlusionUVTransform, 1, GL_FALSE, &(__construct_texture_transform_matrix(*transform)[0][0])));
                }
            }
        }
        if (gltfMaterial.normalTexture.has_value() ) {
            GL_CALL(glUniform1i(uniforms.normalSampler, 3));
            GL_CALL(glUniform1f(uniforms.normalScale, static_cast<float>(gltfMaterial.normalTexture.value().scale) ));
            GL_CALL(glUniform1i(uniforms.normalUVSet, prim.normalTexcoordIndex));
            if (gltfMaterial.normalTexture->transform) {
                auto& transform = gltfMaterial.normalTexture->transform;
                if (transform) {
                    GL_CALL(glUniformMatrix3fv(uniforms.normalUVTransform, 1, GL_FALSE, &(__construct_texture_transform_matrix(*transform)[0][0])));
                }
            }
        }
        if (gltfMaterial.sheen) {
            //std::cout << "*** SHEEN FACTORS : Roughness: " << gltfMaterial.sheen->sheenRoughnessFactor << " : R/G/B " <<  gltfMaterial.sheen->sheenColorFactor[0] << " / " <<  gltfMaterial.sheen->sheenColorFactor[1] << " / " <<  gltfMaterial.sheen->sheenColorFactor[2] << " ***\n";
            GL_CALL(glUniform3f(uniforms.sheenColorFactor, gltfMaterial.sheen->sheenColorFactor[0], gltfMaterial.sheen->sheenColorFactor[1], gltfMaterial.sheen->sheenColorFactor[2] ));
            GL_CALL(glUniform1f(uniforms.sheenRoughnessFactor, static_cast<float>(gltfMaterial.sheen->sheenRoughnessFactor) ));
            if (gltfMaterial.sheen->sheenColorTexture.has_value()) {
                //std::cout << "***MATERIAL HAS SHEEN TEXTURE***\n";
            }
        }
        if (gltfMaterial.specular) {
            //std::cout << "*** SPECULAR FACTORS : specularFactor: " << gltfMaterial.specular->specularFactor << " : R/G/B " <<  gltfMaterial.specular->specularColorFactor[0] << " / " <<  gltfMaterial.specular->specularColorFactor[1] << " / " <<  gltfMaterial.specular->specularColorFactor[2] << " ***\n";
            GL_CALL(glUniform3f(uniforms.specularColorFactor, 
                static_cast<float>(gltfMaterial.specular->specularColorFactor[0]), 
                static_cast<float>(gltfMaterial.specular->specularColorFactor[1]), 
                static_cast<float>(gltfMaterial.specular->specularColorFactor[2]) ) );
            GL_CALL(glUniform1f(uniforms.specularFactor, static_cast<float>(gltfMaterial.specular->specularFactor) ) );
            if (gltfMaterial.specular->specularTexture.has_value()) {
                //std::cout << "***MATERIAL HAS SPECULAR TEXTURE***\n";
            }
            if (gltfMaterial.specular->specularColorTexture.has_value()) {
                //std::cout << "***MATERIAL HAS SPECULAR COLOR TEXTURE***\n";
            }
        }
        
        /* commented out until fastgltf approves one of the diffuse transmission PR's 
        if (gltfMaterial.diffuseTransmission) {
            //std::cout << "*** DIFFUSE TRANSMISSION : factor : " << gltfMaterial.diffuseTransmission->diffuseTransmissionFactor << " : R/G/B " <<  gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor[0] << " / " <<  gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor[1] << " / " <<  gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor[2] << " ***\n";
            GL_CALL(glUniform3f(uniforms.diffuseTransmissionColorFactor, gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor[0], gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor[1], gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor[2] ) );
            GL_CALL(glUniform1f(uniforms.diffuseTransmissionFactor, static_cast<float>(gltfMaterial.diffuseTransmission->diffuseTransmissionFactor) ) );
            if (gltfMaterial.diffuseTransmission->diffuseTransmissionTexture.has_value()) {
                //std::cout << "***MATERIAL HAS DIFFUSE TRANSMISSION TEXTURE***\n";
                GL_CALL(glUniform1i(uniforms.diffuseTransmissionSampler, 4));
                GL_CALL(glUniform1i(uniforms.diffuseTransmissionUVSet, prim.diffuseTransmissionTexcoordIndex));
                if (gltfMaterial.diffuseTransmission->diffuseTransmissionTexture->transform) {
                    auto& transform = gltfMaterial.diffuseTransmission->diffuseTransmissionTexture->transform;
                    if (transform) {
                        GL_CALL(glUniformMatrix3fv(uniforms.diffuseTransmissionUVTransform, 1, GL_FALSE, &(__construct_texture_transform_matrix(*transform)[0][0])));
                    }
                }
            }
            if (gltfMaterial.diffuseTransmission->diffuseTransmissionColorTexture.has_value()) {
                //std::cout << "***MATERIAL HAS DIFFUSE TRANSMISSION COLOR TEXTURE***\n";
                GL_CALL(glUniform1i(uniforms.diffuseTransmissionColorSampler, 5));
                GL_CALL(glUniform1i(uniforms.diffuseTransmissionColorUVSet, prim.diffuseTransmissionColorTexcoordIndex));
                if (gltfMaterial.diffuseTransmission->diffuseTransmissionColorTexture->transform) {
                    auto& transform = gltfMaterial.diffuseTransmission->diffuseTransmissionColorTexture->transform;
                    if (transform) {
                        GL_CALL(glUniformMatrix3fv(uniforms.diffuseTransmissionColorUVTransform, 1, GL_FALSE, &(__construct_texture_transform_matrix(*transform)[0][0])));
                    }
                }                    
            }
        } */

        std::size_t index_count = 0;
        auto& indexAccessor = gltf_data->asset.accessors[gltf_data->asset.meshes[meshIndex].primitives[i].indicesAccessor.value()];
        if (indexAccessor.bufferViewIndex.has_value()) {
            index_count = (uint32_t)indexAccessor.count; }
        GL_CALL(glDrawElements(prim.primitiveType, index_count, prim.indexType, 0));
     }
}

void __getUniformLocations(UniformLocs* uniforms, unsigned int _shader_prog_program) {

    uniforms->modelMatrixUniform = glGetUniformLocation(_shader_prog_program, "u_ModelMatrix");
    uniforms->viewProjectionMatrixUniform = glGetUniformLocation(_shader_prog_program, "u_ViewProjectionMatrix");
    
    uniforms->envDiffuseSampler = glGetUniformLocation(_shader_prog_program, "u_LambertianEnvSampler");
    uniforms->envSpecularSampler = glGetUniformLocation(_shader_prog_program, "u_GGXEnvSampler");
    uniforms->envSheenSampler = glGetUniformLocation(_shader_prog_program, "u_CharlieEnvSampler");
    uniforms->envGgxLutSampler = glGetUniformLocation(_shader_prog_program, "u_GGXLUT");
    uniforms->envCharlieLutSampler = glGetUniformLocation(_shader_prog_program, "u_CharlieLUT");
    uniforms->envMipCount = glGetUniformLocation(_shader_prog_program, "u_MipCount");
    
    uniforms->baseColorFactor = glGetUniformLocation(_shader_prog_program, "u_BaseColorFactor");
    uniforms->baseColorSampler = glGetUniformLocation(_shader_prog_program, "u_BaseColorSampler");
    uniforms->baseColorUVSet = glGetUniformLocation(_shader_prog_program, "u_BaseColorUVSet");
    uniforms->baseColorUVTransform = glGetUniformLocation(_shader_prog_program, "u_BaseColorUVTransform");
    
    uniforms->metallicFactor = glGetUniformLocation(_shader_prog_program, "u_MetallicFactor");
    uniforms->roughnessFactor = glGetUniformLocation(_shader_prog_program, "u_RoughnessFactor");
	uniforms->metallicRoughnessSampler = glGetUniformLocation(_shader_prog_program, "u_MetallicRoughnessSampler");
    uniforms->metallicRoughnessUVSet = glGetUniformLocation(_shader_prog_program, "u_MetallicRoughnessUVSet");
    uniforms->metallicRoughnessUVTransform = glGetUniformLocation(_shader_prog_program, "u_MetallicRoughnessUVTransform");
        
    uniforms->occlusionSampler = glGetUniformLocation(_shader_prog_program, "u_OcclusionSampler");
    uniforms->occlusionStrength = glGetUniformLocation(_shader_prog_program, "u_OcclusionStrength");
    uniforms->occlusionUVSet = glGetUniformLocation(_shader_prog_program, "u_OcclusionUVSet");
    uniforms->occlusionUVTransform = glGetUniformLocation(_shader_prog_program, "u_OcclusionUVTransform");
    
    uniforms->normalSampler = glGetUniformLocation(_shader_prog_program, "u_NormalSampler");
    uniforms->normalScale = glGetUniformLocation(_shader_prog_program, "u_NormalScale");
    uniforms->normalUVSet = glGetUniformLocation(_shader_prog_program, "u_NormalUVSet");
    uniforms->normalUVTransform = glGetUniformLocation(_shader_prog_program, "u_NormalUVTransform");
    
    uniforms->sheenColorFactor = glGetUniformLocation(_shader_prog_program, "u_SheenColorFactor");
    uniforms->sheenRoughnessFactor = glGetUniformLocation(_shader_prog_program, "u_SheenRoughnessFactor");
    uniforms->specularColorFactor = glGetUniformLocation(_shader_prog_program, "u_KHR_materials_specular_specularColorFactor");
    uniforms->specularFactor = glGetUniformLocation(_shader_prog_program, "u_KHR_materials_specular_specularFactor");

    uniforms->diffuseTransmissionFactor = glGetUniformLocation(_shader_prog_program, "u_DiffuseTransmissionFactor");
    uniforms->diffuseTransmissionSampler = glGetUniformLocation(_shader_prog_program, "u_DiffuseTransmissionSampler");
    uniforms->diffuseTransmissionUVSet = glGetUniformLocation(_shader_prog_program, "u_DiffuseTransmissionUVSet");
    uniforms->diffuseTransmissionUVTransform = glGetUniformLocation(_shader_prog_program, "u_DiffuseTransmissionUVTransform");

    uniforms->diffuseTransmissionColorFactor = glGetUniformLocation(_shader_prog_program, "u_DiffuseTransmissionColorFactor");
    uniforms->diffuseTransmissionColorSampler = glGetUniformLocation(_shader_prog_program, "u_DiffuseTransmissionColorSampler");
    uniforms->diffuseTransmissionColorUVSet = glGetUniformLocation(_shader_prog_program, "u_DiffuseTransmissionColorUVSet");
    uniforms->diffuseTransmissionColorUVTransform = glGetUniformLocation(_shader_prog_program, "u_DiffuseTransmissionColorUVTransform");

    uniforms->exposure = glGetUniformLocation(_shader_prog_program, "u_Exposure");
}

unsigned int new_render_gltf_model_to_opengl_texture(gl_viewer_desc_t view_desc, float temp_elevation, pShaderCache shaders, pViewer viewer, pGltf_data_t gltf_data, uint32_t texture_w, uint32_t texture_h, lv_color_t color) {
    uint sceneIndex = 0;
    gl_renwin_state_t _output;
    if (gltf_data->render_state_ready) {
        // OPENGL OUTPUT TEXTURES\n";
        _output = *(gltf_data->render_state);
        if (__restore_opengl_output(_output, texture_w, texture_h) ) {
            // Should drawing this frame be canceled due to GL_INVALID_OPERATION error from possibly conflicting update cycles?
            return _output.texture;
        }
    } else {
        //std::cout << "CREATING OPENGL OUTPUT TEXTURES\n";
        _output = __prepare_opengl_output(texture_w, texture_h);
        gltf_data->render_state = &_output;
        gltf_data->render_state_ready = true;
    }
    {   auto i = 0UL;
        fastgltf::iterateSceneNodes(gltf_data->asset, sceneIndex, fastgltf::math::fmat4x4(), [&](fastgltf::Node& node, fastgltf::math::fmat4x4 matrix) {
            if (node.meshIndex.has_value()) { i++; } } );
        viewer->shaderSets.resize(i);
        viewer->shaderUniforms.resize(i);
        i = 0UL;
        fastgltf::iterateSceneNodes(gltf_data->asset, sceneIndex, fastgltf::math::fmat4x4(), [&](fastgltf::Node& node, fastgltf::math::fmat4x4 matrix) {
            if (node.meshIndex.has_value()) { 
                auto& mesh = gltf_data->asset.meshes[node.meshIndex.value()];
                __discover_defines(gltf_data, mesh);
                viewer->shaderSets[i]  = __compile_and_load_shaders(shaders);
                viewer->shaderUniforms[i] = UniformLocs();
                __getUniformLocations(&(viewer->shaderUniforms[i]), viewer->shaderSets[i].program);
                i++; } } ); }

    //viewer->position = fastgltf::math::fvec3(2.f, 2.f, 2.f);
	//viewer->direction = -viewer->position;

    auto i = 0UL;
    fastgltf::iterateSceneNodes(gltf_data->asset, sceneIndex, fastgltf::math::fmat4x4(), [&](fastgltf::Node& node, fastgltf::math::fmat4x4 matrix) {
        if (node.meshIndex.has_value()) {
            drawMesh(view_desc, temp_elevation, viewer, i, gltf_data, *node.meshIndex, matrix, *(shaders->lastEnv)); i++;} } );

    __finish_opengl_frame(gltf_data->ebo, gltf_data->vbo1, gltf_data->vbo2);

    return _output.texture;
}
