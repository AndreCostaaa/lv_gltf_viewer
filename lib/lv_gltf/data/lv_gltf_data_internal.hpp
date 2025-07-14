#ifndef LV_GLTFDATA_HPP
#define LV_GLTFDATA_HPP

#include "lv_gltf_data.h"
#include "lv_gltf_data_internal.h"
#include "lv_gltf_override.h"

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
using NodeIndexPair = std::pair<NodePtr, int32_t>;
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
using NodeOverrideMap = std::map<NodePtr, lv_gltf_override_t *>;
// Map of Overrides by Node
using OverrideVector = std::vector<lv_gltf_override_t>;

typedef struct {
	GLuint drawsBuffer;
	std::vector<Primitive> primitives;
} mesh_data_t;

struct lv_gltf_data_struct {
	const char *filename;
	fastgltf::Asset asset;
	bool load_success;
	StringNodeMap node_by_path;
	StringNodeMap node_by_ip;
	NodeVector node_by_index;
	NodeVector node_by_light_index;
	NodeTransformMap node_transform_cache;
	MaterialIndexMap opaque_nodes_by_material_index;
	MaterialIndexMap blended_nodes_by_material_index;
	NodeDistanceVector distance_sort_nodes;
	NodeOverrideMap overrides;
	OverrideVector all_overrides;
	LongVector validated_skins;
	IntVector skin_tex;
	NodePrimCenterMap local_mesh_to_center_points_by_primitive;

	std::vector<mesh_data_t> meshes;
	std::vector<Texture> textures;
	std::vector<UniformLocs> shader_uniforms;
	std::vector<gl_renwin_shaderset_t> shader_sets;

