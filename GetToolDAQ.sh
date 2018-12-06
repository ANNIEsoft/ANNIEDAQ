#!/bin/bash

<<<<<<< HEAD
# rpms prerequisits needs root
#yum install make gcc-c++ gcc binutils libX11-devel libXpm-devel libXft-devel libXext-devel postgresql-devel.x86_64 emacs screennc firefox git screen libcurl-devel libusb-devel.x86_64


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

ln -s configfiles/LoggerToolChain ./Logger


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
#mkdir /data/logs
#mkdir /data/output
#mkdir /data/monitoring
#cd /va/www/html/
#ln -s /data/logs/ ./
#ln -s /data/output/ ./
#ln -s /data/monitoring/ ./
#chown -R annie:apache /data/

#cd /home/annie/ANNIEDAQ/ToolDAQ/
#wget ftp://ftp.gnu.org/gnu/cgicc/cgicc-3.2.9.tar.gz
#tar -zxvf cgicc-3.2.9.tar.gz
#cd cgicc-3.2.9
#./configure --prefix=/usr/local
#make
#make install

#cd ../boost_1_60_0
#cp -r install/include/* /usr/local/include/
#cp -r install/lib/* /usr/local/lib/

#cd ../zeromq-4.0.7/
#cp -r include/* /usr/local/include/
#cp -r lib/* /usr/local/lib/

#cd ../../
#cp -r include/* /usr/local/include/
#cp -r lib/* /usr/local/lib/

#cd /var/www/cgi-bin
#make

#cp /home/annie/ANNIEDAQ/setup/rc.local /etc/
#cp /home/annie/ANNIEDAQ/setup/ANNIEcgi.conf /etc/ld.so.conf.d/
#cp /home/annie/ANNIEDAQ/setup/ANNIE.conf /etc/httpd/conf.d/
###edit the daq name to correct one
#ldconfig
#service httpd restart

##postgresql
## as root
chown -R products:annie /local
chmod -R g+w /local

## as annie
source /local/ups/etc/setups.sh 
setup upd
upd install postgres v9_3_9_x64 -f Linux+2.6 -z /local/ups/db
ups declare -c postgres v9_3_9_x64
ups tailor postgres
  > /data/postgres
  > annie
  >
  > 
  >
## no longer needed chgrp  postgres /data/postgres/*
cp /home/annie/ANNIEDAQ/setup/postgresql.conf /data/postgres/
cp /home/annie/ANNIEDAQ/setup/pg_hba.conf /data/postgres/
/local/ups/prd/postgres/v9_3_9_x64/Linux-2-6/bin/postgres --config-file=/data/postgres/postgresql.conf &

##setting up dbs
psql postgres
  > create database annie;
  > \c annie
  > create table runinformation(ID bigserial primary key, runnumber bigint, subrunnumber bigint, starttime timestamp without time zone, stoptime timestamp without time zone, runtype integer, runstatus integer, numevents bigint);
 > create role root;
 > grant all privileges on runinformation to root;
 > grant usage, select on sequence runinformation_id_seq to root;
 > grant usage, update on sequence runinformation_id_seq to root;
 > create role apache;
 > alter role apache with login;
 > grant all privileges on runinformation to apache;
 > \q

##
=======
init=1
tooldaq=1
boostflag=1
zmq=1
final=1
rootflag=0

while [ ! $# -eq 0 ]
do
    case "$1" in
	--help | -h)
	    helpmenu
	    exit
	    ;;

	--with_root | -r)
	    echo "Installing ToolDAQ with root"
	    rootflag=1 
	    ;;
	
	--no_boost | -b)
            echo "Installing ToolDAQ without boost"
            boostflag=0
	    ;;
	
	--no_init )
	     echo "Installing ToolDAQ without creating ToolDAQ Folder"
	    init=0;
	    ;;

	--no_zmq )
            echo "Installing ToolDAQ without zmq"
            zmq=0
            ;;

	--no_tooldaq )
	    echo "Installing dependancies without ToolDAQ"
	    tooldaq=0
	    ;;

	--no_final )
            echo "Installing ToolDAQ without compiling ToolAnalysis"
            final=0
            ;;

	--ToolDAQ_ZMQ )
            echo "Installing ToolDAQ & ZMQ"
	    boostflag=0
	    rootflag=0
	    final=0
            ;;

	--Boost )
            echo "Installing Boost"
	    init=0
	    tooldaq=0
	    zmq=0
	    final=0
	    rootflag=0
            ;;

	--Root )
            echo "Installing Root"
	    init=0
	    tooldaq=0
	    boostflag=0
	    zmq=0
	    final=0
            ;;
	
	
	--Final )
            echo "Compiling ToolDAQ"
	    init=0
	    tooldaq=0
	    boostflag=0
	    rootflag=0
	    zmq=0
            ;;

    esac
    shift
done

if [ $init -eq 1 ]
then
    
    mkdir ToolDAQ
fi

cd ToolDAQ

if [ $tooldaq -eq 1 ]
then

git clone https://github.com/ToolDAQ/ToolDAQFramework.git
fi

if [ $zmq -eq 1 ]
then
    git clone https://github.com/ToolDAQ/zeromq-4.0.7.git
    
    cd zeromq-4.0.7
    
    ./configure --prefix=`pwd`
    make -j8
    make install
    
    export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH
    
    cd ../
fi

if [ $boostflag -eq 1 ]
then
    
    git clone https://github.com/ToolDAQ/boost_1_66_0.git
    #wget http://downloads.sourceforge.net/project/boost/boost/1.66.0/boost_1_66_0.tar.gz
    
    #tar zxf boost_1_66_0.tar.gz
    #rm -rf boost_1_66_0.tar.gz
     
    cd boost_1_66_0
    
    mkdir install
    
    ./bootstrap.sh --prefix=`pwd`/install/  > /dev/null 2>/dev/null
    ./b2 install iostreams
    
    export LD_LIBRARY_PATH=`pwd`/install/lib:$LD_LIBRARY_PATH
    cd ../
fi


if [ $rootflag -eq 1 ]
then
    
    wget https://root.cern.ch/download/root_v5.34.34.source.tar.gz
    tar zxvf root_v5.34.34.source.tar.gz
    rm -rf root_v5.34.34.source.tar.gz
    cd root
    
    ./configure --enable-rpath
    make -j8
    make install
    
    source ./bin/thisroot.sh
    
    cd ../
    
fi

cd ../

if [ $final -eq 1 ]
then
    
    echo "current directory"
    echo `pwd`
    make clean
    make 
    
    export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH
fi
>>>>>>> 4219bde6968fd8bcdabe575097d20d2af57ddac7
