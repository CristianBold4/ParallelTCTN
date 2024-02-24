#!/bin/bash

cd code
# --  compile ankerl
cd unordered_dense
rm -rf build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=./unordered_dense_install ..
cmake --build . --target install

# -- 
cd ../..
rm -rf build
mkdir build
cd build
#cmake -DCMAKE_PREFIX_PATH=../unordered_dense/build/unordered_dense_install ..
cmake ..
make
