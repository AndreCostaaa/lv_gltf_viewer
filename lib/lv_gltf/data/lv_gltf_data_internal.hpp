#ifndef LV_GLTFDATA_HPP
#define LV_GLTFDATA_HPP

#include "fastgltf/math.hpp"
#include "lv_gltf_data.h"
#include "lv_gltf_data_internal.h"
#include "lv_gltf_bind.h"

#include "misc/lv_array.h"
#include <GL/gl.h>

#ifdef __cplusplus

#include <string>
#include <vector>
#include <map>
#include <fastgltf/types.hpp>

// Vector of int32_t's
using UintVector = std::vector<uint32_t>;
// Vector of int32_t's
using IntVector = std::vector<int32_t>;
// Vector of int64_t's
using LongVector = std::vector<int64_t>;
// Pointer to fastgltf::Node
using NodePtr = fastgltf::Node *;
// A standard 4x4 transform matrix
using Transform = fastgltf::math::fmat4x4;
// Pair of Node pointer and int32_t
using NodeIndexPair = std::pair<NodePtr, size_t>;
// Pair of float and Node/Index pair
using NodeIndexDistancePair = std::pair<float, NodeIndexPair>;
// Vector of NodeIndexPair
using NodePairVector = std::vector<NodeIndexPair>;
// Vector of NodeIndexDistancePair
using NodeDistanceVector = std::vector<NodeIndexDistancePair>;
// Map of uint32_t to NodePairVector
using MaterialIndexMap = std::map<uint32_t, NodePairVector>;
// Map of Node Pointers to Transforms
using NodeTransformMap = std::map<NodePtr, Transform>;
// Map of 4x4 Transform Maps by int32_t index (skin)
using MapofTransformMap = std::map<int32_t, NodeTransformMap>;
// Map of Nodes by string (name)
using StringNodeMap = std::map<std::string, NodePtr>;
// Map of Nodes by string (name)
using NodeIntMap = std::map<NodePtr, uint32_t>;
// Map of Nodes by string (name)
using NodeVector = std::vector<NodePtr>;
// Map of Node Index to Map of Prim Index to CenterXYZ+RadiusW Vec4
using NodePrimCenterMap =
	std::map<uint32_t, std::map<uint32_t, fastgltf::math::fvec4> >;
// Map of Overrides by Node
using NodeOverrideMap = std::map<fastgltf::Node*, lv_gltf_bind_t *>;
// Map of Overrides by Node
using OverrideVector = std::vector<lv_gltf_bind_t>;

typedef struct {
	GLuint drawsBuffer;
	std::vector<lv_gltf_primitive_t> primitives;
} lv_gltf_mesh_data_t;

typedef struct {
	const char* ip;
	const char* path;
	fastgltf::Node* node;
}lv_gltf_data_node_t;


struct lv_gltf_data_struct {
	const char *filename;
	fastgltf::Asset asset;
	lv_array_t nodes;
	NodeVector node_by_light_index;
	NodeTransformMap node_transform_cache;
	MaterialIndexMap opaque_nodes_by_material_index;
	MaterialIndexMap blended_nodes_by_material_index;
	NodeOverrideMap node_binds;
	OverrideVector all_binds;
	LongVector validated_skins;
	std::vector<GLuint> skin_tex;
	NodePrimCenterMap local_mesh_to_center_points_by_primitive;

	std::vector<lv_gltf_mesh_data_t> meshes;
	std::vector<GLuint> textures;
	std::vector<lv_gltf_uniform_locs> shader_uniforms;
	std::vector<lv_gltf_renwin_shaderset_t> shader_sets;

	fastgltf::math::fvec3 vertex_max;
	fastgltf::math::fvec3 vertex_min;
	fastgltf::math::fvec3 vertex_cen;
	float bound_radius;

	bool has_any_cameras;
	int32_t current_camera_index;
	int32_t last_camera_index;
	fastgltf::math::fmat4x4 view_mat;
	fastgltf::math::fvec3 view_pos;

	int32_t last_anim_num;
	float cur_anim_maxtime;
	float local_timestamp;

