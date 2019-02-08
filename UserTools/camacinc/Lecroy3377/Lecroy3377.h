/*
 * Lecroy 3377 TDC CAMAC module class, single word only!
 * double word not implemented yet
 *
 * Author: Tommaso Boschi
 */

#ifndef Lecroy3377_H
#define Lecroy3377_H

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <bitset>

#include "libxxusb.h"
#include "CamacCrate.h"

class Lecroy3377 : public CamacCrate
{
	public:
/*		Lecroy3377(int NSlot, int i = 0) : CamacCrate(i)	//Subclass constructor, n of Slot given
		{
			Slot.push_back(NSlot);
			ID = Slot.size()-1;
		}
*/
		Lecroy3377(int NSlot, std::string config, int i = 0);// : CamacCrate(i);	//Constructor !

		int GetData(std::map<int, int> &mData);		//Store data into map

		int ReadFIFOall(std::map<int, int> &vData);	//F(0)·A(0) 
		int ReadFIFO();					//F(0)·A(1)	COMMON START	
		int ExFIFOOut();				//F(0)·A(2)	COMMON START
		int ReadReg(int R);				//F(1)·A(1) 
		int ReadTestReg();				//F(1)·A(6)	COMMON START
		int TestLAM();					//F(8)·A(0)
		int ClearAll();					//F(9)·A(0)
		int ClearLAM();					//F(10)·A(0)
		int WriteFIFOData();				//F(16)·A(0)	COMMON START
		int WriteFIFOtag();				//F(16)·A(1)	COMMON START
		int WriteReg(int R, int *Data);		//F(17)·A(0)
		int DisLAM();					//F(24)·A(0)
		int DisAcq();					//F(24)·A(1)
		int InitTest();					//F(25)·A(0)	COMMON START
		int EnLAM();					//F(26)·A(0)
		int EnAcq();					//F(26)·A(1)
		int TestBuff();					//F(27)·A(0)
		int TestBusy();					//F(27)·A(1)
		int TestEvent();				//F(27)·A(2)
		int TestFIFO();					//F(27)·A(3)
		int CommonStop();				//F(30)
		int CommonStart();				//F(30) and F(21)
		int READ(int F, int A, int &Data, int &Q, int &X);		//F(x)·A(y) 
		int WRITE(int F, int A, int &Data, int &Q, int &X);		//F(x)·A(y)

		int GetID();				//Return ID
		int GetSlot();				//Return Slot
		int GetCrate();
		void ParseCompData(int Word, int &Stat, int &Num, bool &B1);
		void PrintRegRaw();			//Print undecoded register
		void PrintRegister();			//Print Register vars
		void GetRegister();			//Retrieve register 
		void SetRegister();			//Send register parameters
		void StartTestReg();			//Set default register for Common Start testing
		void StopTestReg();			//Set default register for Common Stop testing
		void EncRegister();			//Register control Encoder
		void DecRegister();			//Register control Decoder	
		void SetConfig(std::string config);	//Set register from configfile




	private:

		int ID;
		int Control[6];

		///////Register Control vars: num_reg, bit-size
		//////
		/////Private, settable by config file
		////
		///For correct usage, please read documentation
		//
		bool Common;		//0 if STOP, 1 if START

		int ModID;		//0, 0-7	user definable module ID code, in header data word. 0 (def)
		int TDCRes;		//0, 8-9	Resolution: 0 = 0.5 nsec (def), 1 = 1.0 nsec, 2 = 2.0 nsec, 3 = 4.0 nsec
		int RecEdges;		//0, 10		0 (def) Leading edge recorded, 1 both edges recorded
		int ReadoutMode;	//0, 11		0 (def) CAMAC readout, 1 ECL PORT
		int BuffMode;		//0, 12		0 (def) Single Buffer, 1 Multievent Buffer
		int HeaderMode;		//0, 13		0 (def) Header on, 1 skip header
		int Mode;		//0, 14-15	Mode: 0 (def), 1, 2, 3
		int TrgWidth;		//1, 0-3	Selects the trigger output pulse width, in clock units. 0 (def).
		int TrgDelay;		//1, 4-7	Selects the trigger pulse delay, in clock units. 0 (def)
		int TrgClock;		//1, 8-9	0 (def) = 25 ns, 1 = 50 ns, 2 = 100 ns, 3 = selects external trg clock
		int MPI;		//1, 10-11	0 (def) = no MPI, 1 = 800 ns MPI, 2 = 1600 ns MPI, 3 = 3200 ns MP	
		int FFERAmode;		//1, 12		Selects FAST FERA mode: 1 = fast, 0 (def) = normal
		int EvSerNum;		//1, 13-15	Event serial number. This 3 bit number is in the header
		int MaxHITS;		//2, 0-3	Maximum number of hits allowed per TDC channel. From 0 = 1 to 15 (def)= 16
		int FullScale;		//2, 4-15	Maximum full scale time allowed for the TDC data, in units of 8 ns, from 0 to 32767.5 ns
		int ReqDelay;		//3, 0-3	Request delay setting, from 0 to 30 microseconds, in 2 microsecond steps. 0 (def)
		int OffSet;		//3, 4-15	Offset to subtract from tdc value, in common stop mode
		int MaxTimeRange;	//3, 4-15	Maximum time range for data, tested before shifting and readout, in 8 ns
		int StartTO;		//4, 0-9	Common start time out, in 50 ns, up to 32 us. Must be a value LESS THAN full scale
		int TestPulse;		//5, 0-4	Number of pulses generated in test mode. 0-31 pulses, each 1/2 clock period long.	
		int TestClock;		//5, 5-6	Test mode clock: 0 = 100 ns, 1 = 200 ns, 2 = 400 ns, 3 = 800 ns
		int TestFlag;		//5, 8		Test enable. This must be 1 for test mode.						

};

#endif
