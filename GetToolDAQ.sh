#!/bin/bash

mkdir ToolDAQ
cd ToolDAQ

git clone https://github.com/ToolDAQ/ToolDAQFramework.git
git clone https://github.com/ToolDAQ/zeromq-4.0.7.git

cd zeromq-4.0.7

./configure --prefix=`pwd`
make
make install

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/lib


cd ../ToolDAQFramework

make clean
make

cd ../../