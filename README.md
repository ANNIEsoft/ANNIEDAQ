# MRD DAQ in ToolDAQ Framework

The main executable creates a ToolChain which is an object that holds Tools.
Tools are added to the ToolChain and then the ToolChain can be told to Initialise Execute and Finalise each tool in the chain.

The ToolChain also holds a uesr defined DataModel which each tool has access too and can read, update and modify.
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

Trigger mode is set in configfiles/TRGConfig:h
* 1 : external trigger, provided by module tpye `TRG`;
* 0 : software trigger, cards accessed randomly, with programmable probability.

###Lecroy module tool, Lecroy.cpp
This tool works for both the TDCs and ADCs cards.
PRogrammable trigger probability in `configfiles/TDCConfig` and `configfiles/ADCConfig`.

###ROOT file writer, TreeRecorder.cpp
Fill ROOT trees and save them to file.
Output path and output name are set by `configfiles/TreeConfig`

###DataModel
The data retrived from the CCUSB controler are stored in a DataModel structure called `Model`, in which both the Camac classes (`CC`) pointer and data are saved (`Data`).
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
