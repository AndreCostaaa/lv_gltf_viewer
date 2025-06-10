#ifndef LV_GLTFANIM_H
#define LV_GLTFANIM_H

#ifdef __cplusplus
extern "C" {
#endif

FVEC3 __get_animated_vec3_at_timestamp(pGltf_data_t _data, uint32_t _animNum, fastgltf::AnimationSampler * sampler, float _seconds);
fastgltf::math::fquat __get_animated_quat_at_timestamp(pGltf_data_t _data, uint32_t _animNum, fastgltf::AnimationSampler * sampler, float _seconds);
//std::vector<uint32_t> * __get_channel_set(std::size_t anim_num, pGltf_data_t gltf_data,  fastgltf::Node& node);
void animateMatrix(float timestamp, std::size_t anim_num, pGltf_data_t gltf_data,  fastgltf::Node& node, FMAT4& matrix);

float __get_animation_total_time(pGltf_data_t _data, uint32_t _animNum);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTFANIM_H*/
