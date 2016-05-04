S = src
MAIN = $S/main.cpp
DEMO = $S/CC-USB_demo.cpp
DAQ = $S/CC-DAQ_2249.cpp
TEST3 = $S/test3377.cpp
TEST4 = $S/test4300b.cpp
STACK = $S/stackbuilder.cpp
CCSTACK = $S/ccstack.cpp
DCC = $S/CamacCrate
DL3 = $S/Lecroy3377
DL4 = $S/Lecroy4300b
O1 = main
O2 = demo
DQ = daq
O3 = test3377 
O4 = test4300b 
SB = stackbuilder
CS = ccstack
LXX = -lm -lxx_usb

all: cpinclude
	g++ -g -fPIC -o $(O1) $(MAIN) -I include/ -L lib/ $(DCC)/*.cpp $(DL3)/*.cpp $(DL4)/*.cpp $(LXX)
	@echo "Done."

demo: cpinclude
	g++ -g -fPIC -o $(O2) $(DEMO) -I include/ -L lib/ $(LXX)
	@echo "Done."

daq: cpinclude
	g++ -g -fPIC -o $(DQ) $(DAQ) -I include/ -L lib/ $(LXX)
	@echo "Done."

test3377: cpinclude
	g++ -g -fPIC -o $(O3) $(TEST3) -I include/ -L lib/ $(DCC)/*.cpp $(DL3)/*.cpp $(DL4)/*.cpp $(LXX)
	@echo "Done."

test4300b: cpinclude
	g++ -g -fPIC -o $(O4) $(TEST4) -I include/ -L lib/ $(DCC)/*.cpp $(DL3)/*.cpp $(DL4)/*.cpp $(LXX)
	@echo "Done."

stackbuild: cpinclude
	g++ -g -fPIC -o $(SB) $(STACK) -I include/ -L lib/ $(DCC)/*.cpp $(DL3)/*.cpp $(DL4)/*.cpp $(LXX)
	@echo "Done."

ccstack: cpinclude
	g++ -g -fPIC -o $(CS) $(CCSTACK) -I include/ -L lib/ $(DCC)/*.cpp $(DL3)/*.cpp $(DL4)/*.cpp $(LXX)
	@echo "Done."

cpinclude:
	cp $(DCC)/*.h include/
	cp $(DL3)/*.h include/
	cp $(DL4)/*.h include/
	cp $S/XXUSB/*.h include/
	cp $S/makelib/libxx_usb.so lib/

clean:
	rm -f $(O1)
	rm -f $(O2)
	rm -f $(O3)
	rm -f $(O4)
	rm -f $(SB)
	rm -f $(CS)
	rm -f $(DQ)
	rm -f lib/*.so
	rm -f include/*.h


