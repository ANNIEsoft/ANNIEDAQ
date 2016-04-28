/*
 * Lecroy 4300b ADC CAMAC module class
 * 10 or 11 bits with 50 ohm input impedance
 *
 * Author: Tommaso Boschi
 */

#ifndef Lecroy4300b_H
#define Lecroy4300b_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <bitset>
#include <ctime>

#include "libxxusb.h"
#include "CamacCrate.h"

class Lecroy4300b : public CamacCrate
{
	public:
		Lecroy4300b(int NSlot, int i);
		int ReadReg(long &Data);		//F(0)·A(0)
		int ReadPed(int Ch, long &Data);	//F(1)·A(0-15)
		int ReadOut(long &Data, int);		//F(2)·A(0-15)
		int TestLAM();				//F(8)·A(0)
		int ClearAll();				//F(9)·A(0)
		int ClearLAM();				//F(10)·A(0)
		int WriteReg(long &Data);		//F(16)·A(0)
		int WritePed(int Ch, long &Data);	//F(17)·A(0-15)
		int TestAll();				//F(25)·A(0)
		int READ(int F, int A, long &Data, int &Q, int &X);	//F(x)·A(y) 
		int WRITE(int F, int A, long &Data, int &Q, int &X);	//F(x)·A(y)

		int GetData(std::vector<long> &vData, std::vector<long> &vSAD);		//Read Data from ADC compressed or not
		int DumpCompressed(std::vector<long> &vData, std::vector<long> &vSAD);	//Retrieve compressed data
		int DumpAll(std::vector<long> &vData);				//Retrieve from all channels
		void DecRegister();						//Register decoder	
		void EncRegister();						//Register encoder
		void GetRegister();						//Retrieve register
		void SetRegister();						//Load register
		void PrintRegister();						//Show register
		void PrintRegRaw();
		void ParseCompData(long Word, long &Stat, long &Num, bool &B1);	//Compressed data parser
		int GetPedestal();					//Get Pedestal from card
		int SetPedestal();					//Set Pedestal to card
		int SetPedFile(std::string fname);			//Read ped from file and SetPedestal
		int GetPedFile(std::string fname);			//GetPedestal and save it to file
		int GetID();		//Return ID
		int GetSlot();		//Return Slot

		///////Status Register, Pedestal vars: bit-size
		//////
		/////Public, so anyone can use them
		////
		///For correct usage, please read documentation
		//
		int VSN;	//0-7	Virtual Station Number
		bool EPS;	//8	ECL Pedestal Subtraction; EPS = 1 subtracts from each ADC pedestal, EPS = 0 no subtraction
		bool ECE;	//9	ECL Compression Enable; ECE = 1 data compression cycle, ECE = 0 blocks the suppression of data
		bool EEN;	//10	ECL ENable; EEN = 1 ECL enabled then CAMAC, EEN = 0 ECL disabled CAMAC enabled after a conversation
		bool CPS;	//11	CAMAC Pedestal Subtraction; CPS = 1 subtracts from each ADC pedestal, CPS = 0 no subtraction
		bool CCE;	//12	CAMAC Compression Enable; CCE = 1 data compression cycle, CCE = 0 blocks the suppression of data
		bool CSR;	//13	CAMAC Sequential Readout; CSR = 1 Valid data read sequentially, CSR = 0 Random access readout
		bool CLE;	//14	CAMAC Look-at-me Enable; CLE = 1 LAM set as data ready, CLE = 0 LAM output inhibited
		bool OFS;	//15	Overflow Suppress; OFS = 1 suppress overflows in ECE or CCE, OFS = 0 disabled

		long Ped[16];	//0-8

	private:

		int ID;
		long Control, Word;
};

#endif
