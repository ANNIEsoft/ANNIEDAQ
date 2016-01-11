#!/bin/bash

boostlib=/usr/local/lib
echo $boostlib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/ToolDAQ/zeromq-4.0.7/lib:$boostlib

