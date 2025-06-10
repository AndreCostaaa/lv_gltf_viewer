clear
make -C build -j$(nproc) lvgl_workspace
if [ $? == 0 ]
then
    ./build/lvgl_workspace ./gltfs/torusknot.gltf -aa 2 -env media/hdr/directional.jpg -bg 0
fi
