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
Test for CCUSB stack mode using Lecroy 2249A.

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
##StackBuilder
Stack words creator, from text file to text file.

The format is the following:

```
N	A	F	C	M
```

where:
- N is the slot Number;
- A is the function subAddress;
- F is the Function number;
- C is the Continuation bit. Set this to 1 to complex stack lines, mainly used for Q-stop mode;
- M is the Maximum number of repetetion in Q-stop mode. Don't care if C is 0;

**
###Building
**
```
make clean
make stackbuild
```
**
###Usage 
**
```
./stackbuild Input Output

```

Input is the file with the commands to be encoded. Default name is "Stackconfig".
Output is the file with the encoded lines in. Default name is "Stackline".

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
