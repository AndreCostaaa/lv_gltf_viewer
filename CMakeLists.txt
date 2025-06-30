cmake_minimum_required(VERSION 3.25.1)
project(gltf_view LANGUAGES C CXX)

# Set policy to allow to run the target_link_libraries cmd on targets that are
# build in another directory. Currently, the linking is not handled by
# env_support/cmake/os.cmake This means that if a driver is enabled and it
# requires linking an external library it needs to be handled in the top-level
# project.
cmake_policy(SET CMP0079 NEW)

include(FetchContent)

# Compiler settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wshadow -Wpedantic -ggdb")
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wpedantic -flax-vector-conversions"
)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(WebP REQUIRED)

set(GLFW_TARGET)
set(GLEW_TARGET)
set(WEBP_TARGET)

FetchContent_Declare(
  fastgltf
  GIT_REPOSITORY https://github.com/spnda/fastgltf.git
  GIT_TAG v0.8.0
  GIT_SHALLOW TRUE)

set(FASTGLTF_COMPILE_AS_CPP20
    OFF
    CACHE BOOL "" FORCE)
set(FASTGLTF_ENABLE_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(FASTGLTF_ENABLE_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
set(FASTGLTF_ENABLE_DOCS
    OFF
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(fastgltf)

set(CONFIG_LV_BUILD_EXAMPLES
    OFF
    CACHE BOOL "disable lvgl examples" FORCE)
set(CONFIG_LV_BUILD_DEMOS
    OFF
    CACHE BOOL "disable lvgl demos" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL
    OFF
    CACHE BOOL "disable thorvg internal" FORCE)

add_subdirectory(lvgl)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)

# Define common include directories
set(LVGL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/lvgl ${CMAKE_SOURCE_DIR}/lvgl/src)

set(PROTO_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/lvgl_proto/src/others
                       ${CMAKE_SOURCE_DIR}/lvgl_proto/src/others/gltf/data/deps)

# Create library targets
add_library(
  lvgl_opengl_shader_cache
  lvgl_proto/src/others/opengl_shader_cache/lv_opengl_shader_cache.cpp)
target_include_directories(lvgl_opengl_shader_cache
                           PRIVATE ${LVGL_INCLUDE_DIRS})

add_library(
  lvgl_gltfdata
  lvgl_proto/src/others/gltf/data/lv_gltf_data.c
  lvgl_proto/src/others/gltf/data/lv_gltf_data.cpp
  lvgl_proto/src/others/gltf/data/lv_gltf_override.cpp)
target_include_directories(lvgl_gltfdata PRIVATE ${LVGL_INCLUDE_DIRS}
                                                 ${PROTO_INCLUDE_DIRS})
target_link_libraries(lvgl_gltfdata PRIVATE lvgl_opengl_shader_cache fastgltf)
target_compile_definitions(lvgl_gltfdata
                           PRIVATE FASTGLTF_ENABLE_DEPRECATED_EXT=1)

add_library(lvgl_gltfview lvgl_proto/src/others/gltf/view/lv_gltf_view.c
                          lvgl_proto/src/others/gltf/view/lv_gltf_view.cpp)
target_include_directories(lvgl_gltfview PRIVATE ${LVGL_INCLUDE_DIRS}
                                                 ${PROTO_INCLUDE_DIRS})
target_link_libraries(lvgl_gltfdata PRIVATE lvgl_opengl_shader_cache fastgltf
                                            ${WEBP_TARGET})
target_link_libraries(lvgl_gltfview PRIVATE lvgl_gltfdata)

# Main executable
add_executable(
  gltf_view
  demo.c
  demo_ui.c
  demo_cli.c
  demo_nav.c
  demo_os_integrate.c
  demo_file_load_dialog.c
  demo_misc_utils.c
  resources/mouse_cursor_icon.c
  resources/lvgl_icon_40px_ARGB888.c
  resources/sprites1_32x32x7.c)

# Build type definitions
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_definitions(-DNDEBUG)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  add_definitions(-DNDEBUG)
endif()

# Optional: Enable AddressSanitizer for Debug builds (can be disabled if
# problematic)
option(ENABLE_ASAN "Enable AddressSanitizer in Debug builds" OFF)

if(ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(gltf_view PRIVATE -fsanitize=address)
  target_link_options(gltf_view PRIVATE -fsanitize=address)
endif()

# Additional optimization for MinSizeRel
if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  target_link_options(gltf_view PRIVATE -Wl,--gc-sections)
endif()

target_link_libraries(
  gltf_view
  PRIVATE lvgl
          lvgl_gltfview
          m
          OpenGL::GL
          GLEW::GLEW
          glfw
          WebP::webp)

target_include_directories(
  gltf_view PRIVATE lvgl_proto/src/others lvgl lvgl/src
                    ./lvgl_proto/src/others/gltf/data/deps)

set_target_properties(gltf_view PROPERTIES COMPILE_DEFINITIONS
                                           "${LVGL_COMPILER_DEFINES}")