	size_t last_material_index;
	bool last_pass_was_transmission;
	bool last_frame_was_antialiased;
	bool _last_frame_no_motion;
	bool __last_frame_no_motion;
	bool nodes_parsed;
	lv_gltf_data_t *linked_view_source;
};


/**
 * @brief Retrieve a specific texture from the GLTF model data.
 *
 * @param data Pointer to the lv_gltf_data_t object containing the model data.
 * @param index The index of the texture to retrieve.
 * @return Pointer to the texture object.
 */
GLuint lv_gltf_data_get_texture(lv_gltf_data_t *data, size_t index);

/**
 * @brief Retrieve the uniform location IDs for all textures from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I Draw order index.
 * @return Pointer to the UniformLocs structure containing the uniform location IDs.
 */
lv_gltf_uniform_locs *get_uniform_ids(lv_gltf_data_t *D, size_t I);

/**
 * @brief Retrieve the shader program associated with a specific index from the GLTF model data.
 *
 * @param data Pointer to the lv_gltf_data_t object containing the model data.
 * @param index The index of the shader program to retrieve.
 * @return The shader program ID.
 */
GLuint lv_gltf_data_get_shader_program(lv_gltf_data_t *data, size_t index);

/**
 * @brief Retrieve the shader set associated with a specific index from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the shader set to retrieve.
 * @return Pointer to the gl_renwin_shaderset_t structure containing the shader set.
 */
lv_gltf_renwin_shaderset_t *lv_gltf_data_get_shader_set(lv_gltf_data_t *data, size_t index);

/**
 * @brief Retrieve the minimum bounds (X/Y/Z) of the model from the GLTF data.
 *
 * @param data Pointer to the lv_gltf_data_t object containing the model data.
 * @return Pointer to a 3-element float array representing the minimum bounds.
 */
fastgltf::math::fvec3 lv_gltf_data_get_bounds_min(const lv_gltf_data_t *data);

/**
 * @brief Retrieve the maximum bounds (X/Y/Z) of the model from the GLTF data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Pointer to a 3-element float array representing the maximum bounds.
 */
fastgltf::math::fvec3 lv_gltf_data_get_bounds_max(const lv_gltf_data_t *data);

/**
 * @brief Retrieve the center coordinates of the GLTF data object.
 *
 * @param D Pointer to the lv_gltf_data_t object from which to get the center.
 * @return Pointer to an array containing the center coordinates (x, y, z).
 */
fastgltf::math::fvec3 lv_gltf_data_get_center(const lv_gltf_data_t * data);

/**
 * @brief Retrieve the filename of the GLTF model.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Pointer to a constant character string representing the filename.
 */
const char *lv_gltf_get_filename(const lv_gltf_data_t *data);

/**
 * @brief Check if the centerpoint cache contains a specific entry.
 *
 * @param data Pointer to the lv_gltf_data_t object containing the model data.
 * @param index The index of the entry to check.
 * @param element The specific parameter to check within the cache.
 * @return True if the cache contains the entry, false otherwise.
 */
bool lv_gltf_data_centerpoint_cache_contains(lv_gltf_data_t *data, size_t index, int32_t element);

/**
 * @brief Retrieve a specific primitive from a mesh.
 *
 * @param M Pointer to the MeshData structure containing the mesh data.
 * @param I The index of the primitive to retrieve.
 * @return Pointer to the primitive data.
 */
lv_gltf_primitive_t *lv_gltf_data_get_primitive_from_mesh(lv_gltf_mesh_data_t *M, size_t I);

/**
 * @brief Retrieve the asset associated with the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Pointer to the asset data.
 */
fastgltf::Asset *lv_gltf_data_get_asset(lv_gltf_data_t *data);

/**
 * @brief Retrieve mesh data for a specific index from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the mesh data to retrieve.
 * @return Pointer to the MeshData structure containing the mesh data.
 */
lv_gltf_mesh_data_t *lv_gltf_data_get_mesh(lv_gltf_data_t *data, size_t index);

