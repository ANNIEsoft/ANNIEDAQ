#! /bin/bash

scp  /home/annie/VME/ANNIEDAQ/* 192.168.163.36:/home/annie/ANNIEDAQ/
scp  /home/annie/VME/ANNIEDAQ/configfiles/* 192.168.163.36:/home/annie/ANNIEDAQ/configfiles/
scp  /home/annie/VME/ANNIEDAQ/DataModel/* 192.168.163.36:/home/annie/ANNIEDAQ/DataModel/
scp  -r /home/annie/VME/ANNIEDAQ/uc500adc 192.168.163.36:/home/annie/ANNIEDAQ/
scp  /home/annie/VME/ANNIEDAQ/src/* 192.168.163.36:/home/annie/ANNIEDAQ/src/
scp  -r /home/annie/VME/ANNIEDAQ/UserTools/* 192.168.163.36:/home/annie/ANNIEDAQ/UserTools/
scp -r /home/annie/VME/ANNIEDAQ/ToolDAQ/ToolDAQFramework 192.168.163.36:/home/annie/ANNIEDAQ/ToolDAQ/