#!/bin/bash

# rpms prerequisits needs root
#yum install make gcc-c++ gcc binutils libX11-devel libXpm-devel libXft-devel libXext-devel postgresql-devel.x86_64 emacs screennc firefox git screen


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

mkdir install

./bootstrap.sh --prefix=`pwd`/install/ 
./b2 install 

export LD_LIBRARY_PATH=`pwd`/install/lib:$LD_LIBRARY_PATH

cd ../ToolDAQFramework

make clean
make

export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH

cd ../

wget https://root.cern.ch/download/root_v5.34.34.source.tar.gz
tar zxvf root_v5.34.34.source.tar.gz
cd root

#sudo yum install git make gcc-c++ gcc binutils libX11-devel libXpm-devel libXft-devel libXext-devel

./configure --prefix=`pwd` --enable-rpath
make
make install

cd ../
#sudo yum install postgresql-devel.x86_64


wget http://pqxx.org/download/software/libpqxx/libpqxx-4.0.1.tar.gz
tar zxvf libpqxx-4.0.1.tar.gz
cd libpqxx-4.0.1
mkdir install
./configure --enable-shared --prefix=`pwd`/install
make
make install

cd ../../

make clean
make

#as root
#route add -net 224.0.0.0 netmask 240.0.0.0 dev eth1

#webpage
#cp /home/annie/ANNIEDAQ/Webpage/*.cgi /var/www/cgi-bin/
#cp /home/annie/ANNIEDAQ/Webpage/*.cpp /var/www/cgi-bin
#cp /home/annie/ANNIEDAQ/Webpage/Makefile /var/www/cgi-bin
#cp /home/annie/ANNIEDAQ/Webpage/*.html /var/www/html/
#cp /home/annie/ANNIEDAQ/Webpage/*.png /var/www/html/
#cp /home/annie/ANNIEDAQ/Webpage/*.JPG /var/www/html/
#cp /home/annie/ANNIEDAQ/Webpage/styles.css /var/www/html/
mkdir /data/logs
mkdir /data/output
cd /va/www/html/
ln -s /data/logs/ ./
ln -s /data/output/ ./
chown -R annie:apache /data/

cd /home/annie/ANNIEDAQ/ToolDAQ/
wget ftp://ftp.gnu.org/gnu/cgicc/cgicc-3.2.9.tar.gz
tar -zxvf cgicc-3.2.9.tar.gz
cd cgicc-3.2.9
./configure --prefix=/usr/local
make
make install

cd ../boost_1_60_0
cp -r install/include/* /usr/local/include/
cp -r install/lib/* /usr/local/lib/

cd ../zeromq-4.0.7/
cp -r include/* /usr/local/include/
cp -r lib/* /usr/local/lib/

cd ../../
cp -r include/* /usr/local/include/
cp -r lib/* /usr/local/lib/

cd /var/www/cgi-bin
make

cp /home/annie/ANNIEDAQ/rc.local/rc.local /etc/
cp /home/annie/ANNIEDAQ/rc.local/ANNIEcgi.conf /etc/ld.so.conf.d/
ldconfig