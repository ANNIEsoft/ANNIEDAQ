# MRD DAQ in ToolDAQ Framework

The main executable creates a ToolChain which is an object that holds Tools.
Tools are added to the ToolChain and then the ToolChain can be told to Initialise Execute and Finalise each tool in the chain.

The ToolChain also holds a uesr defined DataModel which each tool has access too and can read, update and modify.
This is the method by which data is passed between Tools.

Currently tools are:
* Trigger;
* Lecroy module;
* ROOT file writer.

##Trigger tool, CCTrigger.cpp
Read from configfiles/ModuleConfig the configuration of the CAMAC crate.
Set up the cards register and map module type to slot number.

Suported types are:
* ADC: Lecroy 4300b
* TDC: Lecroy 3377
* TRG: Lecroy 337 used as trigger board

Trigger mode is set in configfiles/TRGConfig:
* 1 : external trigger, provided by module tpye TRG;
* 0 : software trigger, cards accessed randomly, with programmable probability.

##Lecroy module tool, Lecroy.cpp
This tool works for both the TDCs and ADCs cards.
PRogrammable trigger probability in configfiles/TDCConfig and configfiles/ADCConfig.

##ROOT file writer, TreeRecorder.cpp
Fill ROOT trees and save them to file.
Output path and output name are set by configfiles/TreeConfig
These are the two classes used by main.
EventReco is called by PulseFinder, even if there is no inheritance between them.
PulseFinder takes care of scanning the data, selects the pulses, gather them into events, plots 2D histograms for their energy and time.
EventReco is used for individual pulse analysis, which calculates precious and helpful quantities.

Input variables are set through a config file.


##Merge
This code merges in one root file the combined histograms and graphs from the PulseFinder root file outputs.

###Building
```
make clean
make
```

note all configuration variables are passed through config files to the Tools

PulseFinder and Merge can be compiled separately, for quicker compilation
```
make pulse
make merge
```


###Usage 
PulseFinder requires 0 or 1 argument.
The sole argument is the directory where the root files (preferably from the same Run/Subrun) are stored.
Within this folder, a new directory called `result` is created.
If 0 argument are passed, the the current directory is used.

Two outputs are generated for each input root file: a root file and a text file.
The name is the same of the input file, except for the extension and a `r` in front of it if it's a root file or a `d` if it's a dat file.
The root file would contain all the histograms and graphs created, the text file some statistical information concerning the histograms and more.

```
./main
```

For merging the root file into one, the Merge programs must be called.
It requires 0 or 1 argument, which is the folder of the output root file, therefore terminating with `/result`.
In no argoment is passed, then the current folder is used.

```
./Merge directory/result
```

##Config File
The main code searches in the current directory for a file called `config` where all the configuration for PulseFinder, EventReco and Merge are set.
Eleven variables can be set up.
There are some dependancies.

* `debug`	: it is a `bool`, 1 for overwriting already analysed files, 0 for analysing only files without already existing outputs
* `verb`	: sets the verbosity level. At the moment there are 4 levels of verbosity (0 is least verbose, 4 is the most)
* `r_hys`	: it is the hysteresis ratio with respect to any threshold. The disable the threshold hysteresis set this to 0
* `thr_pek`	: it is the threshold for pulse definition
* `thr_evt`	: it is the threshold for event definition
* `thr_sig`	: it is the threshold for signal discrimination from background
* `shape`	: it is the time window, for peak and energy computation
* `binwid`	: it is the time calibration of Data bin width
* `graph`	: it is a `bool`, 1 for shape analysis with EventReco, 0 for skipping it. Requires some postprocessing resources.
* `evlength`	: it is the bin size of Data for shape analysis by EventReco
* `peakpos`	: it is the position of the peak in the evlength window, in percent, during EventReco analysis

Most of these variables have default parameters.
The default values will be implemented in next versions.

##Versions issues

1. v1	issues : The averaging of the pulses is not working as expected, to be fixed. Many variables lack of default values.

