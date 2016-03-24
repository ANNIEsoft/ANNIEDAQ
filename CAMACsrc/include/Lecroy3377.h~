/*
 * Lecroy 3377 TDC CAMAC module class
 *
 * Author: Tommaso Boschi
 */

#ifndef Lecroy3377_H
#define Lecroy3377_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <bitset>

#include "libxxusb.h"
#include "CamacCrate.h"

class Lecroy3377 : public CamacCrate
{
	public:

		Lecroy3377(int NSlot);	//Constructor !
//		~Lecroy3377();
		int ReadFIFOall(std::vector<long> &vData);	//F(0)·A(0) 
		int ReadFIFO();					//F(0)·A(1)COMMON START ONLY	
		int ExFIFOOut();				//F(0)·A(2)COMMON START ONLY
		long ReadReg(int R);				//F(1)·A(1) 
		int ReadTestReg();				//F(1)·A(6) COMMON START ONLY
		int TestLAM();					//F(8)·A(0)
		int ClearAll();					//F(9)·A(0) !
		int ClearLAM();					//F(10)·A(0)
		int WriteFIFOData();				//F(16)·A(0)COMMON START ONLY
		int WriteFIFOtag();				//F(16)·A(1)COMMON START ONLY
		int WriteReg(int R, long &Data);		//F(17)·A(0) !
		int DisLAM(long *Data, int *Q, int *X);		//F(24)·A(0)
		int DisAcq(long *Data, int *Q, int *X);		//F(24)·A(1)
		int InitTest();					//F(25)·A(0)COMMON START ONLY
		int EnLAM();					//F(26)·A(0)
		int EnAcq();					//F(26)·A(1) !
		int TestBuff();					//F(27)·A(0) !
		int TestBusy();					//F(27)·A(1) !
		int TestEvent();				//F(27)·A(2) !
		int TestFIFO();					//F(27)·A(3) !
		int Reprog();					//F(30)
		int READ(int F, int A, long &Data);		//F(x)·A(y) 
		int WRITE(int F, int A, long &Data);		//F(x)·A(y)

		int GetID();					//Return ID
		int GetSlot();					//Return Slot
		void Lecroy3377::PrintRegister();		//Print Register vars
		void Lecroy3377::GetRegister()			//Retrieve register 
		void Lecroy3377::SetRegister()			//Send register parameters
		void Lecroy3377::EncRegister()			//Register control Encoder
		void Lecroy3377::DecRegister()			//Register control Decoder


		///////Register Control vars: num_reg, bit-size
		//////
		/////Public, so anyone can use them
		////
		///For correct usage, please read documentation
		//
		int ModIDcode;		//0, 0-7	er definable module ID code, in header data word. 0 (def)
		bool RecEdges;		//0, 10		0 (def) Leading edge recorded, 1 both edges recorded
		bool RedoutMode;	//0, 11		0 (def) CAMAC redout, 1 ECL PORT
		bool BuffMode;		//0, 12		0 (def) Single Buffer, 1 Multievent Buffer
		bool HeaderMode;	//0, 13		0 (def) Header on, 1 skip header
		int Mode;		//0, 14-15	Mode: 0 (def), 1, 2, 3
		int TrgWidth;		//1, 0-3	Selects the trigger output pulse width, in clock units. 0 (def).
		int TrgDelay;		//1, 4-7	Selects the trigger pulse delay, in clock units. 0 (def)
		int TrgClock;		//1, 8-9	0 (def) = 25 ns, 1 = 50 ns, 2 = 100 ns, 3 = selects external trg clock
		int MPI;		//1, 10-11	0 (def) = no MPI, 1 = 800 ns MPI, 2 = 1600 ns MPI, 3 = 3200 ns MP	
		bool FFERAmode;		//1, 12		Selects FAST FERA mode: 1 = fast, 0 (def) = normal
		int EvSerNum;		//1, 13-15	Event serial number. This 3 bit number is in the header
		int MaxHITS;		//2, 0-3	Maximum number of hits allowed per TDC channel. From 0 = 1 to 15 (def)= 16
		int FullScale;		//2, 4-15	Maximum full scale time allowed for the TDC data, in units of 8 ns, from 0 to 32767.5 ns
		int ReqDelay;		//3, 0-3	Request delay setting, from 0 to 30 microseconds, in 2 microsecond steps. 0 (def)

	private:

		int ID;
		long Control[4];
};

#endif
