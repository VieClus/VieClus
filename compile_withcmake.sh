#!/bin/bash

NCORES=4
unamestr=`uname`
if [[ "$unamestr" == "Linux" ]]; then
        NCORES=`grep -c ^processor /proc/cpuinfo`
fi

if [[ "$unamestr" == "Darwin" ]]; then
        NCORES=`sysctl -n hw.ncpu`
fi

rm -rf deploy
rm -rf build
mkdir build
cd build
if [[ "$1" == "NOMPI" ]]; then
        cmake -DNOMPI=ON ../
else
        cmake ../
fi
make -j $NCORES
cd ..

mkdir deploy

cp ./build/evolutionary_clustering deploy/vieclus
cp ./build/evaluator deploy/
cp ./build/graphchecker deploy/

