clear
make -C build -j$(nproc) lvgl_workspace
if [ $? == 0 ]
then
    ./build/lvgl_workspace ./gltfs/torusknot.gltf -aa 1 -env 7 -bg 0 -spin_rate 30 -yaw 470 -pitch 240 -distance 840
fi
