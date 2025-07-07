/**
  @file lv_opengl_shader_internal.h
 *
 */

#ifndef LV_GL_SHADER_INTERNAL_H
#define LV_GL_SHADER_INTERNAL_H

#include "lv_gl_shader.h"
#include <drivers/glfw/lv_opengles_debug.h>
#include <stdlib/lv_mem.h>
#include <misc/lv_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

typedef struct {
    lv_gl_shader_t data;
    bool src_allocated;
} lv_gl_shader_source_t;

typedef struct {
    uint32_t hash;
    GLuint id;
} lv_gl_compiled_shader_t;

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGL_SHADER_INTERNAL_H*/
