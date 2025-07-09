#include <misc/lv_color.h>
#include <stdlib/lv_mem.h>

#include <GL/glew.h>
#include <drivers/glfw/lv_opengles_debug.h> /* GL_CALL */

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include "../lv_gltf_data_internal.hpp"

fastgltf::math::fvec3 get_cached_centerpoint(lv_gltf_data_t * D, _UINT I,
                                             int32_t P, _MAT4 M);

fastgltf::math::fvec4 lv_gltf_get_primitive_centerpoint(lv_gltf_data_t * data,
                                                        fastgltf::Mesh & mesh,
                                                        uint32_t prim_num)
{
    fastgltf::math::fvec4 result{ 0.f };
    fastgltf::math::fvec3 v_min{ 999999999.f };
    fastgltf::math::fvec3 v_max{ -999999999.f };
    fastgltf::math::fvec3 v_cen{ 0.f };
    float radius = 0.f;

    if(mesh.primitives.size() <= prim_num) {
        return result;
    }
    const auto & it = mesh.primitives[prim_num];
    const auto & asset = data->asset;

    const auto * positionIt = it.findAttribute("POSITION");
    const auto & positionAccessor =
        asset.accessors[positionIt->accessorIndex];
    if(!positionAccessor.bufferViewIndex.has_value()) {
        return result;
    }

    if(!(positionAccessor.min.has_value() &&
         positionAccessor.max.has_value())) {
        LV_LOG_ERROR(
            "Could not get primitive center point. Missing min/max values");
        return result;
    }

    fastgltf::math::fvec4 t_min{
        (float)(positionAccessor.min.value().get<double>((size_t)0)),
        (float)(positionAccessor.min.value().get<double>((size_t)1)),
        (float)(positionAccessor.min.value().get<double>((size_t)2)),
        0.f
    };
    fastgltf::math::fvec4 t_max{
        (float)(positionAccessor.max.value().get<double>((size_t)0)),
        (float)(positionAccessor.max.value().get<double>((size_t)1)),
        (float)(positionAccessor.max.value().get<double>((size_t)2)),
        0.f
    };

    v_max[0] = LV_MAX(t_min.x(), t_max.x());
    v_max[1] = LV_MAX(t_min.y(), t_max.y());
    v_max[2] = LV_MAX(t_min.z(), t_max.z());
    v_min[0] = LV_MIN(t_min.x(), t_max.x());
    v_min[1] = LV_MIN(t_min.y(), t_max.y());
    v_min[2] = LV_MIN(t_min.z(), t_max.z());
    v_cen[0] = (v_max[0] + v_min[0]) / 2.0f;
    v_cen[1] = (v_max[1] + v_min[1]) / 2.0f;
    v_cen[2] = (v_max[2] + v_min[2]) / 2.0f;
    float size_x = v_max[0] - v_min[0];
    float size_y = v_max[1] - v_min[1];
    float size_z = v_max[2] - v_min[2];
    radius = std::sqrt((size_x * size_x) + (size_y * size_y) +
                       (size_z * size_z)) /
             2.0f;
    result[0] = v_cen[0];
    result[1] = v_cen[1];
    result[2] = v_cen[2];
    result[3] = radius;
    return result;
}

fastgltf::math::fvec3 lv_gltf_get_centerpoint(lv_gltf_data_t * gltf_data,
                                              fastgltf::math::fmat4x4 matrix,
                                              uint32_t meshIndex, int32_t elem)
{
    if(!centerpoint_cache_contains(gltf_data, meshIndex, elem))
        recache_centerpoint(gltf_data, meshIndex, elem);
    return get_cached_centerpoint(gltf_data, meshIndex, elem, matrix);
}

bool lv_gltf_data_utils_get_texture_info(lv_gltf_data_t * data_obj,
                                         uint32_t model_texture_index,
                                         uint32_t mipmapnum, size_t * byte_count,
                                         uint32_t * width, uint32_t * height,
                                         bool * has_alpha)
{
    *byte_count = 0;
    if(model_texture_index >= data_obj->textures->size()) {
        return false;
    }
    uint32_t texid = (*data_obj->textures)[model_texture_index].texture;
    // Bind the texture
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texid));
    int32_t gl_color_format;
    GL_CALL(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
                                     GL_TEXTURE_INTERNAL_FORMAT,
                                     &gl_color_format));

    // Determine if the texture has an alpha channel
    *has_alpha = false;
    bool not_valid = false;

    switch(gl_color_format) {
        case GL_RGBA:
        case GL_BGRA:
        case GL_RGBA8:
            *has_alpha = true;
            break;
        case GL_RGB:
        case GL_BGR:
        case GL_RGB8:
            *has_alpha = false;
            break;
        default:
            LV_LOG_ERROR("Unhandled texture color format %d",
                         gl_color_format);
            not_valid = true;
            break;
    }
    // even if the pixel format is invalid, we can still get the width and height
    GLint texture_width;
    GLint texture_height;
    GL_CALL(glGetTexLevelParameteriv(GL_TEXTURE_2D, mipmapnum,
                                     GL_TEXTURE_WIDTH, &texture_width));
    GL_CALL(glGetTexLevelParameteriv(GL_TEXTURE_2D, mipmapnum,
                                     GL_TEXTURE_HEIGHT, &texture_height));
    *width = (uint32_t)(texture_width);
    *height = (uint32_t)(texture_height);

    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    if(not_valid) {
        return false;
    }

    *byte_count = texture_width * texture_height * (*has_alpha ? 4 : 3);
    return true;
}

