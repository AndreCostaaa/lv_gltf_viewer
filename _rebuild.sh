clear
make -C build -j$(nproc) lvgl_workspace
if [ $? == 0 ]
then
    ./build/lvgl_workspace ../gltfs/webp_abduction_of_polyxena_alt.glb -aa 2 -env media/hdr/helipad.jpg -bg 2
fi