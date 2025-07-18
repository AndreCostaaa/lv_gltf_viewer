/**
 * @file lv_opengl_shader.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gl_shader_manager.h"
#include "lv_gl_shader_internal.h"
#include "lv_gl_shader_manager_internal.h"
#include "lv_gl_shader_program_internal.h"
#include "lv_gl_shader_program.h"
#include "misc/lv_assert.h"
#include "misc/lv_log.h"
#include "misc/lv_rb.h"
#include "misc/lv_types.h"
#include "stdlib/lv_mem.h"
#include <lvgl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <drivers/glfw/lv_opengles_debug.h> /* GL_CALL */
#include "lv_gl_shader.h"

#include <src/misc/lv_rb_private.h>
#include <stdlib.h>
#include <stdlib/lv_string.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_gl_compiled_shader_t lv_gl_shader_texture_t;

typedef struct {
    lv_gl_shader_program_t * program;
    uint32_t vertex_shader_hash;
    uint32_t fragment_shader_hash;
} lv_gl_program_map_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_rb_compare_res_t
shader_source_compare_cb(const lv_gl_shader_source_t * lhs,
                         const lv_gl_shader_source_t * rhs);

static lv_rb_compare_res_t
compiled_shader_compare_cb(const lv_gl_compiled_shader_t * lhs,
                           const lv_gl_compiled_shader_t * rhs);

static lv_rb_compare_res_t
shader_program_compare_cb(const lv_gl_program_map_key_t * lhs,
                          const lv_gl_program_map_key_t * rhs);

static lv_rb_t create_shader_map(const lv_gl_shader_t * shaders, size_t len);
static bool string_ends_with(const char * value, const char * suffix);

static char * construct_shader(const char * source,
                               const lv_gl_shader_t * permutations,
                               size_t permutations_len);

static GLuint compile_shader(const char * shader_source, bool is_vertex_shader);
static GLuint link_program(GLuint vertex_shader_id, GLuint fragment_shader_id);
static void destroy_environment(lv_gl_shader_manager_env_textures_t * env);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_gl_shader_manager_t * lv_gl_shader_manager_create(lv_gl_shader_t * sources,
                                                     size_t len, char * vert_src,
                                                     char * frag_src)
{
    lv_gl_shader_manager_t * shader =
        (lv_gl_shader_manager_t *)lv_malloc_zeroed(sizeof(*shader));
    LV_ASSERT_MALLOC(shader);
    if(!shader) {
        return NULL;
    }

    shader->last_env = NULL;
    shader->sources_map = create_shader_map(sources, len);
    if(vert_src != NULL) {
        lv_gl_shader_t entry = { "__MAIN__.vert", strdup(vert_src) };
        lv_rb_node_t * node = lv_rb_insert(&shader->sources_map, &entry);
        LV_ASSERT_MSG(node,
                      "Failed to insert shader source to source map");
        lv_memcpy(node->data, &entry, sizeof(entry));
        lv_free(vert_src);
    }
    if(frag_src != NULL) {
        lv_gl_shader_t entry = { "__MAIN__.frag", strdup(frag_src) };
        lv_rb_node_t * node = lv_rb_insert(&shader->sources_map, &entry);
        LV_ASSERT_MSG(node, "Failed to insert shader to shader map");
        lv_memcpy(node->data, &entry, sizeof(entry));
        lv_free(frag_src);
    }

    lv_rb_init(&shader->compiled_shaders_map,
               (lv_rb_compare_t)compiled_shader_compare_cb,
               sizeof(lv_gl_compiled_shader_t));

    /* Textures and compiled shaders share the same compare function */
    lv_rb_init(&shader->textures_map,
               (lv_rb_compare_t)compiled_shader_compare_cb,
               sizeof(lv_gl_shader_texture_t));

    lv_rb_init(&shader->programs_map,
               (lv_rb_compare_t)shader_program_compare_cb,
               sizeof(lv_gl_program_map_key_t));

    shader->bg_index_buf = 0;
    shader->bg_vertex_buf = 0;
    shader->bg_program = 0;
    return shader;
}

uint32_t lv_gl_shader_hash(const char * value)
{
    uint32_t hash = 0;
    const size_t len = lv_strlen(value);
    if(len == 0) {
        return hash;
    }
    for(size_t i = 0; i < len; i++) {
        hash = ((hash << 5) - hash) + value[i];
    }
    return hash;
}

