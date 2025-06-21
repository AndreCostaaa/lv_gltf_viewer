#!/bin/bash

find ./lvgl_proto/ -type f -exec wc -l {} + | awk '{total += $1} END {print total}'
#find ./gltfview/lv_gltfview_shader_includes.h -type f -exec wc -l {} + | awk '{total += $1} END {print total}'
#find ./gltfview/lv_gltfview_shader_v1.h -type f -exec wc -l {} + | awk '{total += $1} END {print total}'
