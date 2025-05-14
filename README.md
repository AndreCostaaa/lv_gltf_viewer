# `lv_gltf_viewer`

A simple glTF file viewer, made using the LVGL `3dtexture` widget.

![example running screenclip one](readme_image.png)

Ensure [GLFW is installed](https://docs.lvgl.io/master/details/integration/driver/opengles.html) for this example.

prebuild.sh will initialize git submodules, then build and install fastgltf

```shell
./prebuild.sh
cmake -B build -S .
make -C build -j$(nproc) lvgl_workspace
./build/lvgl_workspace
```

For demonstration purposes, there is a very simple
glTF loader.  This is a proof of concept build and there
are many known issues.

---

This app requires a non-standard function within lvgl/src/drivers/glfw/lv_glfw_window.c / .h:

```c
void * lv_glfw_window_get_glfw_window(lv_glfw_window_t * window) {
    return (void*)(window->window);
}
```

I'll put in a PR to get that added in properly but if you encounter compilation errors you 
might need to patch that function in manually for now.

---


![example running screenclip two](screenshot_image1.png)


![example running screenclip three](screenshot_image2.png)