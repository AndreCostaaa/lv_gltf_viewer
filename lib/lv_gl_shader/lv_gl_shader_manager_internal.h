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
    bool loaded;
    unsigned int diffuse;
    unsigned int specular;
    unsigned int sheen;
    unsigned int ggxLut;
    unsigned int charlieLut;
    unsigned int mipCount;
    float iblIntensityScale;
    float angle;
} lv_gl_shader_manager_env_textures_t;

typedef struct lv_opengl_shader_cache_struct {
    lv_rb_t sources_map;
    lv_rb_t textures_map;
    lv_rb_t compiled_shaders_map;
    lv_rb_t programs_map;

    uint32_t (*select_shader)(struct lv_opengl_shader_cache_struct *,
                              const char *, lv_gl_shader_t *, size_t);

    lv_gl_shader_program_t * (*get_shader_program)(
        struct lv_opengl_shader_cache_struct *, uint32_t, uint32_t);

    void (*set_texture_cache_item)(struct lv_opengl_shader_cache_struct *,
                                   uint32_t, GLuint);

    GLuint(*getCachedTexture)(struct lv_opengl_shader_cache_struct *,
                              uint32_t);

    unsigned int bg_index_buf;
    unsigned int bg_vertex_buf;
    unsigned int bg_program;
    unsigned int bg_vao;
    /* The last displayed environment, it gets reused if not null and loaded. */
    lv_gl_shader_manager_env_textures_t *
    last_env;
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
