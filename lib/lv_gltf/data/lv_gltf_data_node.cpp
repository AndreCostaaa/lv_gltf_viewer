/**
 * @file lv_gltf_data_node.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data_internal.hpp"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_gltf_data_allocate_index(lv_gltf_data_t * data, size_t index)
{
    data->node_by_index.resize(index);
}

void set_node_at_path(lv_gltf_data_t * data, const std::string & path,
                      fastgltf::Node * node)
{
    data->node_by_path[path] = node;
}
void set_node_at_ip(lv_gltf_data_t * data, const std::string & ip,
                    fastgltf::Node * node)
{
    data->node_by_ip[ip] = node;
}
void set_node_index(lv_gltf_data_t * data, size_t index, fastgltf::Node * node)
{
    data->node_by_index[index] = node;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
