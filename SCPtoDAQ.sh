#! /bin/bash

scp  vme1:/home/annie/ANNIEDAQ/* /home/annie/VME/ANNIEDAQ/
scp  vme1:/home/annie/ANNIEDAQ/configfiles/* /home/annie/VME/ANNIEDAQ/configfiles
scp  vme1:/home/annie/ANNIEDAQ/DataModel/* /home/annie/VME/ANNIEDAQ/Datamodel
scp  vme1:/home/annie/ANNIEDAQ/src/* /home/annie/VME/ANNIEDAQ/src
scp  -r vme1:/home/annie/ANNIEDAQ/UserTools/* /home/annie/VME/ANNIEDAQ/UserTools
scp -r vme1:/home/annie/ANNIEDAQ/ToolDAQ/ToolDAQFramework /home/annie/VME/ANNIEDAQ/ToolDAQ/