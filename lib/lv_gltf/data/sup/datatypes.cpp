
#include "../lv_gltf_data_internal.hpp"
#include "stdlib/lv_mem.h"
#include <GLFW/glfw3.h>

#include <fastgltf/types.hpp>
#include <algorithm>

#define MAX_OVERRIDES 1024

mesh_data_t *lv_gltf_get_new_meshdata(lv_gltf_data_t *data)
{
	mesh_data_t outMesh = {};
	data->meshes.emplace_back(outMesh);
	return &(data->meshes[data->meshes.size() - 1]);
}

uint32_t get_primitive_datasize(void)
{
	return sizeof(Primitive);
}

lv_gltf_data_t *lv_gltf_data_create_internal(const char *gltf_path,
					     fastgltf::Asset asset)
{
	lv_gltf_data_t *data = (lv_gltf_data_t *)lv_zalloc(sizeof(*data));
	LV_ASSERT_MALLOC(data);
	new (data) lv_gltf_data_t;
	new (&data->asset) fastgltf::Asset(std::move(asset));
	data->filename = gltf_path;
	data->load_success = false;
	data->has_any_cameras = false;
	data->current_camera_index = -1;
	data->last_camera_index = -5;
	data->last_anim_num = -5;
	data->cur_anim_maxtime = -1.f;
	data->local_timestamp = 0.0f;
	data->last_material_index = 99999;
	data->last_pass_was_transmission = false;
	data->last_frame_was_antialiased = false;
	data->last_frame_no_motion = false;
	data->last_frame_no_motion = false;
	data->nodes_parsed = false;
	new (&data->overrides) NodeOverrideMap();
	new (&data->all_overrides) OverrideVector();

	data->all_overrides.reserve(MAX_OVERRIDES);

	new (&data->node_by_path) StringNodeMap();
	new (&data->node_by_ip) StringNodeMap();
	new (&data->index_by_node) NodeIntMap();
	new (&data->node_by_index) NodeVector();
	new (&data->node_transform_cache) NodeTransformMap();
	new (&data->opaque_nodes_by_material_index) MaterialIndexMap();
	new (&data->blended_nodes_by_material_index) MaterialIndexMap();
	new (&data->distance_sort_nodes) NodeDistanceVector();
	new (&data->validated_skins) LongVector();
	new (&data->skin_tex) IntVector();
	new (&data->local_mesh_to_center_points_by_primitive)
		NodePrimCenterMap();

	new (&data->node_by_light_index) NodeVector();
	new (&data->meshes) std::vector<mesh_data_t>();
	new (&data->textures) std::vector<Texture>();
	new (&data->cameras) std::vector<fastgltf::math::fmat4x4>();
	new (&data->shader_uniforms) std::vector<UniformLocs>();
	new (&data->shader_sets) std::vector<gl_renwin_shaderset_t>();
	return data;
}

void lv_gltf_data_destroy(lv_gltf_data_t *_data)
{
	_data->all_overrides.clear();
	_data->overrides.clear();
	_data->node_by_path.clear();
	_data->node_by_ip.clear();
	_data->index_by_node.clear();
	_data->node_by_index.clear();
	_data->node_transform_cache.clear();
	_data->opaque_nodes_by_material_index.clear();
	_data->blended_nodes_by_material_index.clear();
	_data->distance_sort_nodes.clear();
	_data->validated_skins.clear();
	_data->local_mesh_to_center_points_by_primitive.clear();
	_data->node_by_light_index.clear();
	_data->meshes.clear();
	_data->textures.clear();
	_data->cameras.clear();
	_data->shader_uniforms.clear();
	_data->shader_sets.clear();

	glDeleteTextures(_data->skin_tex.size(),
			 (const GLuint *)_data->skin_tex.data());
	_data->skin_tex.clear();
	lv_free(_data);
}

const char *lv_gltf_get_filename(lv_gltf_data_t *data)
{
	return (data->filename);
}

