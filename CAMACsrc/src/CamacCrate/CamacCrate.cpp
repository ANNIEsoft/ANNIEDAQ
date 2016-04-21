#include "CamacCrate.h"

bool CamacCrate::IsOpen = false;
int CamacCrate::ndev = 0;
xxusb_device_type CamacCrate::devices[100] = {};
struct usb_device *CamacCrate::Mdev = 0;
usb_dev_handle *CamacCrate::Mudev = 0;

CamacCrate::CamacCrate(int i = 0)	//Class constructor
{
	std::cout << "d0\n";
	if (!CamacCrate::IsOpen) Init(i);
	else std::cout << "\nWarning: USB device has already been opened\n\n";
	CamacCrate::IsOpen = true;
	std::cout << "IsOpen " << CamacCrate::IsOpen << std::endl;
}

CamacCrate::~CamacCrate()		//Class destructor
{
	USBClose();
	CamacCrate::IsOpen = false;
}

void CamacCrate::Init(int i)		//Initialize device
{
	/*Find XX_USB devices and open the first one found. */
	USBFind();
	std::cout << "d3\n";
	USBOpen(i);
	std::cout << "d6\n";
}

void CamacCrate::USBFind()		//Find usb devices
{
	std::cout << "d1\n";
	ndev = xxusb_devices_find(devices);
	std::cout << ndev << " d2\n";
	for (int i = 0; i < ndev; i++)
		std::cout << i << " device is " << devices[i].SerialString << std::endl;
}

void CamacCrate::USBOpen(int i)		//Open i device and create handler
{
	Mdev = devices[i].usbdev;
	std::cout << i << " d4\n";
	Mudev = xxusb_device_open(Mdev); 
	std::cout << "d5\n";
	if(!Mudev) std::cout << "\n\nFailed to open CC-USB. \n\n";
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
