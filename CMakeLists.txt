cmake_minimum_required(VERSION 3.25.1)

project(lvgl_workspace LANGUAGES C CXX)

set(CONFIG_LV_BUILD_EXAMPLES 1)
set(CONFIG_LV_BUILD_DEMOS 1)

# LVGL
include(${CMAKE_CURRENT_LIST_DIR}/lvgl/CMakeLists.txt)
include_directories(${CMAKE_CURRENT_LIST_DIR})

# find_package(Freetype REQUIRED)
# include_directories(${FREETYPE_INCLUDE_DIRS})

add_executable(lvgl_workspace
    main.c
    mouse_cursor_icon.c
    gltf_loader.c
    gl_renwin.cpp
    gl_shader_includes.cpp
    lib/mathc/mathc.c
    ibl_sampler.cpp
    shader_cache.cpp
    lvgl_icon_40px_ARGB888.c
)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -fsanitize=address -Wno-unused-parameter -Wunused-parameter -Wshadow")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -fsanitize=address -Wno-unused-parameter")
set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address -Wno-unused-parameter")

target_link_libraries(lvgl_workspace lvgl)
target_link_libraries(lvgl_workspace pthread)
target_link_libraries(lvgl_workspace m)
# target_link_libraries(lvgl_workspace lvgl_thorvg)
# target_link_libraries(lvgl_workspace lvgl_examples)
# target_link_libraries(lvgl_workspace lvgl_demos)

target_link_libraries(lvgl_workspace GL)
target_link_libraries(lvgl_workspace GLEW)
target_link_libraries(lvgl_workspace glfw)
target_link_libraries(lvgl_workspace fastgltf)

# target_link_libraries(lvgl_workspace jpeg)
# target_link_libraries(lvgl_workspace png)
# target_link_libraries(lvgl_workspace ${FREETYPE_LIBRARIES})
