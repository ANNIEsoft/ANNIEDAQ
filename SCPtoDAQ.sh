#! /bin/bash

scp  192.168.163.36:/home/annie/ANNIEDAQ/* /home/annie/VME/ANNIEDAQ/
scp  192.168.163.36:/home/annie/ANNIEDAQ/configfiles/* /home/annie/VME/ANNIEDAQ/configfiles
scp  192.168.163.36:/home/annie/ANNIEDAQ/DataModel/* /home/annie/VME/ANNIEDAQ/Datamodel
scp  192.168.163.36:/home/annie/ANNIEDAQ/src/* /home/annie/VME/ANNIEDAQ/src
scp  -r 192.168.163.36:/home/annie/ANNIEDAQ/UserTools/* /home/annie/VME/ANNIEDAQ/UserTools
scp -r 192.168.163.36:/home/annie/ANNIEDAQ/ToolDAQ/ToolDAQFramework /home/annie/VME/ANNIEDAQ/ToolDAQ/