#
# filename: Makefile
# 
# basic makefile
#
# Before building install the linux usb packages.
# libusb
# libusb-devel
# usbutils - /usr/sbin/lsusb
#
# For SLF6:
# yum install libusb
# yum install libusb-devel
# yum install usbutils
#
# September 2012 - Geoff Savage
# libxxusb.cpp -> libxxusb.c - build for C, not C++
# Remove extra definitions.
#
# >>> gmake
# gcc -Wall -fPIC -shared -I./../include/ -L./ -lusb -o libxx_usb.so libxxusb.c
# Done

.LIBPATTERNS ="'lib%.so lib%.a' "

CC		= gcc
RM		= rm -f

LDFLAGS		= -O
SOFLAGS		= -shared

CLIBDIRS	= -L./
CLIBS		= -lusb
INCLUDEDIRS	= -I./../include/
CFLAGS      	= -Wall -fPIC $(SOFLAGS) $(INCLUDEDIRS) $(CLIBDIRS) $(CLIBS)

# Library to build.
# The xx indicates that this source can be built for two controllers.
# cc - the camac crate controller
# vm - the vme crate controller
OBJ = libxx_usb.so

library: 
	$(CC) $(CFLAGS) -o $(OBJ) libxxusb.c
	@echo "Done"

install:
	@cp $(OBJ) /usr/lib/

clean:
	$(RM) *.so 

