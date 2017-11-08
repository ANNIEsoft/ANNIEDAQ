#!/bin/bash

#source /home/annie/ANNIEDAQ/Setup.sh
#export LD_LIBRARY_PATH=hello
echo $LD_LIBRARY_PATH > /tmp/httppath
more /data/logs/test1 > /tmp/httpfile
/var/www/cgi-bin/logtest.cgi &> /tmp/httprun