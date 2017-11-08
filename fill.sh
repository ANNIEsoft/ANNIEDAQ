#!/bin/bash

while read CMD; do
    echo "$CMD"
echo "Insert into pmtconnections (Configuration, PMTID, GF , GX , GY , GZ , SplitterID, HVID, OPVoltage, Channel, VMECARD, VMEChannel) values (0 $CMD);"|psql
done <./PMTid