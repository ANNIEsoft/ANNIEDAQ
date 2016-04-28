**
# CAMAC class

##Main
**
Main load modules from config file.
Addresses cards and retrieve data

**
###Building
**
```
make clean
make
```
**
###Usage 
**
```
./main
```
##Demo
Code provided in XXUSB libraries.
Easy way to test whether the CCUSB works or not.
In line access to CAMAC functions via NAF code.

**
###Building
**
```
make clean
make demo
```
**
###Usage 
**
```
./demo
```
##DAQ
Code provided in XXUSB libraries.
Test for CCUSB stack mode.

**
###Building
**
```
make clean
make daq
```
**
###Usage 
**
```
./daq
```
**
##Other utilities
**

###test3377
For testing the Lecroy 3377 module (TDC)

```
make test3377
./test3377
```

###test4300b
For testing the Lecroy 4300b module (ADC)

```
make test4300b
./test4300b
```