mesh_data_t *get_meshdata_num(lv_gltf_data_t *data, _UINT I)
{
	return &data->meshes[I];
}
void *get_texdata_set(lv_gltf_data_t *data)
{
	return &(data->textures);
}
double lv_gltf_data_get_radius(lv_gltf_data_t *data)
{
	return (double)data->bound_radius;
}
int64_t lv_gltf_data_get_int_radiusX1000(lv_gltf_data_t *data)
{
	return (int64_t)(data->bound_radius * 1000);
}
float *lv_gltf_data_get_center(lv_gltf_data_t *data)
{
	return data->vertex_cen;
}
float *get_bounds_min(lv_gltf_data_t *data)
{
	return data->vertex_min;
}
float *get_bounds_max(lv_gltf_data_t *data)
{
	return data->vertex_max;
}
void *get_skintex_set(lv_gltf_data_t *data)
{
	return &data->skin_tex;
}
int32_t get_skintex_at(lv_gltf_data_t *data, _UINT I)
{
	return data->skin_tex[I];
}
uint64_t get_shader_program(lv_gltf_data_t *data, _UINT I)
{
	return data->shader_sets[I].program;
}
Texture *get_texdata(lv_gltf_data_t *data, _UINT I)
{
	return &data->textures[I];
}
UniformLocs *get_uniform_ids(lv_gltf_data_t *data, _UINT I)
{
	return &data->shader_uniforms[I];
}
uint64_t get_texdata_glid(lv_gltf_data_t *data, _UINT I)
{
	return get_texdata(data, I)->texture;
}
void allocate_index(lv_gltf_data_t *data, _UINT I)
{
	data->node_by_index.resize(I);
}
void set_node_at_path(lv_gltf_data_t *data, const std::string &path,
		      fastgltf::Node *node)
{
	data->node_by_path[path] = node;
}
void set_node_at_ip(lv_gltf_data_t *data, const std::string &ip,
		    fastgltf::Node *node)
{
	data->node_by_ip[ip] = node;
}
void set_node_index(lv_gltf_data_t *data, size_t index, fastgltf::Node *node)
{
	data->node_by_index[index] = node;
	data->index_by_node[node] = index;
}
void *get_prim_from_mesh(mesh_data_t *M, uint64_t I)
{
	return &(M->primitives[I]);
}

fastgltf::math::fmat4x4 get_cached_transform(lv_gltf_data_t *data,
					     fastgltf::Node *node)
{
	return data->node_transform_cache[node];
}
bool has_cached_transform(lv_gltf_data_t *data, fastgltf::Node *node)
{
	return (data->node_transform_cache.find(node) !=
		data->node_transform_cache.end());
}
void set_cached_transform(lv_gltf_data_t *data, fastgltf::Node *node,
			  fastgltf::math::fmat4x4 M)
{
	data->node_transform_cache[node] = M;
}
void clear_transform_cache(lv_gltf_data_t *data)
{
	data->node_transform_cache.clear();
}
bool transform_cache_is_empty(lv_gltf_data_t *data)
{
	return data->node_transform_cache.empty();
}

void recache_centerpoint(lv_gltf_data_t *data, _UINT I, int32_t P)
{
	data->local_mesh_to_center_points_by_primitive[I][P] =
		lv_gltf_get_primitive_centerpoint(data, data->asset.meshes[I],
						  P);
}
bool centerpoint_cache_contains(lv_gltf_data_t *data, _UINT I, int32_t P)
{
	return (data->local_mesh_to_center_points_by_primitive.find(I) !=
		data->local_mesh_to_center_points_by_primitive.end()) &&
	       (data->local_mesh_to_center_points_by_primitive[I].find(P) !=
		data->local_mesh_to_center_points_by_primitive[I].end());
}
bool validated_skins_contains(lv_gltf_data_t *data, int64_t I)
{
	return ((std::find(data->validated_skins.begin(),
			   data->validated_skins.end(),
			   I) != data->validated_skins.end()));
}
void validate_skin(lv_gltf_data_t *data, int64_t I)
{
	data->validated_skins.push_back(I);
}
_UINT get_skins_size(lv_gltf_data_t *data)
{
	return data->validated_skins.size();
}
int32_t get_skin(lv_gltf_data_t *data, uint64_t I)
{
	return data->validated_skins[I];
}

void add_opaque_node_prim(lv_gltf_data_t *data, size_t index,
			  fastgltf::Node *node, int32_t P)
{
	data->opaque_nodes_by_material_index[index].push_back(
		std::make_pair(node, std::as_const(P)));
}
MaterialIndexMap::iterator get_opaque_begin(lv_gltf_data_t *data)
{
	return data->opaque_nodes_by_material_index.begin();
}
MaterialIndexMap::iterator get_opaque_end(lv_gltf_data_t *data)
{
	return data->opaque_nodes_by_material_index.end();
}

