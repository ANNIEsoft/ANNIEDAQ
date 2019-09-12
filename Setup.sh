#!/bin/bash

boostlib=ToolDAQ/boost_1_66_0/install/lib/
echo $boostlib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/ToolDAQ/zeromq-4.0.7/lib:$boostlib:`pwd`/lib