void lv_gl_shader_manager_store_texture(lv_gl_shader_manager_t * manager,
                                        uint32_t texture_hash,
                                        GLuint texture_id)
{
    lv_gl_shader_texture_t key = { .id = texture_id, .hash = texture_hash };
    lv_rb_node_t * node = lv_rb_insert(&manager->textures_map, &key);
    if(!node) {
        LV_LOG_WARN("Failed to cache texture hash: %d id: %d",
                    texture_hash, texture_id);
        return;
    }

    lv_memcpy(node->data, &key, sizeof(key));
}

GLuint lv_gl_shader_manager_get_texture(lv_gl_shader_manager_t * manager,
                                        uint32_t texture_hash)
{
    lv_gl_shader_texture_t key = { .hash = texture_hash };
    lv_rb_node_t * node = lv_rb_find(&manager->textures_map, &key);
    if(!node) {
        LV_LOG_WARN("Couldn't find texture with hash %d in cache",
                    texture_hash);
        return GL_NONE;
    }
    return ((lv_gl_shader_texture_t *)node->data)->id;
}

uint32_t lv_gl_shader_manager_select_shader(lv_gl_shader_manager_t * shader,
                                            const char * shader_identifier,
                                            lv_gl_shader_t * permutations,
                                            size_t permutations_len)
{
    /* First check that the shader identifier exists */
    lv_gl_shader_t key = { shader_identifier, NULL };
    lv_rb_node_t * source_node = lv_rb_find(&shader->sources_map, &key);

    if(!source_node) {
        LV_LOG_WARN("Couldn't find shader %s", shader_identifier);
        return 0;
    }

    const char * original_shader_source =
        ((lv_gl_shader_source_t *)source_node->data)->data.source;
    uint32_t hash = lv_gl_shader_hash(shader_identifier);

    char * shader_source = construct_shader(original_shader_source,
                                            permutations, permutations_len);

    hash ^= lv_gl_shader_hash(shader_source);

    lv_gl_compiled_shader_t shader_map_key = { hash, 0 };
    lv_rb_node_t * shader_map_node =
        lv_rb_find(&shader->compiled_shaders_map, &shader_map_key);

    if(shader_map_node != NULL) {
        lv_free(shader_source);
        return hash;
    }

    shader_map_key.id = compile_shader(
                            shader_source, string_ends_with(shader_identifier, ".vert"));
    lv_rb_node_t * node =
        lv_rb_insert(&shader->compiled_shaders_map, &shader_map_key);
    LV_ASSERT_MSG(node, "Failed to insert shader to shader map");
    lv_memcpy(node->data, &shader_map_key, sizeof(shader_map_key));

    lv_free(shader_source);

    return hash;
}

lv_gl_shader_program_t *
lv_gl_shader_manager_get_program(lv_gl_shader_manager_t * manager,
                                 uint32_t fragment_shader_hash,
                                 uint32_t vertex_shader_hash)
{
    lv_gl_program_map_key_t key = {
        .vertex_shader_hash = vertex_shader_hash,
        .fragment_shader_hash = fragment_shader_hash
    };

    lv_rb_node_t * node = lv_rb_find(&manager->programs_map, &key);
    if(node) {
        return ((lv_gl_program_map_key_t *)node->data)->program;
    }

    lv_gl_compiled_shader_t shader_key = { .hash = vertex_shader_hash };
    lv_rb_node_t * vertex_node =
        lv_rb_find(&manager->compiled_shaders_map, &shader_key);
    shader_key.hash = fragment_shader_hash;
    lv_rb_node_t * fragment_node =
        lv_rb_find(&manager->compiled_shaders_map, &shader_key);

    LV_ASSERT_FORMAT_MSG(
        vertex_node,
        "Unable to find to find vertex shader with hash %d",
        vertex_shader_hash);
    LV_ASSERT_FORMAT_MSG(
        fragment_node,
        "Unable to find to find fragment shader with hash %d",
        fragment_shader_hash);

    const GLuint vertex_shader_id =
        ((lv_gl_compiled_shader_t *)vertex_node->data)->id;
    const GLuint fragment_shader_id =
        ((lv_gl_compiled_shader_t *)fragment_node->data)->id;
    GLuint program_id = link_program(vertex_shader_id, fragment_shader_id);

    lv_gl_shader_program_t * program =
        lv_gl_shader_program_create(program_id);

    LV_ASSERT_MSG(program, "Failed to create program");

    lv_gl_program_map_key_t prog_key = {
        .program = program,
        .fragment_shader_hash = fragment_shader_hash,
        .vertex_shader_hash = vertex_shader_hash
    };
    node = lv_rb_insert(&manager->programs_map, &prog_key);
    LV_ASSERT_MSG(node, "Failed to store program in cache");
    lv_memcpy(node->data, &prog_key, sizeof(prog_key));
    return program;
}

