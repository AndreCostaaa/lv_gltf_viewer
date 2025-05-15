#ifndef GL_RENWIN_H
#define GL_RENWIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lib/mathc/mathc.h"
#include "gl_shaderdef_v1.h"
#include "shader_cache.h"



struct _Viewer;
typedef struct _Viewer *pViewer;

struct gltf_data_t;
typedef struct gltf_data_t *pGltf_data_t;

typedef struct {
    bool init_success;
    unsigned int texture;
    unsigned int renderbuffer;
    unsigned framebuffer;
} gl_renwin_state_t;

typedef struct {
    unsigned int program;
    unsigned int vert;
    unsigned int frag;
} gl_renwin_shaderset_t;

typedef struct {
    float pitch;
    float yaw;
    float distance;
//    float elevation;  For reasons I do not understand, uncommenting this line causes critical failures elsewhere in the OpenGL pipeline, and repeat renders will flood GL_INVALID_OPERATION errors.  I don't know why this is, the errors don't appear to originate from this code but they are somehow influenced by it.  Passing this as a parameter until I figure that out.
} gl_viewer_desc_t;


unsigned int get_gltf_datastruct_datasize(void);
unsigned int get_viewer_datasize(void);

void load_gltf(const char * gltf_path, pGltf_data_t _retdata, pViewer viewer, pShaderCache shaders);

unsigned int new_render_gltf_model_to_opengl_texture(gl_viewer_desc_t view_desc, float temp_elevation, pShaderCache shaders, pViewer viewer, pGltf_data_t gltf_data, uint32_t texture_w, uint32_t texture_h, lv_color_t color);

void DestroyDataStructs(pViewer _viewer, pGltf_data_t _data, pShaderCache _shaders);
void setViewerPitchYawDistance(pViewer viewer, float _viewPitch, float _viewYaw, float _viewDistance);
bool setLoadPhaseCallback(void (*_load_progress_callback)(const char*, const char* , float, float, float, float));
const char* getIsolatedFilename(const char* filename);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*GL_RENWIN_H*/