/**
 * @brief Retrieve the skin texture index for a specific entry in the GLTF model data.
 *
 * @param data Pointer to the lv_gltf_data_t object containing the model data.
 * @param index The index of the entry for which to retrieve the skin texture index.
 * @return The skin texture index.
 */
GLuint lv_gltf_data_get_skin_texture_at(lv_gltf_data_t *data, size_t index);

/**
 * @brief Check if the validated skins contain a specific entry.
 *
 * @param data Pointer to the lv_gltf_data_t object containing the model data.
 * @param index The index of the skin to check.
 * @return True if the validated skins contain the entry, false otherwise.
 */
bool lv_gltf_data_validated_skins_contains(lv_gltf_data_t *data, size_t index);

/**
 * @brief Validate a specific skin in the GLTF model data.
 *
 * @param data Pointer to the lv_gltf_data_t object containing the model data.
 * @param index The index of the skin to validate.
 */
void lv_gltf_data_validate_skin(lv_gltf_data_t *data, size_t index);

/**
 * @brief Add an opaque node primitive to the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the primitive to add.
 * @param N Pointer to the NodePtr representing the node to add.
 * @param P The specific parameter associated with the primitive.
 */
void lv_gltf_data_add_opaque_node_primitive(lv_gltf_data_t * data, size_t index,
                          fastgltf::Node * node, size_t primitive_index);

/**
 * @brief Add a blended node primitive to the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the primitive to add.
 * @param N Pointer to the NodePtr representing the node to add.
 * @param P The specific parameter associated with the primitive.
 */
void lv_gltf_data_add_blended_node_primitive(lv_gltf_data_t *data, size_t mesh_index, fastgltf::Node *node, size_t primitive_index);

/**
 * @brief Set the cached transformation matrix for a specific node in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param N Pointer to the NodePtr representing the node for which to set the transformation.
 * @param M The transformation matrix to cache.
 */
void lv_gltf_data_set_cached_transform(lv_gltf_data_t *D, NodePtr N,
			  fastgltf::math::fmat4x4 M);

/**
 * @brief Clear the transformation cache for the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 */
void lv_gltf_data_clear_transform_cache(lv_gltf_data_t *D);

/**
 * @brief Retrieve the cached transformation matrix for a specific node in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param N Pointer to the NodePtr representing the node for which to retrieve the transformation.
 * @return The cached transformation matrix.
 */
fastgltf::math::fmat4x4 lv_gltf_data_get_cached_transform(lv_gltf_data_t *D, NodePtr N);

/**
 * @brief Check if a cached transformation matrix exists for a given node.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param N Pointer to the NodePtr representing the node for which to retrieve the transformation.
 * @return true if a cache item exists, false otherwise
 */
bool lv_gltf_data_has_cached_transform(lv_gltf_data_t *D, NodePtr N);

/**
 * @brief Check if the transformation cache is empty.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return True if the transformation cache is empty, false otherwise.
 */
bool lv_gltf_data_transform_cache_is_empty(lv_gltf_data_t *D);

/**
 * @brief Retrieve the size of the skins in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return The size of the skins.
 */
size_t lv_gltf_data_get_skins_size(lv_gltf_data_t *D);

/**
 * @brief Retrieve a specific skin from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the skin to retrieve.
 * @return The skin index.
 */
int32_t lv_gltf_data_get_skin(lv_gltf_data_t *D, size_t I);

/**
 * @brief Ingest and discover defines for a specific node and primitive in the GLTF model data.
 *
 * @param data_obj Pointer to the lv_gltf_data_t object containing the model data.
 * @param node Pointer to the node for which to ingest defines.
 * @param prim Pointer to the primitive for which to ingest defines.
 */
void lv_gltf_data_injest_discover_defines(lv_gltf_data_t * data, fastgltf::Node * node, fastgltf::Primitive * prim);

/**
 * @brief Retrieve the center point of a specific mesh element from the GLTF model data.
 *
 * @param gltf_data Pointer to the lv_gltf_data_t object containing the model data.
 * @param matrix The transformation matrix to apply when calculating the center point.
 * @param meshIndex The index of the mesh from which to retrieve the center point.
 * @param elem The specific element index within the mesh.
 * @return The center point as a fastgltf::math::fvec3 structure.
 */
