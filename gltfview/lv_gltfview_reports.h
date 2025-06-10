#ifndef LV_GLTF_REPORTS_H
#define LV_GLTF_REPORTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a summary of the mesh data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_make_mesh_summary(pGltf_data_t data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the material data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_make_material_summary(pGltf_data_t data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the scenes in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_make_scenes_summary(pGltf_data_t data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the images in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_make_images_summary(pGltf_data_t data, char *dest_buffer, uint32_t dest_buffer_size);

/**
 * @brief Create a summary of the animations in the GLTF data.
 *
 * @param data Pointer to the GLTF data.
 * @param dest_buffer Buffer to store the summary.
 * @param dest_buffer_size Size of the destination buffer.
 */
void lv_gltf_make_animations_summary(pGltf_data_t data, char *dest_buffer, uint32_t dest_buffer_size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_GLTF_REPORTS_H */


