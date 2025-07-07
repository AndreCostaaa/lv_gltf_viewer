/**
  @file lv_opengl_shader.h
 *
 */

#ifndef LV_GL_SHADER_H
#define LV_GL_SHADER_H

#include <misc/lv_types.h>
#include <drivers/glfw/lv_opengles_debug.h>
#include <stdlib/lv_mem.h>

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
    const char * name;
    const char * source;
} lv_gl_shader_t;

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

#endif /*LV_GL_SHADER_H*/
