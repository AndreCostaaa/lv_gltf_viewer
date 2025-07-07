/**
 * @file lv_opengl_shader_program.h
 *
 */

#ifndef LV_OPENGL_SHADER_PROGRAM_H
#define LV_OPENGL_SHADER_PROGRAM_H

#include "drivers/glfw/lv_opengles_debug.h"
#include "lv_gl_shader_program_internal.h"
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

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_gl_shader_program_t * lv_gl_shader_program_create(uint32_t id);
void lv_gl_shader_program_destroy(lv_gl_shader_program_t * program);
GLuint lv_gl_shader_program_get_id(lv_gl_shader_program_t * program);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGL_SHADER_PROGRAM_H*/
