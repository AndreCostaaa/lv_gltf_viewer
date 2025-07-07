
#include "../lv_gltf_data_internal.hpp"
#include "stdlib/lv_mem.h"
#include <GLFW/glfw3.h>

#include <fastgltf/types.hpp>
#include <algorithm>

#define MAX_OVERRIDES 1024

mesh_data_t *lv_gltf_get_new_meshdata(lv_gltf_data_t *_data)
{
	mesh_data_t outMesh = {};
	_data->meshes->emplace_back(outMesh);
	return &((*_data->meshes)[_data->meshes->size() - 1]);
}

uint32_t lv_gltf_data_get_struct_size(void)
{
	return sizeof(lv_gltf_data_t);
}

uint32_t get_primitive_datasize(void)
{
	return sizeof(Primitive);
}

lv_gltf_data_t *lv_gltf_data_create_internal(const char *gltf_path,
					     fastgltf::Asset asset)
{
	lv_gltf_data_t *data = (lv_gltf_data_t *)lv_malloc(sizeof(*data));
	LV_ASSERT_MALLOC(data);
	new (data) lv_gltf_data_t;
	new (&data->asset) fastgltf::Asset(std::move(asset));
	data->filename = gltf_path;
	data->load_success = false;
	data->has_any_cameras = false;
	data->current_camera_index = -1;
	data->last_camera_index = -5;
	data->selected_camera_node = NULL;
	data->last_anim_num = -5;
	data->cur_anim_maxtime = -1.f;
	data->local_timestamp = 0.0f;
	data->_lastMaterialIndex = 99999;
	data->_lastPassWasTransmission = false;
	data->_lastFrameWasAntialiased = false;
	data->_lastFrameNoMotion = false;
	data->__lastFrameNoMotion = false;
	data->nodes_parsed = false;
	data->view_is_linked = false;
	data->linked_view_source = NULL;
	data->overrides = new NodeOverrideMap();
	data->all_overrides = new OverrideVector();
	data->all_overrides->reserve(MAX_OVERRIDES);
	data->all_override_count = 0;
	data->node_by_path = new StringNodeMap();
	data->node_by_ip = new StringNodeMap();
	data->index_by_node = new NodeIntMap();
	data->node_by_index = new NodeVector();
	data->node_transform_cache = new NodeTransformMap();
	data->opaque_nodes_by_materialIndex = new MaterialIndexMap();
	data->blended_nodes_by_materialIndex = new MaterialIndexMap();
	data->distance_sort_nodes = new NodeDistanceVector();
	data->ibmBySkinThenNode = new MapofTransformMap();
	data->validated_skins = new LongVector();
	data->skin_tex = new IntVector();
	data->local_mesh_to_center_points_by_primitive =
		new std::map<uint32_t,
			     std::map<uint32_t, fastgltf::math::fvec4> >();
	data->node_by_light_index = new NodeVector();
	data->meshes = new std::vector<mesh_data_t>();
	data->textures = new std::vector<Texture>();
	data->cameras = new std::vector<fastgltf::math::fmat4x4>();
	data->materialBuffers = new std::vector<GLuint>();
	data->shaderUniforms = new std::vector<UniformLocs>();
	data->shaderSets = new std::vector<gl_renwin_shaderset_t>();
	return data;
}

