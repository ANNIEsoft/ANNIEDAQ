#!/bin/bash

mkdir ToolDAQ
cd ToolDAQ

git clone https://github.com/ToolDAQ/ToolDAQFramework.git
git clone https://github.com/ToolDAQ/zeromq-4.0.7.git
#wget http://downloads.sourceforge.net/project/boost/boost/1.60.0/boost_1_60_0.tar.gz
#tar zxvf boost_1_60_0.tar.gz

cd zeromq-4.0.7

./configure --prefix=`pwd`
make
make install

export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH

cd ..

 git clone https://github.com/ToolDAQ/boost_1_66_0.git
    #wget http://downloads.sourceforge.net/project/boost/boost/1.66.0/boost_1_66_0.tar.gz
    
    #tar zxf boost_1_66_0.tar.gz
    #rm -rf boost_1_66_0.tar.gz
     
    cd boost_1_66_0
    
    mkdir install
    
    ./bootstrap.sh --prefix=`pwd`/install/  > /dev/null 2>/dev/null
    ./b2 install iostreams

cd ../ToolDAQFramework

make clean
make

cd ../../

#wget https://root.cern.ch/download/root_v5.34.34.source.tar.gz
#tar zxvf root_v5.34.34.source.tar.gz
#cd root

#sudo yum install git make gcc-c++ gcc binutils libX11-devel libXpm-devel libXft-devel libXext-devel

#./configure --prefix=`pwd` --enable-rpath
#make
#make install


#sudo yum install postgresql-devel.x86_64


#http://pqxx.org/download/software/libpqxx/libpqxx-4.0.1.tar.gz
#tar zxvf libpqxx-4.0.1.tar.gz
#cd libpqxx-4.0.1
#./configure --enable-shared
#make
#sudo amke install
