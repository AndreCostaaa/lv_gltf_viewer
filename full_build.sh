cmake -B build -S .
make -C build -j$(nproc) lvgl_workspace
./build/lvgl_workspace
