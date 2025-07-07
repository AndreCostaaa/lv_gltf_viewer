/**
 * @file lv_opengl_shader_program_internal.h
 *
 */

#ifndef LV_OPENGL_SHADER_PROGRAM_INTERNAL_H
#define LV_OPENGL_SHADER_PROGRAM_INTERNAL_H

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

/**********************
 *      TYPEDEFS
 **********************/

struct lv_shader_program_struct;

typedef void (*lv_gl_shader_program_update_uniformi_t)(
    struct lv_shader_program_struct *, const char *, int);

typedef void (*lv_gl_shader_program_update_uniformf_t)(
    struct lv_shader_program_struct *, const char *, float);

typedef struct lv_shader_program_struct {
    lv_gl_shader_program_update_uniformi_t update_uniform_1i;
    lv_gl_shader_program_update_uniformf_t update_uniform_1f;
    uint32_t id;
} lv_gl_shader_program_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGL_SHADER_PROGRAM_INTERNAL_H*/