void lv_gl_shader_manager_destroy(lv_gl_shader_manager_t * manager)
{
    LV_LOG_INFO("Destroying shader cache");

    lv_rb_destroy(&manager->textures_map);

    lv_rb_node_t * node;
    while((node = manager->compiled_shaders_map.root)) {
        lv_gl_compiled_shader_t * shader = node->data;
        GL_CALL(glDeleteShader(shader->id));
        lv_rb_remove_node(&manager->compiled_shaders_map, node);
    }
    while((node = manager->programs_map.root)) {
        lv_gl_program_map_key_t * program_key = node->data;
        lv_gl_shader_program_destroy(program_key->program);
        lv_rb_remove_node(&manager->programs_map, node);
    }
    lv_rb_destroy(&manager->programs_map);

    if(manager->last_env && (manager->last_env->loaded)) {
        destroy_environment(manager->last_env);
    }
    manager->last_env = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_rb_compare_res_t
shader_program_compare_cb(const lv_gl_program_map_key_t * lhs,
                          const lv_gl_program_map_key_t * rhs)
{
    const uint32_t cmp = lhs->vertex_shader_hash == rhs->vertex_shader_hash;
    if(cmp == 0) {
        return lhs->fragment_shader_hash - rhs->fragment_shader_hash;
    }
    return cmp;
}

static lv_rb_compare_res_t
shader_source_compare_cb(const lv_gl_shader_source_t * lhs,
                         const lv_gl_shader_source_t * rhs)
{
    return lv_gl_shader_hash(lhs->data.name) -
           lv_gl_shader_hash(rhs->data.name);
}

static lv_rb_compare_res_t
compiled_shader_compare_cb(const lv_gl_compiled_shader_t * lhs,
                           const lv_gl_compiled_shader_t * rhs)
{
    return lhs->hash - rhs->hash;
}
static GLuint compile_shader(const char * shader_source, bool is_vertex_shader)
{
    GLuint shader_id;
    if(is_vertex_shader) {
        GL_CALL(shader_id = glCreateShader(GL_VERTEX_SHADER));
    }
    else {
        GL_CALL(shader_id = glCreateShader(GL_FRAGMENT_SHADER));
    }

    GL_CALL(glShaderSource(shader_id, 1, (const char **)&shader_source,
                           NULL));
    GL_CALL(glCompileShader(shader_id));

    int shader_compiled;
    GL_CALL(glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compiled));
    if(!shader_compiled) {
        GLchar info_log[512];
        GL_CALL(glGetShaderInfoLog(shader_id, sizeof(info_log), NULL,
                                   info_log));
        LV_LOG_ERROR("Failed to compile shader: %s", info_log);
        LV_ASSERT_MSG(shader_compiled, "Couldn't compile shader");
    }
    return shader_id;
}

static GLuint link_program(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    GLuint program_id;
    GL_CALL(program_id = glCreateProgram());
    GL_CALL(glAttachShader(program_id, vertex_shader_id));
    GL_CALL(glAttachShader(program_id, fragment_shader_id));
    GL_CALL(glLinkProgram(program_id));

    int link_status;
    GL_CALL(glGetProgramiv(program_id, GL_LINK_STATUS, &link_status));
    if(!link_status) {
        GLchar info_log[512];
        GL_CALL(glGetProgramInfoLog(program_id, sizeof(info_log), NULL,
                                    info_log));
        LV_LOG_ERROR("GLSL LINK ERROR: %s", info_log);
        LV_ASSERT_MSG(link_status, "Couldn't link program");
    }
    return program_id;
}

static char * append_to_shader(char * dst, const char * src, size_t * curr_index)
{
    lv_strcpy(dst + *curr_index, src);
    *curr_index += strlen(src);
    return dst;
}

