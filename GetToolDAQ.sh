0;136;0c#!/bin/bash

mkdir ToolDAQ
cd ToolDAQ

git clone https://github.com/ToolDAQ/ToolDAQFramework.git
git clone https://github.com/ToolDAQ/zeromq-4.0.7.git
wget http://downloads.sourceforge.net/project/boost/boost/1.60.0/boost_1_60_0.tar.gz
tar zxvf boost_1_60_0.tar.gz

cd zeromq-4.0.7

./configure --prefix=`pwd`
make
make install

export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH

cd ../boost_1_60_0

./bootstrap.sh --prefix=/usr/local/ # change this to pwd
sudo ./b2 install  # then no need for sudo

export LD_LIBRARY_PATH=`pwd`/libs:$LD_LIBRARY_PATH

cd ../ToolDAQFramework

make clean
make

cd ../../

wget https://root.cern.ch/download/root_v5.34.34.source.tar.gz
tar zxvf root_v5.34.34.source.tar.gz
cd root

sudo yum install git make gcc-c++ gcc binutils libX11-devel libXpm-devel libXft-devel libXext-devel

./configure --prefix=`pwd` --enable-rpath
make
make install


sudo yum install postgresql-devel.x86_64


http://pqxx.org/download/software/libpqxx/libpqxx-4.0.1.tar.gz
tar zxvf libpqxx-4.0.1.tar.gz
cd libpqxx-4.0.1
./configure --enable-shared
make
sudo amke install