bool lv_gltf_data_utils_get_texture_pixels(void * pixels,
                                           lv_gltf_data_t * data_obj,
                                           uint32_t model_texture_index,
                                           uint32_t mipmapnum, uint32_t width,
                                           uint32_t height, bool has_alpha)
{
    // This parameter is specified because WebGL can't read a texture's width from the GPU, however this isn't yet implemented so for now it either uses the GPU or it fails.
    LV_UNUSED(width);
    LV_UNUSED(height);

    if(model_texture_index >= data_obj->textures->size()) {
        return false;
    }
    uint32_t texid = (*data_obj->textures)[model_texture_index].texture;
    // Bind the texture
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texid));
    glGetTexImage(GL_TEXTURE_2D, mipmapnum, (has_alpha) ? GL_RGBA : GL_RGB,
                  GL_UNSIGNED_BYTE, pixels);
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    return true;
}

void lv_gltf_data_utils_swap_pixels_red_blue(void * pixels,
                                             size_t byte_total_count,
                                             bool has_alpha)
{
    char * pixel_buffer = (char *)pixels;
    size_t bytes_per_pixel = has_alpha ? 4 : 3;
    size_t pixel_count = (byte_total_count / bytes_per_pixel);
    if(bytes_per_pixel == 4) {
        for(size_t p = 0; p < pixel_count; p++) {
            size_t index = p << 2;
            uint8_t r = pixel_buffer[index + 0];
            uint8_t g = pixel_buffer[index + 1];
            uint8_t b = pixel_buffer[index + 2];
            uint8_t a = pixel_buffer[index + 3];
            pixel_buffer[index + 0] = b;
            pixel_buffer[index + 1] = g;
            pixel_buffer[index + 2] = r;
            pixel_buffer[index + 3] = a;
        }
    }
    else {
        for(size_t p = 0; p < pixel_count; p++) {
            size_t index = p * 3;
            uint8_t r = pixel_buffer[index + 0];
            uint8_t g = pixel_buffer[index + 1];
            uint8_t b = pixel_buffer[index + 2];
            pixel_buffer[index + 0] = b;
            pixel_buffer[index + 1] = g;
            pixel_buffer[index + 2] = r;
        }
    }
}

/* Caller becomes responsible for freeing data of the result if data_size > 0, but if a new_image_dsc is passed to this function with a data_size > 0, it will free it's data first. */
void lv_gltf_data_utils_texture_to_image_dsc(lv_image_dsc_t * new_image_dsc,
                                             lv_gltf_data_t * data_obj,
                                             uint32_t model_texture_index)
{
    size_t byte_total_count = 0;
    uint32_t source_pixel_width = 0;
    uint32_t source_pixel_height = 0;
    bool has_alpha = false;
    uint8_t * pixel_buffer;
    if(lv_gltf_data_utils_get_texture_info(
           data_obj, model_texture_index, 0, &byte_total_count,
           &source_pixel_width, &source_pixel_height, &has_alpha)) {
        pixel_buffer = (uint8_t *)lv_malloc(byte_total_count);
        if(lv_gltf_data_utils_get_texture_pixels(
               pixel_buffer, data_obj, model_texture_index, 0,
               source_pixel_width, source_pixel_height,
               has_alpha)) {
            if(pixel_buffer == NULL || byte_total_count == 0 ||
               source_pixel_width == 0)
                return;

            if(new_image_dsc->data_size > 0) {
                lv_free((uint8_t *)new_image_dsc->data);
                new_image_dsc->data = NULL;
                new_image_dsc->data_size = 0;
            }

            lv_gltf_data_utils_swap_pixels_red_blue(
                pixel_buffer, byte_total_count, has_alpha);
            size_t bytes_per_pixel = has_alpha ? 4 : 3;
            size_t pixel_count =
                (byte_total_count / bytes_per_pixel);

            new_image_dsc->data = (const uint8_t *)pixel_buffer;
            new_image_dsc->data_size = byte_total_count;
            new_image_dsc->header.w = source_pixel_width;
            new_image_dsc->header.h =
                (uint16_t)(pixel_count / source_pixel_width);
            new_image_dsc->header.cf =
                has_alpha ? LV_COLOR_FORMAT_ARGB8888 :
                LV_COLOR_FORMAT_RGB888;
        }
    }
}