void lv_gltf_data_destroy(lv_gltf_data_t *_data)
{
	_data->all_overrides->clear();
	delete _data->all_overrides;
	_data->all_overrides = nullptr;
	_data->overrides->clear();
	delete _data->overrides;
	_data->overrides = nullptr;
	_data->node_by_path->clear();
	delete _data->node_by_path;
	_data->node_by_path = nullptr;
	_data->node_by_ip->clear();
	delete _data->node_by_ip;
	_data->node_by_ip = nullptr;
	_data->index_by_node->clear();
	delete _data->index_by_node;
	_data->index_by_node = nullptr;
	_data->node_by_index->clear();
	delete _data->node_by_index;
	_data->node_by_index = nullptr;
	_data->node_transform_cache->clear();
	delete _data->node_transform_cache;
	_data->node_transform_cache = nullptr;
	_data->opaque_nodes_by_materialIndex->clear();
	delete _data->opaque_nodes_by_materialIndex;
	_data->opaque_nodes_by_materialIndex = nullptr;
	_data->blended_nodes_by_materialIndex->clear();
	delete _data->blended_nodes_by_materialIndex;
	_data->blended_nodes_by_materialIndex = nullptr;
	_data->distance_sort_nodes->clear();
	delete _data->distance_sort_nodes;
	_data->distance_sort_nodes = nullptr;
	_data->ibmBySkinThenNode->clear();
	delete _data->ibmBySkinThenNode;
	_data->ibmBySkinThenNode = nullptr;
	_data->validated_skins->clear();
	delete _data->validated_skins;
	_data->validated_skins = nullptr;
	_data->local_mesh_to_center_points_by_primitive->clear();
	delete _data->local_mesh_to_center_points_by_primitive;
	_data->local_mesh_to_center_points_by_primitive = nullptr;
	_data->node_by_light_index->clear();
	delete _data->node_by_light_index;
	_data->node_by_light_index = nullptr;
	_data->meshes->clear();
	delete _data->meshes;
	_data->meshes = nullptr;
	_data->textures->clear();
	delete _data->textures;
	_data->textures = nullptr;
	_data->cameras->clear();
	delete _data->cameras;
	_data->cameras = nullptr;
	_data->materialBuffers->clear();
	delete _data->materialBuffers;
	_data->materialBuffers = nullptr;
	_data->shaderUniforms->clear();
	delete _data->shaderUniforms;
	_data->shaderUniforms = nullptr;
	_data->shaderSets->clear();
	delete _data->shaderSets;
	_data->shaderSets = nullptr;

	glDeleteTextures(_data->skin_tex->size(),
			 (const GLuint *)_data->skin_tex->data());
	_data->skin_tex->clear();
	delete _data->skin_tex;
	_data->skin_tex = nullptr; // Avoid dangling pointer
	lv_free(_data);
}

const char *lv_gltf_get_filename(lv_gltf_data_t *data)
{
	return (data->filename);
}

mesh_data_t *get_meshdata_num(lv_gltf_data_t *data, _UINT I)
{
	return &((*data->meshes)[I]);
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
	return data->skin_tex;
}
int32_t get_skintex_at(lv_gltf_data_t *data, _UINT I)
{
	return (*data->skin_tex)[I];
}
uint64_t get_shader_program(lv_gltf_data_t *data, _UINT I)
{
	return (*data->shaderSets)[I].program;
}
Texture *get_texdata(lv_gltf_data_t *data, _UINT I)
{
	return &((*data->textures)[I]);
}
UniformLocs *get_uniform_ids(lv_gltf_data_t *data, _UINT I)
{
	return &((*data->shaderUniforms)[I]);
}
uint64_t get_texdata_glid(lv_gltf_data_t *data, _UINT I)
{
	return get_texdata(data, I)->texture;
}
void allocate_index(lv_gltf_data_t *data, _UINT I)
{
	(*data->node_by_index).resize(I);
}
void set_node_at_path(lv_gltf_data_t *data, const std::string &path,
		      fastgltf::Node *node)
{
	(*data->node_by_path)[path] = node;
}
void set_node_at_ip(lv_gltf_data_t *data, const std::string &ip,
		    fastgltf::Node *node)
{
	(*data->node_by_ip)[ip] = node;
}
void set_node_index(lv_gltf_data_t *data, size_t index, fastgltf::Node *node)
{
	(*data->node_by_index)[index] = node;
	(*data->index_by_node)[node] = index;
}
void *get_prim_from_mesh(mesh_data_t *M, uint64_t I)
{
	return &(M->primitives[I]);
}

fastgltf::math::fmat4x4 get_cached_transform(lv_gltf_data_t *data,
					     fastgltf::Node *node)
{
	return ((*data->node_transform_cache)[node]);
}
bool has_cached_transform(lv_gltf_data_t *data, fastgltf::Node *node)
{
	return (data->node_transform_cache->find(node) !=
		data->node_transform_cache->end());
}
void set_cached_transform(lv_gltf_data_t *data, fastgltf::Node *node,
			  fastgltf::math::fmat4x4 M)
{
	(*data->node_transform_cache)[node] = M;
}
void clear_transform_cache(lv_gltf_data_t *data)
{
	data->node_transform_cache->clear();
}
bool transform_cache_is_empty(lv_gltf_data_t *data)
{
	return data->node_transform_cache->size() == 0;
}

