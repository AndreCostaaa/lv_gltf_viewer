cmake_minimum_required(VERSION 3.25.1)
project(gltf_view LANGUAGES C CXX)

# Set policy to allow to run the target_link_libraries cmd on targets that are
# build in another directory. Currently, the linking is not handled by
# env_support/cmake/os.cmake This means that if a driver is enabled and it
# requires linking an external library it needs to be handled in the top-level
# project.
cmake_policy(SET CMP0079 NEW)

include(FetchContent)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
pkg_check_modules(EVDEV REQUIRED libevdev)

FetchContent_Declare(
  fastgltf
  GIT_REPOSITORY https://github.com/spnda/fastgltf
  GIT_TAG 4e2261350888bae7c35a1f39991f6233d57795f5)

set(FASTGLTF_ENABLE_DEPRECATED_EXT
    ON
    CACHE BOOL "" FORCE)

set(FASTGLTF_DIFFUSE_TRANSMISSION_SUPPORT
    ON
    CACHE BOOL "" FORCE)

set(FASTGLTF_DIFFUSE_TRANSMISSION_SUPPORT
    ON
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(fastgltf)

FetchContent_Declare(
  webp
  GIT_REPOSITORY https://github.com/webmproject/libwebp
  GIT_TAG fa6f56496a442eed59b103250021e4b14ebf1427)

FetchContent_MakeAvailable(webp)

list(APPEND PKG_CONFIG_LIB)
list(APPEND PKG_CONFIG_INC ${EVDEV_INCLUDE_DIRS})
list(APPEND LV_LINUX_BACKEND_SRC src/lib/indev_backends/evdev.c)

# Compiler settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -ggdb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ggdb")
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

add_executable(demo src/main.c)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_definitions(-DNDEBUG)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  add_definitions(-DNDEBUG)
endif()

option(ENABLE_ASAN "Enable AddressSanitizer in Debug builds" OFF)

if(ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(demo PRIVATE -fsanitize=address)
  target_link_options(demo PRIVATE -fsanitize=address)
endif()

# Additional optimization for MinSizeRel
if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  target_link_options(demo PRIVATE -Wl,--gc-sections)
endif()

target_link_libraries(lvgl PUBLIC webp fastgltf ${EVDEV_LIBRARIES})
target_link_libraries(demo PUBLIC lvgl m OpenGL::GL GLEW::GLEW glfw
                                  ${EVDEV_LIBRARIES})
set_target_properties(demo PROPERTIES COMPILE_DEFINITIONS
                                      "${LVGL_COMPILER_DEFINES}")