static char * construct_shader(const char * source,
                               const lv_gl_shader_t * permutations,
                               size_t permutations_len)
{
    const char * defines = "#version 300 es\n";
    const char * prefix = "#define ";
    const size_t prefix_len = strlen(prefix);
    size_t shader_source_size = strlen(defines) + strlen(source);

    /* First calculate the necessary size */
    for(size_t i = 0; i < permutations_len; ++i) {
        shader_source_size += prefix_len;
        if(!permutations[i].name) {
            LV_LOG_WARN("Name is NULL for permutation # %zu", i);
            continue;
        }
        shader_source_size +=
            strlen(permutations[i].name) + 1; /* ' ' */
        if(permutations[i].source) {
            shader_source_size +=
                strlen(permutations[i].source) + 1; /* '\n' */
        }
    }

    /* Allocate enough for memory with calculated size*/
    char * result = (char *)lv_malloc(shader_source_size + 1);
    if(!result) {
        LV_LOG_ERROR(
            "Failed to allocate enough space for shader changes");
        return 0;
    }

    /* Construct shader */
    size_t curr_index = 0;
    append_to_shader(result, defines, &curr_index);
    for(size_t i = 0; i < permutations_len; ++i) {
        if(!permutations[i].name) {
            continue;
        }
        append_to_shader(result, prefix, &curr_index);
        append_to_shader(result, permutations[i].name, &curr_index);
        if(permutations[i].source) {
            result[curr_index++] = ' ';
            append_to_shader(result, permutations[i].source,
                             &curr_index);
        }
        result[curr_index++] = '\n';
    }
    append_to_shader(result, source, &curr_index);
    LV_ASSERT(curr_index == shader_source_size);
    result[shader_source_size] = '\0';
    return result;
}

static char * replace_include(const char * source, const char * pattern,
                              const char * replacement)
{
    const char * pos = strstr(source, pattern);
    LV_ASSERT(pos);

    const size_t source_len = lv_strlen(source);
    const size_t pattern_len = lv_strlen(pattern);
    const size_t replacement_len = lv_strlen(replacement);

    const size_t new_len = source_len - pattern_len + replacement_len;

    char * result = (char *)lv_malloc(new_len + 1);
    if(!result) {
        return NULL;
    }

    const size_t before_len = pos - source;
    lv_memcpy(result, source, before_len);
    lv_memcpy(result + before_len, replacement, replacement_len);
    lv_strcpy(result + before_len + replacement_len, pos + pattern_len);

    /* Replace other patterns with whitespaces */
    while((pos = strstr(result, pattern))) {
        lv_memset((void *)pos, ' ', pattern_len);
    }

    return result;
}
static lv_rb_t create_shader_map(const lv_gl_shader_t * shaders, size_t len)
{
    lv_rb_t map;
    lv_rb_init(&map, (lv_rb_compare_t)shader_source_compare_cb,
               sizeof(lv_gl_shader_source_t));

    char pattern[256];
    for(size_t i = 0; i < len; i++) {
        if(strlen(shaders[i].source) == 0) {
            LV_LOG_WARN("Shader %s at index %zu is empty\n",
                        shaders[i].name, i);
            continue;
        }
        lv_gl_shader_source_t value = { {
                .name = shaders[i].name,
                .source = shaders[i].source
            },
            false
        };

        for(size_t j = 0; j < len; j++) {
            const char * source = value.data.source;
            const char * include_name = shaders[j].name;
            const char * include_source = shaders[j].source;
            const size_t include_name_len = strlen(include_name);
            const size_t include_source_len =
                strlen(include_source);
            if(include_name_len == 0 || include_source_len == 0) {
                continue;
            }
            lv_snprintf(pattern, sizeof(pattern), "#include <%s>",
                        include_name);
            const char * include_pattern = strstr(source, pattern);
            if(include_pattern == NULL) {
                continue;
            }
            LV_LOG_TRACE("Replacing %s", pattern);
            char * new_source = replace_include(source, pattern,
                                                include_source);
            LV_ASSERT_MSG(
                new_source,
                "Failed to allocate memory to replace shader include with source code");

            value.data.source = new_source;
            value.src_allocated = true;
        }
        if(strstr(value.data.source, "#include")) {
            LV_LOG_ERROR(
                "Couldn't replace every include in shader %s. Last result:\n%s",
                value.data.name, value.data.source);
            LV_ASSERT_MSG(
                0,
                "Refusing to continue execution with incorrect shaders");
        }

        lv_rb_node_t * node = lv_rb_insert(&map, &value);
        LV_ASSERT_MSG(node,
                      "Failed to allocate memory for shader map entry");
        lv_memcpy(node->data, &value, sizeof(value));
    }
    return map;
}

static bool string_ends_with(const char * value, const char * suffix)
{
    const size_t value_len = strlen(value);
    const size_t suffix_len = strlen(suffix);

    if(value_len < suffix_len) {
        return false;
    }

    return lv_streq(value + value_len - suffix_len, suffix);
}

static void destroy_environment(lv_gl_shader_manager_env_textures_t * env)
{
    LV_LOG_INFO("Destroying shader env resources");
    const unsigned int d[3] = { env->diffuse, env->specular,
                                env->sheen
                              };
    GL_CALL(glDeleteTextures(3, d));
}