void add_blended_node_prim(lv_gltf_data_t *data, size_t index,
			   fastgltf::Node *node, int32_t P)
{
	data->blended_nodes_by_material_index[index].push_back(
		std::make_pair(node, std::as_const(P)));
}
MaterialIndexMap::iterator get_blended_begin(lv_gltf_data_t *data)
{
	return data->blended_nodes_by_material_index.begin();
}
MaterialIndexMap::iterator get_blended_end(lv_gltf_data_t *data)
{
	return data->blended_nodes_by_material_index.end();
}

void clear_distance_sort(lv_gltf_data_t *data)
{
	data->distance_sort_nodes.clear();
}

void add_distance_sort_prim(lv_gltf_data_t *data, NodeIndexDistancePair P)
{
	data->distance_sort_nodes.push_back(P);
}
NodeDistanceVector::iterator get_distance_sort_begin(lv_gltf_data_t *data)
{
	return data->distance_sort_nodes.begin();
}
NodeDistanceVector::iterator get_distance_sort_end(lv_gltf_data_t *data)
{
	return data->distance_sort_nodes.end();
}

gl_renwin_shaderset_t *get_shader_set(lv_gltf_data_t *data, _UINT I)
{
	return &data->shader_sets[I];
}

fastgltf::math::fvec3 get_cached_centerpoint(lv_gltf_data_t *data, _UINT I,
					     int32_t P, _MAT4 M)
{
	fastgltf::math::fvec4 tv = fastgltf::math::fvec4(
		data->local_mesh_to_center_points_by_primitive[I][P]);
	tv[3] = 1.f;
	tv = M * tv;
	return fastgltf::math::fvec3(tv[0], tv[1], tv[2]);
}

void set_shader(lv_gltf_data_t *data, uint64_t _index, UniformLocs _uniforms,
		gl_renwin_shaderset_t _shaderset)
{
	data->shader_uniforms[_index] = _uniforms;
	data->shader_sets[_index] = _shaderset;
}

void init_shaders(lv_gltf_data_t *data, uint64_t _max_index)
{
	auto _prevsize = data->shader_sets.size();
	data->shader_sets.resize(_max_index + 1);
	data->shader_uniforms.resize(_max_index + 1);
	if (_prevsize < _max_index) {
		for (uint64_t _ii = _prevsize; _ii <= _max_index; _ii++) {
			data->shader_sets[_ii] = gl_renwin_shaderset_t();
			data->shader_sets[_ii].ready = false;
		}
	}
}

void set_bounds_info(lv_gltf_data_t *data, fastgltf::math::fvec3 _vmin,
		     fastgltf::math::fvec3 _vmax, fastgltf::math::fvec3 _vcen,
		     float _radius)
{
	{
		auto _d = _vmin.data();
		data->vertex_min[0] = _d[0];
		data->vertex_min[1] = _d[1];
		data->vertex_min[2] = _d[2];
	}
	{
		auto _d = _vmax.data();
		data->vertex_max[0] = _d[0];
		data->vertex_max[1] = _d[1];
		data->vertex_max[2] = _d[2];
	}
	{
		auto _d = _vcen.data();
		data->vertex_cen[0] = _d[0];
		data->vertex_cen[1] = _d[1];
		data->vertex_cen[2] = _d[2];
	}
	data->bound_radius = _radius;
}

void lv_gltf_data_copy_bounds_info(lv_gltf_data_t *to, lv_gltf_data_t *from)
{
	{
		to->vertex_min[0] = from->vertex_min[0];
		to->vertex_min[1] = from->vertex_min[1];
		to->vertex_min[2] = from->vertex_min[2];
	}
	{
		to->vertex_max[0] = from->vertex_max[0];
		to->vertex_max[1] = from->vertex_max[1];
		to->vertex_max[2] = from->vertex_max[2];
	}
	{
		to->vertex_cen[0] = from->vertex_cen[0];
		to->vertex_cen[1] = from->vertex_cen[1];
		to->vertex_cen[2] = from->vertex_cen[2];
	}
	to->bound_radius = from->bound_radius;
}
