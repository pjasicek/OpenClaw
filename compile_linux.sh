#!/bin/bash
export NUMCPUS=`grep -c '^processor' /proc/cpuinfo`
rm -rf build
mkdir build
cd build
cmake ..
make -j$NUMCPUS
