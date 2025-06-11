cmake_minimum_required(VERSION 3.25.1)
# set(CMAKE_CXX_STANDARD 11)  # or 14, 17, etc.
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(lvgl_workspace LANGUAGES C CXX)

set(CONFIG_LV_BUILD_EXAMPLES 1)
set(CONFIG_LV_BUILD_DEMOS 1)
set(FASTGLTF_ENABLE_DEPRECATED_EXT 1)
# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)

# LVGL
include(${CMAKE_CURRENT_LIST_DIR}/lvgl/CMakeLists.txt)
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
)

add_library(lvgl_gltfview 
    gltfview/lv_gltfview.c
    gltfview/lv_gltfview_render.cpp
    gltfview/lv_gltfview_shader_includes.cpp
    gltfview/lv_gltfview_ibl_sampler.cpp
    gltfview/lv_gltfview_shader_cache.cpp
    gltfview/lv_gltfview_internal_datatypes.cpp
)

add_executable(lvgl_workspace
    demo.c
    demo_ui.c
    demo_cli.c
    demo_nav.c
    demo_os_integrate.c
    mouse_cursor_icon.c
    lvgl_icon_40px_ARGB888.c
    sprites1_32x32x7.c
)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -fsanitize=address -Wno-unused-parameter -Wunused-parameter -Wno-unknown-pragmas -Wshadow")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -fsanitize=address -Wno-unused-parameter -Wno-unknown-pragmas -std=c++17")
set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address -Wno-unused-parameter -Wno-unknown-pragmas")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/fastgltf)
target_compile_options(fastgltf PRIVATE -Wno-maybe-uninitialized)
target_compile_options(fastgltf PRIVATE -Wno-redundant-move)

target_link_libraries(lvgl_workspace fastgltf)
target_link_libraries(lvgl_gltfview fastgltf)
target_link_libraries(lvgl_gltfview webp)
target_compile_options(lvgl_gltfview PRIVATE -Wno-maybe-uninitialized)
target_compile_options(lvgl_gltfview PRIVATE -Wno-array-bounds)

target_link_libraries(lvgl_workspace lvgl_gltfview)
target_link_libraries(lvgl_workspace lvgl)
target_link_libraries(lvgl_workspace pthread)
target_link_libraries(lvgl_workspace m)
target_link_libraries(lvgl_workspace GL)
target_link_libraries(lvgl_workspace GLEW)
target_link_libraries(lvgl_workspace glfw)