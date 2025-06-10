git submodule update --init
cd ./lib/fastgltf
cmake .
cmake --build . --target clean
cmake --build .
sudo make install