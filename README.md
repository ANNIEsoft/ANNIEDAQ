# MRD DAQ in ToolDAQ Framework

The main executable creates a ToolChain which is an object that holds Tools.
Tools are added to the ToolChain and then the ToolChain can be told to Initialise Execute and Finalise each tool in the chain.

The ToolChain also holds a user defined DataModel which each tool has access too and can read, update and modify.
This is the method by which data is passed between Tools.

Currently tools are:
* Trigger;
* Lecroy module;
* ROOT file writer.

###Trigger tool, CCTrigger.cpp
Read from `configfiles/ModuleConfig` the configuration of the CAMAC crate.
Set up the cards register and map module type to slot number.

Suported types are:
* `ADC`: Lecroy 4300b
* `TDC`: Lecroy 3377
* `TRG`: Lecroy 337 used as trigger board

In ModuleConfig the configuration file for each card's register can be determined.
Example of configuration files are `configfiles/TDCreg` and `configfiles`ADCreg`.
Please refer to the Lecroy manual in roder to set the cards properly.

Trigger is made reading the FIFO of the specified card: if there's data in it, then all cards are read.
When a trigger is found, a DataModel bool variable is set in the Trigger tool which allows the execution of the other tools.

Trigger mode is set in configfiles/TRGConfig:h
* 0 : external trigger, provided by module tpye `TRG`;
* 1 : software trigger, cards accessed randomly, with programmable probability;
* 2 : software trigger and test functions are called.

###Lecroy module tool, Lecroy.cpp
This tool works for both the TDCs and ADCs cards.
In the corresponding configfile, `configfiles/TDCConfig` or `configfiles/ADCConfig`, the Tool is set to be for TDC or ADC.

###ROOT file writer, TreeRecorder.cpp
Fill ROOT trees and save them to file.
Output path and output name are set by `configfiles/TreeConfig`

###DataModel
The data retrived from the CCUSB controller are stored in a DataModel structure called `Model`, in which both the Camac classes (`CC`) pointer and data are saved (`Data`).
`Data` structure is as follows:
* two synchronised vectors, one for the slot number and one of struct type `Channel`
* each `Channel` is a STL map where the key is the channel number and the value the data.

##Building
```
make clean
make
```

###Usage 
```
./main
```

In Interactive mode, `Start` to Initialise and Execute, until `Stop` is sent to Finalise.
`Quit` to quit the software.
In Inline mode, the average Execution frequency is printed out.
