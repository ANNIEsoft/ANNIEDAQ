#ifndef CamacCrate_H
#define CamacCrate_H

#include <iostream>
#include <vector>

#include "libxxusb.h"

class CamacCrate
{
	public:

		CamacCrate(int i = 0);
		~CamacCrate();
		long READ(int ID, int F, int A, long &Data, int &Q, int &X);
		long WRITE(int ID, int F, int A, long &Data, int &Q, int &X);
		void ListSlot();
		int GetSlot();
//		void SetCrate();
//		int GetCrate();
		int Z();		//Z
		int C();		//C
		int I();		//I

	protected:

		std::vector<int> Slot;

	private:

		void USBFind();
		void USBOpen(int i);
		void USBClose();
		void Init(int i);

		xxusb_device_type devices[100];
		struct usb_device *Mdev;
		usb_dev_handle *Mudev;

};

#endif
