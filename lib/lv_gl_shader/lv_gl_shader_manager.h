/**
 * @file lv_templ.h
 *
 */

#ifndef LV_GL_SHADER_MANAGER_H
#define LV_GL_SHADER_MANAGER_H

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

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GL_SHADER_MANAGER_H*/
