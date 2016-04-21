/*
 * CC-USB CAMAC controller module and Crate initialiser class 
 *
 * Author: Tommaso Boschi
 */


#ifndef CamacCrate_H
#define CamacCrate_H

#include <iostream>
#include <vector>
#include <bitset>

#include "libxxusb.h"

class CamacCrate
{
	public:

		//CAMACCRATE
		CamacCrate(int i);
		~CamacCrate();
		int READ(int ID, int F, int A, long &Data, int &Q, int &X);
		int WRITE(int ID, int F, int A, long &Data, int &Q, int &X);
		int GetSlot(int ID);
		void ListSlot();

		int Z();		//Z
		int C();		//C
		int I(bool inh);	//I

		//LECROY3377
		virtual int ReadFIFOall(std::vector<long> &vData) { return 0; }	
		virtual int ReadFIFO() { return 0; }				
		virtual int ExFIFOOut() { return 0; }	
		virtual long ReadReg(int R) { return 0; }
		virtual int ReadTestReg() { return 0; }	
		virtual int WriteFIFOData() { return 0; }			
		virtual int WriteFIFOtag() { return 0; }	
		virtual int WriteReg(int R, long *Data) { return 0; }	
		virtual int DisLAM() { return 0; }		
		virtual int DisAcq() { return 0; }			
		virtual int InitTest() { return 0; }				
		virtual int EnLAM() { return 0; }					
		virtual int EnAcq() { return 0; }		
		virtual int TestBuff() { return 0; }		
		virtual int TestBusy() { return 0; }		
		virtual int TestEvent() { return 0; }		
		virtual int TestFIFO() { return 0; }		
		virtual int Reprog() { return 0; }

		//LECROY4300B
		virtual int ReadReg(long &Data)	{ return 0; }
		virtual int ReadPed(int Ch, long &Data) { return 0; }
		virtual int ReadOut(long &Data, int) { return 0; }
		virtual int WriteReg(long &Data) { return 0; }
		virtual int WritePed(int Ch, long &Data) { return 0; }
		virtual int TestAll() { return 0; }
		virtual int GetData(std::vector<long> &vData, std::vector<long> &vSAD) { return 0; }
		virtual int DumpCompressed(std::vector<long> &vData, std::vector<long> &vSAD) { return 0; }
		virtual int DumpAll(std::vector<long> &vData) { return 0; }
		virtual void ParseCompData(long Word, long &Stat, long &Num, bool &B0) { ; }

		//COMMON
		virtual int ClearAll() { return 0; }				
		virtual int ClearLAM() { return 0; }	
		virtual int TestLAM() { return 0; }			
		virtual int READ(int F, int A, long &Data, int &Q, int &X) { return 0; }//F(x)·A(y) 
		virtual int WRITE(int F, int A, long &Data, int &Q, int &X) { return 0; }//F(x)·A(y)
		virtual void DecRegister() { ; }
		virtual void EncRegister() { ; }
		virtual void GetRegister() { ; }
		virtual void SetRegister() { ; }
		virtual void PrintRegister() { ; }


	protected:

		int BittoInt(std::bitset<16> &bitref, int a, int b);
		std::vector<int> Slot;

	private:

		void USBFind();
		void USBOpen(int i);
		void USBClose();
		void Init(int i);

		static bool IsOpen;
		static int ndev;
		static xxusb_device_type devices[100];
		static struct usb_device *Mdev;
		static usb_dev_handle *Mudev;

};

#endif