fastgltf::math::fvec3 lv_gltf_data_get_centerpoint(lv_gltf_data_t *gltf_data,
					      fastgltf::math::fmat4x4 matrix,
					      size_t mesh_index, int32_t elem);

/**
 * @brief Set the shader information for a specific index in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param _index The index of the shader to set.
 * @param _uniforms The UniformLocs structure containing the uniform locations.
 * @param _shaderset The gl_renwin_shaderset_t structure containing the shader set.
 */
void lv_gltf_data_set_shader(lv_gltf_data_t *D, size_t _index, lv_gltf_uniform_locs _uniforms,
		lv_gltf_renwin_shaderset_t _shaderset);

/**
 * @brief Initialize shaders for the GLTF model data with a specified maximum index.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param _max_index The maximum index for the shaders to initialize.
 */
void lv_gltf_data_init_shaders(lv_gltf_data_t *D, size_t _max_index);


lv_gltf_mesh_data_t *lv_gltf_get_new_meshdata(lv_gltf_data_t *_data);

lv_gltf_data_t *lv_gltf_data_create_internal(const char *gltf_path,
					     fastgltf::Asset);

lv_gltf_data_t *lv_gltf_data_load_internal(const void *data_source,
					   size_t data_size,
					   lv_gl_shader_manager_t *shaders);

/*void set_node_at_path(lv_gltf_data_t *data, const std::string &path,*/
/*		      fastgltf::Node *node);*/
/*void set_node_at_ip(lv_gltf_data_t *data, const std::string &ip,*/
/*		    fastgltf::Node *node);*/
/*void set_node_index(lv_gltf_data_t *data, size_t index, fastgltf::Node *node);*/

fastgltf::math::fvec4 lv_gltf_get_primitive_centerpoint(lv_gltf_data_t *data,
							fastgltf::Mesh &mesh,
							uint32_t prim_num);

fastgltf::math::fvec3 get_cached_centerpoint(lv_gltf_data_t *data, size_t index,
					     int32_t element,
					     fastgltf::math::fmat4x4 matrix);

void lv_gltf_data_destroy_textures(lv_gltf_data_t* data);
GLuint lv_gltf_data_create_texture(lv_gltf_data_t* data);
void lv_gltf_data_nodes_init(lv_gltf_data_t *data, size_t size);
void lv_gltf_data_node_init(lv_gltf_data_node_t *node, fastgltf::Node *fastgltf_node, const char *ip, const char *path);
void lv_gltf_data_node_add(lv_gltf_data_t *data, const lv_gltf_data_node_t *data_node);
void lv_gltf_data_node_delete(lv_gltf_data_node_t *node);

/**
 * @brief Retrieve the pixel data for a specific texture in a GLTF model.
 *
 * @param pixels Pointer to the memory where the pixel data will be stored.
 * @param data_obj Pointer to the lv_gltf_data_t object containing the model data.
 * @param model_texture_index The index of the texture in the model.
 * @param mipmapnum The mipmap level to retrieve pixel data for.
 * @param width The width of the texture.
 * @param height The height of the texture.
 * @param has_alpha Flag indicating whether the texture includes an alpha channel.
 * @return True if the pixel data was successfully retrieved, false otherwise.
 */
bool lv_gltf_data_get_texture_pixels(void * pixels,
                                           lv_gltf_data_t * data_obj,
                                           uint32_t model_texture_index,
                                           uint32_t mipmapnum, uint32_t width,
                                           uint32_t height, bool has_alpha);

lv_gltf_data_node_t * lv_gltf_data_node_get_by_index(lv_gltf_data_t* data, size_t index);
lv_gltf_data_node_t * lv_gltf_data_node_get_by_ip(lv_gltf_data_t* data, const char* ip);
lv_gltf_data_node_t * lv_gltf_data_node_get_by_path(lv_gltf_data_t* data, const char* path);

#endif

#endif /*LV_GLTFVIEW_H*/
