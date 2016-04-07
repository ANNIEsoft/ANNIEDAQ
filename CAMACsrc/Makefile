LUSB = -lm -lusb 
MAIN = src/main.cpp
DCC = src/CamacCrate/CamacCrate.cpp
DL3 = src/Lecroy3377/Lecroy3377.cpp
DL4 = src/Lecroy4300b/Lecroy4300b.cpp
DXX = src/XXUSB/libxxusb.cpp

all: cpinclude
	g++ -o main $(MAIN) -I include -L lib $(DCC) $(DL3) $(DL4) $(DXX) $(LUSB)

demo: cpinclude
	g++ -v -o demo CC-USB_demo.cpp -I include/ -L lib/ -lusb src/XXUSB/libxxusb.cpp

cpinclude:
	cp src/CamacCrate/*.h include/
	cp src/Lecroy3377/*.h include/
	cp src/Lecroy4300b/*.h include/
	cp src/XXUSB/*.h include/
	cp src/libusb.so lib/

clean:
	rm -f main
	rm -f lib/*.so
	rm -f include/*.h
	rm -f demo

