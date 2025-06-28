cmake_minimum_required(VERSION 3.25.1)
project(gltf_view LANGUAGES C CXX)

# Set policy to allow to run the target_link_libraries cmd on targets that are
# build in another directory. Currently, the linking is not handled by
# env_support/cmake/os.cmake This means that if a driver is enabled and it
# requires linking an external library it needs to be handled in the top-level
# project.
cmake_policy(SET CMP0079 NEW)


# Compiler settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wshadow -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wpedantic -flax-vector-conversions")

# LVGL build settings
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "disable lvgl examples" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "disable lvgl demos" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "disable thorvg internal" FORCE)

add_subdirectory(lvgl)

set(FASTGLTF_ENABLE_DEPRECATED_EXT ON)
set(LVGL_ENABLE_WEBP_IMAGES 1) # 0 = disable / 1 = enabled with internal decoder code / 2 = enabled with external lib webp

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)

add_library(lvgl_opengl_shader_cache 
    lvgl_proto/src/others/opengl_shader_cache/lv_opengl_shader_cache.cpp
)
target_include_directories(lvgl_opengl_shader_cache PRIVATE lvgl lvgl/src)

add_library(lvgl_gltfdata 
    lvgl_proto/src/others/gltf/data/lv_gltf_data.c
    lvgl_proto/src/others/gltf/data/lv_gltf_data.cpp
    lvgl_proto/src/others/gltf/data/lv_gltf_override.cpp
)

target_include_directories(lvgl_gltfdata PRIVATE lvgl lvgl/src ./lvgl_proto/src/others/gltf/data/deps)

add_library(lvgl_gltfview 
    lvgl_proto/src/others/gltf/view/lv_gltf_view.c
    lvgl_proto/src/others/gltf/view/lv_gltf_view.cpp
)

target_include_directories(lvgl_gltfview PRIVATE lvgl lvgl/src ./lvgl_proto/src/others/gltf/data/deps)

add_executable(gltf_view
    demo.c
    demo_ui.c
    demo_cli.c
    demo_nav.c
    demo_os_integrate.c
    demo_file_load_dialog.c
    demo_misc_utils.c
    resources/mouse_cursor_icon.c
    resources/lvgl_icon_40px_ARGB888.c
    resources/sprites1_32x32x7.c
)


if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DNDEBUG)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_definitions(-DNDEBUG)
endif()

# Set flags for Debug build
set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Wextra -fsanitize=address -Wshadow")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -fsanitize=address -std=c++17")
set(CMAKE_LINKER_FLAGS_DEBUG "-fsanitize=address")

# Set flags for Release build
set(CMAKE_C_FLAGS_RELEASE "-O3 -Wall -Wshadow")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -std=c++17")
set(CMAKE_LINKER_FLAGS_RELEASE "-O3")

#  Set flags for MinSizeRel build
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -Wall -Wshadow")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -Wall -std=c++17")
set(CMAKE_LINKER_FLAGS_MINSIZEREL "-Wl,--gc-sections")  # Remove unused sections

target_link_libraries(lvgl_gltfdata lvgl_opengl_shader_cache)
if(DEFINED LVGL_ENABLE_WEBP_IMAGES AND LVGL_ENABLE_WEBP_IMAGES)
    #target_link_libraries(lvgl_gltfdata webp)
    if (LVGL_ENABLE_WEBP_IMAGES EQUAL 1)
        add_library(lvgl_webpmin
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dec/alpha_dec.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dec/buffer_dec.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dec/frame_dec.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dec/io_dec.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dec/quant_dec.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dec/tree_dec.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dec/vp8_dec.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dec/vp8l_dec.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dec/webp_dec.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/utils/bit_reader_utils.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/utils/color_cache_utils.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/utils/huffman_utils.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/utils/palette.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/utils/quant_levels_dec_utils.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/utils/random_utils.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/utils/rescaler_utils.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/utils/thread_utils.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/utils/utils.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/alpha_processing.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/alpha_processing_neon.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/cpu.c        
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/dec.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/dec_neon.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/dec_clip_tables.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/filters.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/filters_neon.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/lossless.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/lossless_neon.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/rescaler.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/rescaler_neon.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/upsampling.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/upsampling_neon.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/yuv.c
            lvgl_proto/src/others/gltf/data/deps/webp_min/src/dsp/yuv_neon.c
        )
        target_include_directories(lvgl_webpmin PRIVATE 
            ${CMAKE_SOURCE_DIR}/lvgl_proto/src/others/gltf/data/deps/webp_min
        )
        target_link_libraries(lvgl_gltfdata lvgl_webpmin)
    endif()
    if (LVGL_ENABLE_WEBP_IMAGES EQUAL 2) 
        target_link_libraries(lvgl_gltfdata webp)
    endif()
    target_compile_definitions(lvgl_gltfdata PRIVATE LVGL_ENABLE_WEBP_IMAGES=${LVGL_ENABLE_WEBP_IMAGES})
endif()

target_include_directories(
  lvgl_gltfdata
  PRIVATE
    ${CMAKE_SOURCE_DIR}/lvgl_proto/src/others/gltf/data/deps/fastgltf/include/)
target_link_libraries(lvgl_gltfview lvgl_gltfdata)

target_link_libraries(gltf_view PRIVATE lvgl lvgl_gltfview pthread m GL GLEW glfw webp)
target_include_directories(gltf_view PRIVATE lvgl_proto/src/others lvgl lvgl/src ./lvgl_proto/src/others/gltf/data/deps)

set_target_properties(gltf_view PROPERTIES COMPILE_DEFINITIONS
                                            "${LVGL_COMPILER_DEFINES}")