	float vertex_max[3];
	float vertex_min[3];
	float vertex_cen[3];
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

typedef uint64_t _UINT;
typedef NodePtr _NODE;
typedef fastgltf::math::fmat4x4 _MAT4;
/**
 * @brief Retrieve the texture data set from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Pointer to the texture data set.
 */
void *get_texdata_set(lv_gltf_data_t *D);

/**
 * @brief Retrieve a specific texture from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the texture to retrieve.
 * @return Pointer to the Texture object.
 */
Texture *get_texdata(lv_gltf_data_t *D, uint64_t I);

/**
 * @brief Retrieve the OpenGL ID of a specific texture from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the texture whose OpenGL ID is to be retrieved.
 * @return The OpenGL ID of the texture.
 */
uint64_t get_texdata_glid(lv_gltf_data_t *D, uint64_t I);

/**
 * @brief Retrieve the uniform location IDs for all textures from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I Draw order index.
 * @return Pointer to the UniformLocs structure containing the uniform location IDs.
 */
UniformLocs *get_uniform_ids(lv_gltf_data_t *D, uint64_t I);

/**
 * @brief Retrieve the shader program associated with a specific index from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the shader program to retrieve.
 * @return The shader program ID.
 */
uint64_t get_shader_program(lv_gltf_data_t *D, uint64_t I);

/**
 * @brief Retrieve the shader set associated with a specific index from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the shader set to retrieve.
 * @return Pointer to the gl_renwin_shaderset_t structure containing the shader set.
 */
gl_renwin_shaderset_t *get_shader_set(lv_gltf_data_t *D, uint64_t I);

/**
 * @brief Retrieve the minimum bounds (X/Y/Z) of the model from the GLTF data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Pointer to a 3-element float array representing the minimum bounds.
 */
float *get_bounds_min(lv_gltf_data_t *D);

/**
 * @brief Retrieve the maximum bounds (X/Y/Z) of the model from the GLTF data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Pointer to a 3-element float array representing the maximum bounds.
 */
float *get_bounds_max(lv_gltf_data_t *D);

/**
 * @brief Retrieve the center point of the model from the GLTF data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Pointer to a float array representing the center point (X/Y/Z).
 */
float *setup_get_center(lv_gltf_data_t *D);

/**
 * @brief Retrieve the radius of the model from the GLTF data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return The radius of the model.
 */
double get_radius(lv_gltf_data_t *D);

/**
 * @brief Retrieve the filename of the GLTF model.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Pointer to a constant character string representing the filename.
 */
const char *lv_gltf_get_filename(lv_gltf_data_t *D);

/**
 * @brief Check if the centerpoint cache contains a specific entry.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the entry to check.
 * @param P The specific parameter to check within the cache.
 * @return True if the cache contains the entry, false otherwise.
 */
bool centerpoint_cache_contains(lv_gltf_data_t *D, uint64_t I, int32_t P);

/**
 * @brief Retrieve a specific primitive from a mesh.
 *
 * @param M Pointer to the MeshData structure containing the mesh data.
 * @param I The index of the primitive to retrieve.
 * @return Pointer to the primitive data.
 */
void *get_prim_from_mesh(mesh_data_t *M, uint64_t I);

/**
 * @brief Retrieve the asset associated with the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Pointer to the asset data.
 */
fastgltf::Asset *lv_gltf_data_get_asset(lv_gltf_data_t *data);

#define TEXDSET(x)		 ((std::vector<Texture> *)get_texdata_set(x))
#define TEXD(x, y)		 ((Texture *)get_texdata(x, y))
#define TEXDGLID(x, y)		 ((_UINT)get_texdata_glid(x, y))
#define MATRIXSET(v)		 ((_MatrixSet *)get_matrix_set(v))
#define SKINTEXS(d)		 ((IntVector *)get_skintex_set(d))
#define GET_PRIM_FROM_MESH(m, i) ((Primitive *)get_prim_from_mesh(m, i))

/**
 * @brief Allocate an index for a specific entry in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index to allocate.
 */
void allocate_index(lv_gltf_data_t *D, uint64_t I);

/**
 * @brief Recache the centerpoint for a specific entry in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the entry to recache.
 * @param P The specific parameter to recache.
 */
void recache_centerpoint(lv_gltf_data_t *D, uint64_t I, int32_t P);

/**
 * @brief Retrieve mesh data for a specific index from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the mesh data to retrieve.
 * @return Pointer to the MeshData structure containing the mesh data.
 */
mesh_data_t *get_meshdata_num(lv_gltf_data_t *D, uint64_t I);

/**
 * @brief Retrieve the skin texture index for a specific entry in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the entry for which to retrieve the skin texture index.
 * @return The skin texture index.
 */
int32_t get_skintex_at(lv_gltf_data_t *D, uint64_t I);

/**
 * @brief Retrieve the set of skin textures associated with the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Pointer to the skin texture set.
 */
void *get_skintex_set(lv_gltf_data_t *D);

/**
 * @brief Check if the validated skins contain a specific entry.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the skin to check.
 * @return True if the validated skins contain the entry, false otherwise.
 */
bool validated_skins_contains(lv_gltf_data_t *D, int64_t I);

/**
 * @brief Validate a specific skin in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the skin to validate.
 */
void validate_skin(lv_gltf_data_t *D, int64_t I);

/**
 * @brief Add an opaque node primitive to the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the primitive to add.
 * @param N Pointer to the NodePtr representing the node to add.
 * @param P The specific parameter associated with the primitive.
 */
void add_opaque_node_prim(lv_gltf_data_t *D, uint64_t I, NodePtr N, int32_t P);

/**
 * @brief Retrieve an iterator to the beginning of the opaque material index map.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Iterator to the beginning of the MaterialIndexMap.
 */
MaterialIndexMap::iterator get_opaque_begin(lv_gltf_data_t *D);

/**
 * @brief Retrieve an iterator to the end of the opaque material index map.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Iterator to the end of the MaterialIndexMap.
 */
MaterialIndexMap::iterator get_opaque_end(lv_gltf_data_t *D);

/**
 * @brief Add a blended node primitive to the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the primitive to add.
 * @param N Pointer to the NodePtr representing the node to add.
 * @param P The specific parameter associated with the primitive.
 */
void add_blended_node_prim(lv_gltf_data_t *D, uint64_t I, NodePtr N, int32_t P);

/**
 * @brief Retrieve an iterator to the beginning of the blended material index map.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Iterator to the beginning of the MaterialIndexMap.
 */
MaterialIndexMap::iterator get_blended_begin(lv_gltf_data_t *D);

/**
 * @brief Retrieve an iterator to the end of the blended material index map.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Iterator to the end of the MaterialIndexMap.
 */
MaterialIndexMap::iterator get_blended_end(lv_gltf_data_t *D);

/**
 * @brief Clear the distance sorting data for the GLTF model.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 */
void clear_distance_sort(lv_gltf_data_t *D);

/**
 * @brief Add a primitive to the distance sorting data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param P The NodeIndexDistancePair representing the primitive to add.
 */
void add_distance_sort_prim(lv_gltf_data_t *D, NodeIndexDistancePair P);

/**
 * @brief Retrieve an iterator to the beginning of the distance sorted node vector.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Iterator to the beginning of the NodeDistanceVector.
 */
NodeDistanceVector::iterator get_distance_sort_begin(lv_gltf_data_t *D);

/**
 * @brief Retrieve an iterator to the end of the distance sorted node vector.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return Iterator to the end of the NodeDistanceVector.
 */
NodeDistanceVector::iterator get_distance_sort_end(lv_gltf_data_t *D);

/**
 * @brief Set the cached transformation matrix for a specific node in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param N Pointer to the NodePtr representing the node for which to set the transformation.
 * @param M The transformation matrix to cache.
 */
void set_cached_transform(lv_gltf_data_t *D, NodePtr N,
			  fastgltf::math::fmat4x4 M);

/**
 * @brief Clear the transformation cache for the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 */
void clear_transform_cache(lv_gltf_data_t *D);

/**
 * @brief Retrieve the cached transformation matrix for a specific node in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param N Pointer to the NodePtr representing the node for which to retrieve the transformation.
 * @return The cached transformation matrix.
 */
fastgltf::math::fmat4x4 get_cached_transform(lv_gltf_data_t *D, NodePtr N);

/**
 * @brief Check if a cached transformation matrix exists for a given node.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param N Pointer to the NodePtr representing the node for which to retrieve the transformation.
 * @return true if a cache item exists, false otherwise
 */
bool has_cached_transform(lv_gltf_data_t *D, NodePtr N);

/**
 * @brief Check if the transformation cache is empty.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return True if the transformation cache is empty, false otherwise.
 */
bool transform_cache_is_empty(lv_gltf_data_t *D);

/**
 * @brief Retrieve the size of the skins in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @return The size of the skins.
 */
uint64_t get_skins_size(lv_gltf_data_t *D);

/**
 * @brief Retrieve a specific skin from the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param I The index of the skin to retrieve.
 * @return The skin index.
 */
int32_t get_skin(lv_gltf_data_t *D, uint64_t I);

/**
 * @brief Ingest and discover defines for a specific node and primitive in the GLTF model data.
 *
 * @param data_obj Pointer to the lv_gltf_data_t object containing the model data.
 * @param node Pointer to the node for which to ingest defines.
 * @param prim Pointer to the primitive for which to ingest defines.
 */
void injest_discover_defines(lv_gltf_data_t *data_obj, void *node, void *prim);

/**
 * @brief Retrieve the center point of a specific mesh element from the GLTF model data.
 *
 * @param gltf_data Pointer to the lv_gltf_data_t object containing the model data.
 * @param matrix The transformation matrix to apply when calculating the center point.
 * @param meshIndex The index of the mesh from which to retrieve the center point.
 * @param elem The specific element index within the mesh.
 * @return The center point as a fastgltf::math::fvec3 structure.
 */
fastgltf::math::fvec3 lv_gltf_get_centerpoint(lv_gltf_data_t *gltf_data,
					      fastgltf::math::fmat4x4 matrix,
					      uint32_t meshIndex, int32_t elem);

/**
 * @brief Set the shader information for a specific index in the GLTF model data.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param _index The index of the shader to set.
 * @param _uniforms The UniformLocs structure containing the uniform locations.
 * @param _shaderset The gl_renwin_shaderset_t structure containing the shader set.
 */
void set_shader(lv_gltf_data_t *D, uint64_t _index, UniformLocs _uniforms,
		gl_renwin_shaderset_t _shaderset);

/**
 * @brief Initialize shaders for the GLTF model data with a specified maximum index.
 *
 * @param D Pointer to the lv_gltf_data_t object containing the model data.
 * @param _max_index The maximum index for the shaders to initialize.
 */
void init_shaders(lv_gltf_data_t *D, uint64_t _max_index);

void set_bounds_info(lv_gltf_data_t *data, fastgltf::math::fvec3 v_min,
		     fastgltf::math::fvec3 v_max, fastgltf::math::fvec3 v_cen,
		     float radius);

mesh_data_t *lv_gltf_get_new_meshdata(lv_gltf_data_t *_data);

lv_gltf_data_t *lv_gltf_data_create_internal(const char *gltf_path,
					     fastgltf::Asset);

lv_gltf_data_t *lv_gltf_data_load_internal(const void *data_source,
					   size_t data_size,
					   lv_gl_shader_manager_t *shaders);

void set_node_at_path(lv_gltf_data_t *data, const std::string &path,
		      fastgltf::Node *node);
void set_node_at_ip(lv_gltf_data_t *data, const std::string &ip,
		    fastgltf::Node *node);
void set_node_index(lv_gltf_data_t *data, size_t index, fastgltf::Node *node);

fastgltf::math::fvec4 lv_gltf_get_primitive_centerpoint(lv_gltf_data_t *data,
							fastgltf::Mesh &mesh,
							uint32_t prim_num);

#endif

#endif /*LV_GLTFVIEW_H*/
