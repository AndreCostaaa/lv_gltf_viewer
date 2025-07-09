/**
 * @file lv_opengl_shader_manager_internal.h
 *
 */

#ifndef LV_GL_SHADER_MANAGER_INTERNAL_H
#define LV_GL_SHADER_MANAGER_INTERNAL_H

#include "lv_gl_shader_program_internal.h"
#include "lv_gl_shader.h"
#include "misc/lv_rb_private.h"
#include "lv_gl_shader_program_internal.h"
#include "misc/lv_types.h"
#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t diffuse;
    uint32_t specular;
    uint32_t sheen;
    uint32_t ggxLut;
    uint32_t charlieLut;
    uint32_t mipCount;
    float iblIntensityScale;
    float angle;
    bool loaded;
} lv_gl_shader_manager_env_textures_t;

typedef struct lv_opengl_shader_cache_struct {
    lv_rb_t sources_map;
    lv_rb_t textures_map;
    lv_rb_t compiled_shaders_map;
    lv_rb_t programs_map;
    GLuint bg_index_buf;
    GLuint bg_vertex_buf;
    GLuint bg_program;
    GLuint bg_vao;
    /* The last displayed environment, it gets reused if not null and loaded. */
    lv_gl_shader_manager_env_textures_t * last_env;
} lv_gl_shader_manager_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GL_SHADER_INTERNAL_H*/
