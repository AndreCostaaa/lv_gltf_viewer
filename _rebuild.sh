clear
make -C build -j$(nproc) lvgl_workspace
if [ $? == 0 ]
then
    ./build/lvgl_workspace ./gltfs/torusknot.gltf -aa 2 -env 7 -bg 0 -spin_rate 60 -yaw 0 -pitch 0 -distance 900
fi
