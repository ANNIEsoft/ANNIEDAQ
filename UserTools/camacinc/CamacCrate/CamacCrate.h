/*
 * CC-USB CAMAC controller module and Crate initialiser class 
 *
 * Author: Tommaso Boschi
 */


#ifndef CamacCrate_H
#define CamacCrate_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <bitset>

#include "libxxusb.h"

class CamacCrate
{
	public:

		//CAMACCRATE
		CamacCrate(int i = 0);
		virtual ~CamacCrate();
		int READ(int ID, int F, int A, long &Data, int &Q, int &X);
		int WRITE(int ID, int F, int A, long &Data, int &Q, int &X);
		void LoadStack(std::string fname);		//Load stack from file
		void EncStack(std::string fname);		//Encode stack from file
		int PushStack();		//Push stack to CC USB
		int PullStack();				//Pull stack from CC USB
		void PrintStack();				//Print stack
		int StartStack();				//Start Acquisition Mode
		int StopStack();				//Stop Acquisition Mode
		int SetLAMmask(std::string &Mask);		//Set LAM mask as a binary string
		int SetLAMmask(long &Mask);			//Set LAM mask as an integer
		int GlobalRegister(std::string &Reg);		//Write the CC global register as a string
		int GlobalRegister(long &Reg);			//Write the CC global register as an int
		int ReadFIFO(std::vector<int> &vData);		//Read USB fifo
		int ClearFIFO();
		int GetSlot(int ID);
		int GetCrate(int ID);
		void ListSlot();

		int Z();		//Z
		int C();		//C
		int I(bool inh);	//I

		//LECROY3377
		virtual int ReadFIFOall(std::map<int, int> &vData) { return 0; }	
		virtual int ReadFIFO() { return 0; }				
		virtual int ExFIFOOut() { return 0; }	
		virtual int ReadReg(int R) { return 0; }
		virtual int ReadTestReg() { return 0; }	
		virtual int WriteFIFOData() { return 0; }			
		virtual int WriteFIFOtag() { return 0; }	
		virtual int WriteReg(int R, int *Data) { return 0; }	
		virtual int DisLAM() { return 0; }		
		virtual int DisAcq() { return 0; }			
		virtual int EnLAM() { return 0; }					
		virtual int EnAcq() { return 0; }		
		virtual int TestBuff() { return 0; }		
		virtual int TestBusy() { return 0; }		
		virtual int TestEvent() { return 0; }		
		virtual int TestFIFO() { return 0; }		
		virtual int CommonStop() { return 0; }
		virtual int CommonStart() { return 0; }
//		virtual void StartTestReg() { ; }
//		virtual void StopTestReg() { ; }

		//LECROY4300B
		virtual int ReadReg(int &Data)	{ return 0; }
		virtual int ReadPed(int Ch, int &Data) { return 0; }
		virtual int ReadOut(int &Data, int) { return 0; }
		virtual int WriteReg(int &Data) { return 0; }
		virtual int WritePed(int Ch, int &Data) { return 0; }
		virtual int DumpCompressed(std::map<int, int> &mData) { return 0; }
		virtual int DumpAll(std::map<int, int> &mData) { return 0; }
		virtual int GetPedestal() { return 0; }					//Get Pedestal from card
		virtual int SetPedestal() { return 0; }					//Set Pedestal to card
		virtual void LoadPedestal(std::string fname) { ; }
		virtual void PrintPedestal() { ; }


		//COMMON
		virtual int GetData(std::map<int, int> &mData) { return 0; }
		virtual int ClearAll() { return 0; }				
		virtual int ClearLAM() { return 0; }	
		virtual int TestLAM() { return 0; }			
		virtual int InitTest() { return 0; }				
		virtual void ParseCompData(int Word, int &Stat, int &Num, bool &B0) { ; }
		virtual void DecRegister() { ; }
		virtual void EncRegister() { ; }
		virtual void GetRegister() { ; }
		virtual void SetRegister() { ; }
		virtual void PrintRegister() { ; }
		virtual void PrintRegRaw() { ; }
		virtual void SetConfig(std::string config) { ; }
		virtual int READ(int F, int A, int &Data, int &Q, int &X) { return 0; }//F(x)·A(y) 
		virtual int WRITE(int F, int A, int &Data, int &Q, int &X) { return 0; }//F(x)·A(y)
		virtual int GetSlot() { return 0; }
		virtual int GetCrate() { return 0; }
		virtual int GetID() { return 0; }

	protected:

		int BittoInt(std::bitset<16> &bitref, int a, int b);
		static std::vector<int> Slot;
		static std::vector<int> Crate;

	private:

		void USBFind();
		void USBOpen(int i);
		void USBClose();
		void Init(int i);

		std::vector<int> vStack;
		static bool IsOpen;
		static int ndev;
		static xxusb_device_type devices[100];
		static std::vector<struct usb_device*> Mdev;
		static std::map<int, usb_dev_handle*> Mudev;
		std::map<std::string,int> devicemap;
		int cratenum;

};

#endif
