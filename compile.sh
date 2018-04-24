#!/bin/bash
# Compiles the current program using scons


NCORES=4
unamestr=`uname`
if [[ "$unamestr" == "Linux" ]]; then
        NCORES=`grep -c ^processor /proc/cpuinfo`
fi

if [[ "$unamestr" == "Darwin" ]]; then
        NCORES=`sysctl -n hw.ncpu`
fi

rm -rf deploy
for program in evolutionary_clustering evaluator; do 
scons program=$program variant=optimized -j $NCORES 
if [ "$?" -ne "0" ]; then 
        echo "compile error in $program. exiting."
        exit
fi
done

cd extern/KaHIP/
for program in graphchecker ; do 
scons program=$program variant=optimized -j $NCORES 
if [ "$?" -ne "0" ]; then 
        echo "compile error in $program. exiting."
        exit
fi
done
cd ..
cd ..

rm -rf deploy
mkdir deploy

cp ./optimized/evolutionary_clustering deploy/vieclus
cp ./optimized/evaluator deploy/
cp ./extern/KaHIP/optimized/graphchecker deploy/
rm -rf optimized
