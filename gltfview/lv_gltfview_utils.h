#ifndef LV_GLTF_UTILS_H
#define LV_GLTF_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lib/mathc/mathc.h"

/**
 * @brief Push the current OpenGL state onto the stack.
 */
inline void lv_gltf_opengl_state_push(void);

/**
 * @brief Pop the last OpenGL state from the stack.
 */
inline void lv_gltf_opengl_state_pop(void);

/**
 * @brief Get the center point of a primitive in the mesh.
 *
 * @param ret_data Pointer to the GLTF data.
 * @param mesh Reference to the mesh.
 * @param prim_num The index of the primitive.
 * @return The center point of the primitive as a fvec4.
 */
FVEC4 lv_gltf_get_primitive_centerpoint(pGltf_data_t ret_data, fastgltf::Mesh& mesh, uint32_t prim_num);

/**
 * @brief Get the isolated filename from a full path.
 *
 * @param filename The full filename.
 * @param out_buffer Buffer to store the isolated filename.
 * @param max_out_length Maximum length of the output buffer.
 */
void lv_gltf_get_isolated_filename(const char* filename, char* out_buffer, uint32_t max_out_length);

/**
 * @brief Print a summary of the transformation matrix.
 *
 * @param matrix The transformation matrix to print.
 */
void lv_gltf_print_matrix_summary(FMAT4 matrix);

/**
 * @brief Debug print the details of a node in the asset.
 *
 * @param asset Reference to the GLTF asset.
 * @param node The node to print.
 * @param depth The depth in the hierarchy for indentation.
 */
void lv_gltf_debug_print_node(ASSET& asset, fastgltf::Node node, std::size_t depth);

/**
 * @brief Raycast to find the ground position based on mouse coordinates.
 *
 * @param viewer Pointer to the viewer.
 * @param mouse_x The X coordinate of the mouse.
 * @param mouse_y The Y coordinate of the mouse.
 * @param win_width The width of the window.
 * @param win_height The height of the window.
 * @param ground_height The height of the ground.
 * @param out_pos Pointer to store the output position.
 * @return true if the raycast was successful, false otherwise.
 */
bool lv_gltfview_raycast_ground_position(pViewer viewer, int32_t mouse_x, int32_t mouse_y, int32_t win_width, int32_t win_height, double ground_height, float* out_pos);

/**
 * @brief Copy the viewer descriptor from one state to another.
 *
 * @param from_state Pointer to the source viewer descriptor.
 * @param to_state Pointer to the destination viewer descriptor.
 */
void lv_gltf_copy_viewer_desc(gl_viewer_desc_t* from_state, gl_viewer_desc_t* to_state);

/**
 * @brief Compare two viewer descriptors for equality.
 *
 * @param from_state Pointer to the first viewer descriptor.
 * @param to_state Pointer to the second viewer descriptor.
 * @return true if the descriptors are equal, false otherwise.
 */
bool lv_gltf_compare_viewer_desc(gl_viewer_desc_t* from_state, gl_viewer_desc_t* to_state);


FVEC3 lv_gltf_get_centerpoint(pGltf_data_t gltf_data, FMAT4 matrix, uint32_t meshIndex, int32_t elem);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_GLTF_UTILS_H */
