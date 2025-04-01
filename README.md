# `lv_example_3dtexture`

Example usage of the LVGL `3dtexture` widget.

![example running screenclip](readme_image.png)

Ensure [GLFW is installed](https://docs.lvgl.io/master/details/integration/driver/opengles.html) for this example.

```shell
git submodule update --init
cmake -B build -S .
make -C build -j$(nproc) lvgl_workspace
./build/lvgl_workspace
```

For demonstration purposes, there is a very simple
glTF loader implemented in `gltf_loader.c`. It can only
handle very simple glTFs with one mesh described by
a `float` triangle vertex array and a `uint16_t` index array.
Mesh textures are ignored.
