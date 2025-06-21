cmake_minimum_required(VERSION 3.25.1)
#set(CMAKE_CXX_STANDARD 11)  # or 14, 17, etc.
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(lvgl_workspace LANGUAGES C CXX)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CONFIG_LV_BUILD_EXAMPLES 0)
set(CONFIG_LV_BUILD_DEMOS 0)
set(FASTGLTF_ENABLE_DEPRECATED_EXT 1)
set(ASAN_OPTIONS alloc_dealloc_mismatch=0)
set(LVGL_ENABLE_WEBP_IMAGES 1)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
#set(CMAKE_PREFIX_PATH /usr/lib/aarch64-linux-gnu/cmake/glfw3)
#find_package(glfw3 REQUIRED)

# LVGL
include(${CMAKE_CURRENT_LIST_DIR}/lvgl/CMakeLists.txt)
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
)

add_library(lvgl_opengl_shader_cache 
    lvgl_proto/src/others/opengl_shader_cache/lv_opengl_shader_cache.cpp
)

add_library(lvgl_gltfdata 
    lvgl_proto/src/others/gltf/data/lv_gltf_data.c
    lvgl_proto/src/others/gltf/data/lv_gltf_data.cpp
    lvgl_proto/src/others/gltf/data/lv_gltf_override.cpp
)
# Set the compile definition for the target

add_library(lvgl_gltfview 
    lvgl_proto/src/others/gltf/view/lv_gltf_view.c
    lvgl_proto/src/others/gltf/view/lv_gltf_view.cpp
)

add_executable(lvgl_workspace
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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wshadow ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -flax-vector-conversions")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS}")

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
    target_link_libraries(lvgl_gltfdata webp)
    target_compile_definitions(lvgl_gltfdata PRIVATE LVGL_ENABLE_WEBP_IMAGES=${LVGL_ENABLE_WEBP_IMAGES})
endif()

target_link_libraries(lvgl_gltfview lvgl_gltfdata)

target_link_libraries(lvgl_workspace lvgl_gltfview)
target_link_libraries(lvgl_workspace lvgl)
target_link_libraries(lvgl_workspace pthread)
target_link_libraries(lvgl_workspace m)
target_link_libraries(lvgl_workspace GL)
target_link_libraries(lvgl_workspace GLEW)
target_link_libraries(lvgl_workspace glfw)

# These seem necessary in certain build targets like RELEASE, where compilation produces a large 
# flood of warnings within the standard library without them.  Eventually we'll get them isolated to 
# just one target so we can still catch helpful warnings elsewhere.

target_compile_options(lvgl_gltfdata PRIVATE -Wno-redundant-move)
target_compile_options(lvgl_gltfdata PRIVATE -Wno-unknown-pragmas)
target_compile_options(lvgl_gltfdata PRIVATE -Wno-unused-parameter)
target_compile_options(lvgl_gltfview PRIVATE -Wno-unknown-pragmas)
target_compile_options(lvgl_gltfview PRIVATE -Wno-maybe-uninitialized)
target_compile_options(lvgl_gltfview PRIVATE -Wno-array-bounds)
target_compile_options(lvgl_workspace PRIVATE -Wno-array-bounds)