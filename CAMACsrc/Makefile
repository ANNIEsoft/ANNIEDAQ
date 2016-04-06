LUSB = -lm 
LXUSB = $(LSUB) -lXXusb
LCC = $(LXUSB) -lCamacCrate
LLEC = $(LCC) -lLecroy3377 -lLecroy4300b

all: lib/libXXusb lib/libCamacCrate lib/libLecroy3377 lib/libLecroy4300b
	g++ -g src/main.cpp -o main -I include -L lib $(LLEC) 

lib/libXXusb:
	cp src/XXUSB/usb.h include/
	cp src/XXUSB/libxxusb.h include/
	g++ -g -shared -fPIC src/XXUSB/libxxusb.cpp -I include -L lib $(LUSB) -o lib/libXXusb.so

lib/libCamacCrate: lib/libXXusb
	cp src/CamacCrate/CamacCrate.h include/
	g++ -g -shared -fPIC src/CamacCrate/CamacCrate.cpp -I include -L lib $(LXUSB) -o lib/libCamacCrate.so

lib/libLecroy3377: lib/libXXusb lib/libCamacCrate
	cp src/Lecroy3377/Lecroy3377.h include/
	g++ -g -shared -fPIC src/Lecroy3377/Lecroy3377.cpp -I include -L lib $(LCC) -o lib/libLecroy3377.so

lib/libLecroy4300b: lib/libXXusb lib/libCamacCrate
	cp src/Lecroy4300b/Lecroy4300b.h include/
	g++ -g -shared -fPIC src/Lecroy4300b/Lecroy4300b.cpp -I include -L lib $(LCC) -o lib/libLecroy4300b.so

clean:
	rm -f main
	rm -f lib/*.so
	rm -f include/*.h
