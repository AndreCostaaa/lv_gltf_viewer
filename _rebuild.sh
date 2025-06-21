clear
make -C build -j$(nproc) gltf_view
if [ $? == 0 ]
then
    ./build/gltf_view ./gltfs/torusknot.gltf -aa 1 -env 8 -bg 0 -spin_rate 30 -yaw 470 -pitch 2400 -distance 840
fi
