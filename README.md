# ANNIEDAQ

ANNIEDAQ is a DAQ implimentation built using the modular ToolDAQ Framework core[1] to give separation between core and implementation code.


## Concept

The main executable creates a ToolChain which is an object that holds Tools. Tools are added to the ToolChain and then the ToolChain can be told to Initialise Execute and Finalise each tool in the chain.

The ToolChain also holds a uesr defined DataModel which each tool has access too and can read ,update and modify. This is the method by which data is passed between Tools.

User Tools can be generated for use in the tool chain by incuding a Tool header. This can be done manually or by use of the newTool.sh script.

For more information consult the README files in the relavent directories


## Building

make clean
make


## Usage 

./main

note all configuration variables are passed through config files to the Tools


## Customising

To adapt for your own uses you need to:

 Create user tools : see README in UserTools/
 Modify data model : see README in DataModel/
 Modify main : see README in src/ 
 Modify Makefile : For simple applications the only modification needed is to populate the variables for your data model and user tool dependancies.  	

Copyright (c) 2018 ANNIE Collaboration

[1] Benajmin Richards. (2018, November 11). ToolDAQ Framework v2.1.1 (Version V2.1.1). Zenodo. http://doi.org/10.5281/zenodo.1482767
