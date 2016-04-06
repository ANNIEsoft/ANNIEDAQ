#include <iostream>

#include "CamacCrate.h"
#include "Lecroy3377.h"
#include "Lecroy4300b.h"
#include "libxxusb.h"
#include "usb.h"

int main (int argc, char** argv)
{
	std::cout << "\nHello World!\n";
	Lecroy3377* prova = new Lecroy3377(2);
	return 0;
}
