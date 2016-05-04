#include "CamacCrate.h"

bool CamacCrate::IsOpen = false;
int CamacCrate::ndev = 0;
xxusb_device_type CamacCrate::devices[100] = {};
struct usb_device *CamacCrate::Mdev = 0;
usb_dev_handle *CamacCrate::Mudev = 0;

CamacCrate::CamacCrate(int i)	//Class constructor
{
	if (!CamacCrate::IsOpen) Init(i);
	else std::cout << "\nWarning: USB device has already been opened\n\n";
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
	USBOpen(i);
	Z();
	C();
}

void CamacCrate::USBFind()		//Find usb devices
{
	ndev = xxusb_devices_find(devices);
	for (int i = 0; i < ndev; i++)
		std::cout << i << " device is " << devices[i].SerialString << std::endl;
}

void CamacCrate::USBOpen(int i)		//Open i device and create handler
{
	Mdev = devices[i].usbdev;
	Mudev = xxusb_device_open(Mdev); 
	if(!Mudev) std::cout << "\n\nFailed to open CC-USB. \n\n";
	else std::cout << "\n\nCC-USB opened. \n\n";
}

void CamacCrate::USBClose()		//Close USB devices
{
	xxusb_device_close(Mudev);
	if (Mudev) std::cout << "\n\nFailed to close CC-USB. \n\n";
	else std::cout << "\n\nCC-USB closed. \n\n";
}

int CamacCrate::SetLAMmask(std::string &Mask)
{
	int Q = 0, X = 0;
	std::bitset<24> lam(Mask);
	long llam = lam.to_ulong();
	return WRITE(25, 9, 16, llam, Q, X); //LAM
}

int CamacCrate::SetLAMmask(char *Mask)
{
	std::string sMask = std::string(Mask);
	return SetLAMmask(sMask);
}

int CamacCrate::SetLAMmask(long &Mask)
{
	int Q = 0, X = 0;
	return WRITE(25, 9, 16, Mask, Q, X); //LAM
}

int CamacCrate::GlobalRegister(std::string &Reg)
{
	int Q = 0, X = 0;
	std::bitset<16> glob(Reg);
	long lglob = glob.to_ulong();
	return WRITE(25, 1, 16, lglob, Q, X);
}

int CamacCrate::GlobalRegister(char *Reg)
{
	std::string sReg = std::string(Reg);
	return GlobalRegister(sReg);
}

int CamacCrate::GlobalRegister(long &Reg)
{
	int Q = 0, X = 0;
	return WRITE(25, 1, 16, Reg, Q, X);
}

int CamacCrate::LoadStack(char *fname)
{
	std::ifstream fin (fname);
	std::string Line;
	std::stringstream ssL;
	long val, stack[768];
	std::vector<int> vStack;
	while (getline(fin, Line))
	{
		if (Line[0] == '#') continue;
		else
		{
			ssL.str("");
			ssL.clear();
			ssL << Line;
			ssL >> std::hex >> val;
			vStack.push_back(val);
		}
	}
	if (vStack.size() > 768)
		std::cout << "Stack too long! Exceeding max value (768 words)\n\n";
	for (int i = 0; i < vStack.size(); i++)
		stack[i] = vStack.at(i);

	return xxusb_stack_write(Mudev, 2, stack);
}

int CamacCrate::LoadStack(std::string fname)
{
	return LoadStack(fname.c_str());
}			

int CamacCrate::ReadStack()
{
	long stack[768];
	int ret = xxusb_stack_read(Mudev, 2, stack);
	if (ret >= 0)
	{
		std::cout << "Stack from CC-USB:" << std::hex << std::endl;
		for (int i = 0; i < ret/2; i++)
			std::cout << stack[i] << std::endl;
	}
	return ret;
}
			
int CamacCrate::StartStack()
{
        return xxusb_register_write(Mudev, 1, 0x1); // start acquisition
}

int CamacCrate::StopStack()
{
        int ret = xxusb_register_write(Mudev, 1, 0x0); // stop acquisition
	ClearFIFO();
	return ret;
}

int CamacCrate::ReadFIFO(std::vector<std::string> &vData)
{
	short IntArray[1000];
	std::string dt;
	std::stringstream ssdt;
	int ret = xxusb_bulk_read(Mudev, IntArray, 8192, 500);
	if (ret >= 0)
	{
		for (int i = 0; i < ret/2; i++)
		{
			ssdt.str("");
			ssdt.clear();
			ssdt << IntArray[i];
			ssdt >> dt;
			vData.push_back(dt);
		}
	}
	return ret;
}

int CamacCrate::ClearFIFO()
{
	short IntArray[1000];
	int ret;
	do
		ret = xxusb_bulk_read(Mudev, IntArray, 8192, 1000);
	while (ret > 0);
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