void recache_centerpoint(lv_gltf_data_t *data, _UINT I, int32_t P)
{
	(*data->local_mesh_to_center_points_by_primitive)[I][P] =
		lv_gltf_get_primitive_centerpoint(data, data->asset.meshes[I],
						  P);
}
bool centerpoint_cache_contains(lv_gltf_data_t *data, _UINT I, int32_t P)
{
	return ((data->local_mesh_to_center_points_by_primitive->find(I) ==
		 data->local_mesh_to_center_points_by_primitive->end()) ||
		((*data->local_mesh_to_center_points_by_primitive)[I].find(P) ==
		 (*data->local_mesh_to_center_points_by_primitive)[I].end())) ?
		       false :
		       true;
}
bool validated_skins_contains(lv_gltf_data_t *data, int64_t I)
{
	return ((std::find(data->validated_skins->begin(),
			   data->validated_skins->end(),
			   I) != data->validated_skins->end()));
}
void validate_skin(lv_gltf_data_t *data, int64_t I)
{
	data->validated_skins->push_back(I);
}
_UINT get_skins_size(lv_gltf_data_t *data)
{
	return data->validated_skins->size();
}
int32_t get_skin(lv_gltf_data_t *data, uint64_t I)
{
	return (*data->validated_skins)[I];
}

void add_opaque_node_prim(lv_gltf_data_t *data, size_t index,
			  fastgltf::Node *node, int32_t P)
{
	(*data->opaque_nodes_by_materialIndex)[index].push_back(
		std::make_pair(node, std::as_const(P)));
}
MaterialIndexMap::iterator get_opaque_begin(lv_gltf_data_t *data)
{
	return data->opaque_nodes_by_materialIndex->begin();
}
MaterialIndexMap::iterator get_opaque_end(lv_gltf_data_t *data)
{
	return data->opaque_nodes_by_materialIndex->end();
}

void add_blended_node_prim(lv_gltf_data_t *data, size_t index,
			   fastgltf::Node *node, int32_t P)
{
	(*data->blended_nodes_by_materialIndex)[index].push_back(
		std::make_pair(node, std::as_const(P)));
}
MaterialIndexMap::iterator get_blended_begin(lv_gltf_data_t *data)
{
	return data->blended_nodes_by_materialIndex->begin();
}
MaterialIndexMap::iterator get_blended_end(lv_gltf_data_t *data)
{
	return data->blended_nodes_by_materialIndex->end();
}

void clear_distance_sort(lv_gltf_data_t *data)
{
	data->distance_sort_nodes->clear();
}

void add_distance_sort_prim(lv_gltf_data_t *data, NodeIndexDistancePair P)
{
	data->distance_sort_nodes->push_back(P);
}
NodeDistanceVector::iterator get_distance_sort_begin(lv_gltf_data_t *data)
{
	return data->distance_sort_nodes->begin();
}
NodeDistanceVector::iterator get_distance_sort_end(lv_gltf_data_t *data)
{
	return data->distance_sort_nodes->end();
}

gl_renwin_shaderset_t *get_shader_set(lv_gltf_data_t *data, _UINT I)
{
	return &((*data->shaderSets)[I]);
}

fastgltf::math::fvec3 get_cached_centerpoint(lv_gltf_data_t *data, _UINT I,
					     int32_t P, _MAT4 M)
{
	fastgltf::math::fvec4 tv = fastgltf::math::fvec4(
		(*data->local_mesh_to_center_points_by_primitive)[I][P]);
	tv[3] = 1.f;
	tv = M * tv;
	return fastgltf::math::fvec3(tv[0], tv[1], tv[2]);
}

void set_shader(lv_gltf_data_t *data, uint64_t _index, UniformLocs _uniforms,
		gl_renwin_shaderset_t _shaderset)
{
	(*data->shaderUniforms)[_index] = _uniforms;
	(*data->shaderSets)[_index] = _shaderset;
}

void init_shaders(lv_gltf_data_t *data, uint64_t _max_index)
{
	auto _prevsize = data->shaderSets->size();
	data->shaderSets->resize(_max_index + 1);
	data->shaderUniforms->resize(_max_index + 1);
	if (_prevsize < _max_index) {
		for (uint64_t _ii = _prevsize; _ii <= _max_index; _ii++) {
			(*data->shaderSets)[_ii] = gl_renwin_shaderset_t();
			(*data->shaderSets)[_ii].ready = false;
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
