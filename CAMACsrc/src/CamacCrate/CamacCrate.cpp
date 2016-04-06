#include "CamacCrate.h"

CamacCrate::CamacCrate(int i = 0)	//Class constructor
{
	if (!IsOpen) Init(i);
	else std::cout << "\nWarning: USB device has already been opened\n\n";
	IsOpen = true;
}

CamacCrate::~CamacCrate()		//Class destructor
{
	USBClose();
	IsOpen = false;
}

void CamacCrate::Init(int i)		//Initialize device
{
	/*Find XX_USB devices and open the first one found. */
	USBFind();
	USBOpen(i);
}

void CamacCrate::USBFind()		//Find usb devices
{
	xxusb_devices_find(devices);
}

void CamacCrate::USBOpen(int i)		//Open i device and create handler
{
	Mdev = devices[i].usbdev;
	Mudev = xxusb_device_open(Mdev); 
	if(Mudev) std::cout << "\n\nFailed to open CC-USB. \n\n";
	else std::cout << "\n\nCC-USB opened. \n\n";
}

void CamacCrate::USBClose()		//Close USB devices
{
	xxusb_device_close(Mudev);
	if (Mudev) std::cout << "\n\nFailed to close CC-USB. \n\n";
	else std::cout << "\n\nCC-USB closed. \n\n";
}

int CamacCrate::READ(int ID, int F, int A, long &Data, int &Q, int &X)	//Generic READ func, suitable for F 0-15, 24-31
{
	return CAMAC_read(Mudev, Slot.at(ID), F, A, &Data, &Q, &X);
}

int CamacCrate::WRITE(int ID, int F, int A, long &Data, int &Q, int &X)	//Generic WRITE func, suitable for F 16-23
{

	return CAMAC_write(Mudev, Slot.at(ID), F, A, Data, &Q, &X);
}

void CamacCrate::ListSlot()		//List all modules in CAMAC
{
	for (int i = 0; i < Slot.size(); i++)
		std::cout << "[" << i << "] :\tmodule slot " << Slot.at(i);
}

int CamacCrate::GetSlot(int ID)		//Return n of Slot, ID given
{
	return Slot.at(ID);
}

int CamacCrate::Z()			//Global ZERO
{
	return CAMAC_Z(Mudev);
}

int CamacCrate::C()			//Global CLEAR
{
	return CAMAC_C(Mudev);
}

int CamacCrate::I(bool inh)		//Global INHIBIT
{
	return CAMAC_I(Mudev, inh);
}

int CamacCrate::BittoInt(std::bitset<16> &bitref, int a, int b)
{
	int tmp, sum = 0;
	if (a > b)
	{
		tmp = a;
		a = b;
		b = tmp;
	}
	for (int i = b; i >= a; i--)
	{
		sum *= 2;
		sum += bitref.test(i);
	}
	return sum;
}
