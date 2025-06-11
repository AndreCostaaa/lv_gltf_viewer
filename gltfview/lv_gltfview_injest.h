#ifndef LV_GLTF_INJEST_H
#define LV_GLTF_INJEST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load the gltf file at the specified filepath
 *
 * @param gltf_path The gltf filename
 * @param ret_data Pointer to the data container that will be populated.
 * @param viewer Pointer to the viewer object this file will be displayed within.
 * @param shaders Pointer to the shader cache object this file uses.
 */
void lv_gltfview_load(const char * gltf_path, pGltf_data_t ret_data, pViewer viewer, pShaderCache shaders);

/**
 * @brief Set the callback function that is called when loading increments.
 *
 * @param load_progress_callback The callback function with these parameters (const char*, const char* , float, float, float, float) and no return value.
 */
bool lv_gltfview_set_loadphase_callback(void (*load_progress_callback)(const char*, const char* , float, float, float, float));

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTFINJEST_H*/
