/**
 * @file lv_templ.h
 *
 */

#ifndef LV_GL_SHADER_MANAGER_H
#define LV_GL_SHADER_MANAGER_H

#include "drivers/glfw/lv_opengles_debug.h"
#include "lv_gl_shader_program_internal.h"
#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_gl_shader_manager_internal.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_gl_shader_manager_t * lv_gl_shader_manager_create(lv_gl_shader_t * shaders,
                                                     size_t shaders_count,
                                                     char * _vertSrc,
                                                     char * _fragSrc);
void lv_gl_shader_manager_destroy(lv_gl_shader_manager_t * manager);
uint32_t lv_gl_shader_hash(const char * value);

GLuint lv_gl_shader_manager_get_texture(lv_gl_shader_manager_t * manager,
                                        uint32_t hash);
void lv_gl_shader_manager_store_texture(lv_gl_shader_manager_t * manager,

                                        uint32_t hash, GLuint id);
uint32_t lv_gl_shader_manager_select_shader(lv_gl_shader_manager_t * shader,
                                            const char * shader_identifier,
                                            lv_gl_shader_t * permutations,
                                            size_t permutations_len);
lv_gl_shader_program_t *
lv_gl_shader_manager_get_program(lv_gl_shader_manager_t * manager,
                                 uint32_t fragment_shader_hash,
                                 uint32_t vertex_shader_hash);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GL_SHADER_MANAGER_H*/
