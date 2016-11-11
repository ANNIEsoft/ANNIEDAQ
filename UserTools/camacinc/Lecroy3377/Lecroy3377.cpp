#include "CamacCrate.h"
#include "Lecroy3377.h"

Lecroy3377::Lecroy3377(int NSlot, std::string config, int i) : CamacCrate(i)	//Subclass constructor, n of Slot given
{
  //  std::cout<<"l1"<<std::endl;
	Slot.push_back(NSlot);
	//	std::cout<<"l2"<<std::endl;
	ID = Slot.size()-1;
	//std::cout<<"l3"<<std::endl;
	ClearAll();
	//std::cout<<"l4 "<<config<<std::endl; 
	SetConfig(config);
	//std::cout<<"l5"<<std::endl;
	EncRegister();
	//std::cout<<"l6"<<std::endl; 

	if (Common) CommonStart();
	else CommonStop();
	
	//std::cout << "l slot size " << Slot.size() << std::endl;
	//	for (int i = 0; i < Slot.size(); ++i)
	  //	std::cout << "ll " << Slot.at(i) << std::endl;
	  //std::cout<<"l7"<<std::endl; 
}

int Lecroy3377::GetData(std::map<int, int> &mData)
{
  //std::cout<<"L3377 1"<<std::endl;
	int ret = 0;
	//std::cout<<"L3377 2"<<std::endl;
	if (TestEvent() == 1)
		ret = ReadFIFOall(mData);
	//std::cout<<"L3377 3"<<std::endl;
	return ret;
}

int Lecroy3377::ReadFIFOall(std::map<int, int> &mData) //Read FIFO data until end of event, Q = 1 for valid data, Q = 0 at end.
{
  //std::cout<<"ReadFIFO 1"<<std::endl;
	int Data = 0, Chan = 0, Word = 0;
	bool Head;
	int Q = 1, X = 0, ret = 0;
	//std::cout<<"ReadFIFO 2"<<std::endl;
	while (Q == 1)
	{
	  //std::cout<<"ReadFIFO 3"<<std::endl;
		ret = READ(0, 0, Word, Q, X);
		//	std::cout<<"ReadFIFO 4"<<std::endl;
		if (Q == 1) 
		{
		  //std::cout<<"ReadFIFO 5"<<std::endl;
			ParseCompData(Word, Data, Chan, Head);
			//	std::cout<<"ReadFIFO 6"<<std::endl;
			if (!Head)
				mData[Chan] = Data;
			//std::cout<<"ReadFIFO 7"<<std::endl;
		}
		//std::cout<<"ReadFIFO 8"<<std::endl;
	}
	//	std::cout<<"ReadFIFO 9"<<std::endl;
	return ret;
}

//COMMON START ONLY
int Lecroy3377::ReadFIFO()	//F(0)·A(1): Read FIFO data always (Common Start only).
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(1, 0, Data, Q, X);
	return ret;
}

//COMMON START ONLY
int Lecroy3377::ExFIFOOut()	// F(0)·A(2): Examine FIFO output, do not advance FIFO (Common Start only).
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(2, 0, Data, Q, X);
	return ret;
}

int Lecroy3377::ReadReg(int R) //Read Control Register R.
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(R, 1, Data, Q, X);
	if (ret < 0) return ret;
	else return Data;
}

////COMMON START ONLY
int Lecroy3377::ReadTestReg() //F(1)·A(6): Read CAMAC Test Register Common Start only
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(6, 1, Data, Q, X);
	return ret;
}

int Lecroy3377::TestLAM() //Test LAM.
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(0, 8, Data, Q, X);
	return Q;
}

int Lecroy3377::ClearAll() //Clear all data and LAM. This does NOT affect the control registers.
{	
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(0, 9, Data, Q, X);
	return ret;
}

int Lecroy3377::ClearLAM() //Clear LAM.
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(0, 10, Data, Q, X);
	return ret;
}

//COMMON START ONLY
int Lecroy3377::WriteFIFOData() //F(16)·A(0): Write 16-bit data to FIFO (Common Start only).
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(0, 16, Data, Q, X);
	return ret;
}

//COMMON START ONLY
int Lecroy3377::WriteFIFOtag() // F(16)·A(1): Write FIFO tag bit (Common Start only).
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(1, 16, Data, Q, X);
	return ret;
}

int Lecroy3377::WriteReg(int R, int *Data) //Write Control Register R.
{
	int Q = 0, X = 0;
	return WRITE(R, 17, *Data, Q, X);
}

int Lecroy3377::DisLAM() //Disable LAM.
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(0, 24, Data, Q, X);
	return ret;
}

int Lecroy3377::DisAcq() //Disable Acquisition Mode.
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(1, 24, Data, Q, X);
	return ret;
}

//COMMON START ONLY
int Lecroy3377::InitTest() //F(25)·A(0): Initiate test cycle (Common Start only).
{
	int ret;
	if (!TestFlag)
	{
		std::cout << "WARNING: Test flag not set in register\n" << std::endl;
		ret = -1;
	}
	else
	{
		int Data = 0;
		int Q = 0, X = 0;
		ret = READ(0, 25, Data, Q, X);
	}
	return ret;
}

int Lecroy3377::EnLAM() //Enable LAM.
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(0, 26, Data, Q, X);
	return ret;
}

int Lecroy3377::EnAcq() //Enable Acquisition Mode.
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(1, 26, Data, Q, X);
	return ret;
}

int Lecroy3377::TestBuff()	//Test buffering in progress (BIP), Q = 1 while BIP.
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(0, 27, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy3377::TestBusy()	//Test busy, Q = 1 while busy.
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(1, 27, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy3377::TestEvent()	//Test event ready, Q = 1 if event ready for readout.
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(2, 27, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy3377::TestFIFO()	//Test FIFO tag bit, Q = 1 if tag bit set for word to be read next.
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(3, 27, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy3377::CommonStop()	//F(30): Begin the reprogramming sequence for Common Stop
{
	int ret = 0;
	int Data = 0;
	int Q = 0, X = 0;

	ret = READ(0, 30, Data, Q, X);
	ret = ClearAll();
	ret = READ(0, 25, Data, Q, X);

	usleep(500000);
	do
		ret = READ(0, 13, Data, Q, X);
	while (Q != 1);
	ret = ClearAll();

	std::cout << "Common Stop programming complete" << std::endl;

	SetRegister();

	if (ret < 0) return ret;
	else return Q;
}

int Lecroy3377::CommonStart()	//F(30): Begin the reprogramming sequence for Common Start
{
	int Data = 0;
	int Q = 0, X = 0;
	int ret = 0;

	ret = READ(0, 9, Data, Q, X);
	ret = READ(0, 30, Data, Q, X);
	Data = 0;
	ret = WRITE(0, 21, Data, Q, X);
	ret = READ(0, 25, Data, Q, X);

	usleep(500000);
	do
		ret = READ(0, 13, Data, Q, X);
	while (Q != 1);
	ret = ClearAll();

	std::cout << "Common Start programming complete " << std::endl;

	SetRegister();

	ret = EnLAM();
	ret = EnAcq();

	if (ret < 0) return ret;
	else return Q;
	std::cout << "common routine over" << std::endl;
}

int Lecroy3377::READ(int F, int A, int &Data, int &Q, int &X)	//Generic READ
{
	long lData;
	int ret = CamacCrate::READ(GetID(), F, A, lData, Q, X);
	Data = lData;
	return ret;
}

int Lecroy3377::WRITE(int F, int A, int &Data, int &Q, int &X)	//Gneric WRITE
{
	long lData = long(Data);
	return CamacCrate::WRITE(GetID(), F, A, lData, Q, X);
}

int Lecroy3377::GetID()		//Return ID of module
{
	return ID;
}

int Lecroy3377::GetSlot()	//Return n of Slot of module
{
	return Slot.at(GetID());
}

void Lecroy3377::ParseCompData(int Word, int &Stat, int &Num, bool &B1)
{
	std::bitset<16> bWord(Word);
	B1 = bWord.test(15);
	Num = BittoInt(bWord, 10, 14);
	Stat = BittoInt(bWord, 0, 9);
}

void Lecroy3377::GetRegister()
{
	int n_reg = 0;
	if (Common) n_reg = 6;
	else n_reg = 4;
	for (int i = 0; i < n_reg; i++)
		Control[i] = ReadReg(i);
}

void Lecroy3377::SetRegister()	//Common stop only
{
	int ret, n_reg = 0;
	if (Common) n_reg = 6;
	else n_reg = 4;
	for (int i = 0; i < n_reg; i++)
		ret = WriteReg(i, Control+i);
}

void Lecroy3377::DecRegister()
{
	std::bitset<16> breg0(Control[0]);
	std::bitset<16> breg1(Control[1]);
	std::bitset<16> breg2(Control[2]);
	std::bitset<16> breg3(Control[3]);
	std::bitset<16> breg4(Control[4]);
	std::bitset<16> breg5(Control[5]);

	ModID = BittoInt(breg0, 0, 7);		
	TDCRes = BittoInt(breg0, 8, 9);
        RecEdges = breg0.test(10);		
        ReadoutMode = breg0.test(11);		
        BuffMode = breg0.test(12);		
        HeaderMode = breg0.test(13);		
        Mode = BittoInt(breg0, 14, 15);		
	
	if (Common != (Mode % 2)) std::cout << "WARNING: Something is wrong with the registers\n" << std::endl;
	else
	{
		if (Common)
		{
			MPI = BittoInt(breg1, 10, 11);		
			FFERAmode = breg1.test(12); 		
			EvSerNum = BittoInt(breg1, 13, 15);	
		
			MaxHITS = BittoInt(breg2, 0, 3);	
			
			ReqDelay = BittoInt(breg3, 0, 3);	
			MaxTimeRange = BittoInt(breg3, 4, 15);	
		
			StartTO = BittoInt(breg4, 0, 9);	
		
			TestPulse = BittoInt(breg5, 0, 4);	
			TestClock = BittoInt(breg5, 5, 6);	
			TestFlag = breg5.test(8);		
		}
		else
		{
			TrgWidth = BittoInt(breg1, 0, 3);	
			TrgDelay = BittoInt(breg1, 4, 7);	
			TrgClock = BittoInt(breg1, 8, 9);	
			MPI = BittoInt(breg1, 10, 11);		
			FFERAmode = breg1.test(12); 		
			EvSerNum = BittoInt(breg1, 13, 15);	
	
			MaxHITS = BittoInt(breg2, 0, 3);	
			FullScale = BittoInt(breg2, 4, 15);	
		
			ReqDelay = BittoInt(breg3, 0, 3);	
			OffSet = BittoInt(breg3, 4, 15);	
		}
	}
}

void Lecroy3377::StartTestReg()
{
	int Data = 0x10ff;
	WriteReg(0, &Data);	
	Data = 0x0000;
	WriteReg(1, &Data);	
	Data = 0x0000;
	WriteReg(2, &Data);	
	Data = 0xfff0;
	WriteReg(3, &Data);	
	Data = 0x03fe;
	WriteReg(4, &Data);	
	Data = 0x0101;
	WriteReg(5, &Data);
}

void Lecroy3377::StopTestReg()
{
	int Data = 0x10ff;
	WriteReg(0, &Data);	
	Data = 0x00ff;
	WriteReg(1, &Data);	
	Data = 0x03f0;
	WriteReg(2, &Data);	
	Data = 0x0000;
	WriteReg(3, &Data);	
}

void Lecroy3377::EncRegister()
{
	Control[0] = 0;
	Control[0] = Control[0] << 1;
	Control[0] += HeaderMode;
	Control[0] = Control[0] << 1;
	Control[0] += BuffMode;
	Control[0] = Control[0] << 1;
	Control[0] += ReadoutMode;
	Control[0] = Control[0] << 1;
	Control[0] += RecEdges;
	Control[0] = Control[0] << 2;
	Control[0] += TDCRes;
	Control[0] = Control[0] << 8;
	Control[0] += ModID;

	if (Common)
	{
		Control[1] = 0;
		Control[1] += EvSerNum;
		Control[1] = Control[1] << 1;
		Control[1] += FFERAmode;
		Control[1] = Control[1] << 2;
		Control[1] += MPI;
		Control[1] = Control[1] << 10;
	
		Control[2] = 0;
		Control[2] = Control[2] << 4;
		Control[2] += MaxHITS;
	
		Control[3] = 0;
		Control[3] += MaxTimeRange;
		Control[3] = Control[3] << 4;
		Control[3] += ReqDelay;
	
		Control[4] = 0;
		Control[4] = Control[4] << 10;
		Control[4] += StartTO;
	
		Control[5] = 0;
		Control[5] = Control[5] << 1;
		Control[5] += TestFlag;	
		Control[5] = Control[5] << 3;
		Control[5] += TestClock;
		Control[5] = Control[5] << 5;
		Control[5] += TestPulse;
	}
	else
	{
		Control[1] = 0;
		Control[1] += EvSerNum;
		Control[1] = Control[1] << 1;
		Control[1] += FFERAmode;
		Control[1] = Control[1] << 2;
		Control[1] += MPI;
		Control[1] = Control[1] << 2;
		Control[1] += TrgClock;
		Control[1] = Control[1] << 4;
		Control[1] += TrgDelay;
		Control[1] = Control[1] << 4;
		Control[1] += TrgWidth;
	
		Control[2] = 0;
		Control[2] += FullScale;
		Control[2] = Control[2] << 4;
		Control[2] += MaxHITS;
	
		Control[3] = 0;
		Control[3] += ReqDelay;
		Control[3] = Control[3] << 4;
		Control[3] += OffSet;
	}
}

void Lecroy3377::PrintRegRaw()
{
	int n_reg;
	if (Common)
	{
		n_reg = 6;
		std::cout << "Lecroy 3377 in Common Start" << std::endl;
	}
	else 
	{
		n_reg = 4;
		std::cout << "3377 in Common Stop" << std::endl;
	}
	for (int i = 0; i < n_reg; i++)
		std::cout << "Reg " << i << ":\t" << std::hex << Control[i] << std::endl;
	std::cout << std::dec << std::endl;
}

void Lecroy3377::PrintRegister()
{	
//	DecRegister();
	std::cout << "[" << GetID() << "] :\t";
	std::cout << "Lecroy3377 in slot " << GetSlot() << " register control" << std::endl;

	std::cout << "- Module ID code\t" << ModID << std::endl; 
	std::cout << "- TDC resolution\t" << TDCRes << std::endl; 
	std::cout << "- Record Edges\t" << RecEdges << std::endl; 
	std::cout << "- Readout Mode\t" << ReadoutMode << std::endl; 
	std::cout << "- Buffer Mode\t" << BuffMode << std::endl; 
	std::cout << "- Header Mode\t" << HeaderMode << std::endl; 
	std::cout << "- Mode in use\t" << Mode << std::endl; 

	if (Common)
	{
		std::cout << "- MPI\t\t" << MPI << std::endl; 
		std::cout << "- Fast FERA\t" << FFERAmode << std::endl; 
		std::cout << "- Event Serial\t" << EvSerNum << std::endl; 
		std::cout << "- Max HITS\t\t" << MaxHITS << std::endl; 
		std::cout << "- Request Delay\t" << ReqDelay << std::endl; 
		std::cout << "- Max Time Range\t" << MaxTimeRange << std::endl; 
		std::cout << "- Start Timeout\t" << StartTO << std::endl; 
		std::cout << "- Pulse number\t" << TestPulse << std::endl; 
		std::cout << "- Test mode clk\t" << TestClock << std::endl; 
		std::cout << "- Test Enable\t" << TestFlag << std::endl; 
	}
	else
	{	
		std::cout << "- Trg Width\t"  << TrgWidth << std::endl; 
		std::cout << "- Trg Delay\t" << TrgDelay << std::endl; 
		std::cout << "- Trg Clock\t" << TrgClock << std::endl; 
		std::cout << "- MPI\t\t" << MPI << std::endl; 
		std::cout << "- Fast FERA\t" << FFERAmode << std::endl; 
		std::cout << "- Event Serial\t" << EvSerNum << std::endl; 
		std::cout << "- Max HITS\t\t" << MaxHITS << std::endl; 
		std::cout << "- Full Scale\t" << FullScale << std::endl; 
		std::cout << "- Request Delay\t" << ReqDelay << std::endl; 	
		std::cout << "- Offset\t" << OffSet << std::endl; 	
	}

	std::cout << std::endl; 
}

void Lecroy3377::SetConfig(std::string config)
{
	std::ifstream fin (config.c_str());
	std::string Line;
	std::stringstream ssL;

	std::string sEmp;
	int iEmp;
	//std::cout<<"conf 1"<<std::endl;
	while (getline(fin, Line))
	{
	  //std::cout<<"conf 2"<<std::endl;
		if (Line.empty()) continue;
		if (Line[0] == '#') continue;
		else
		{
		  //	  std::cout<<"conf 3"<<std::endl;
			Line.erase(Line.begin()+Line.find('#'), Line.end());
			//	std::cout<<"conf 3.5"<<std::endl;
			ssL.str("");
			ssL.clear();
			ssL << Line;
			if (ssL.str() != "")
			{
			  //std::cout<<"conf 4"<<std::endl;
				ssL >> sEmp >> iEmp;

				if (sEmp == "Common") Common = iEmp;

				if (sEmp == "ModID") ModID = iEmp;
				if (sEmp == "TDCRes") TDCRes = iEmp;
				if (sEmp == "RecEdges") RecEdges = iEmp;
				if (sEmp == "ReadoutMode") ReadoutMode = iEmp;
				if (sEmp == "BuffMode") BuffMode = iEmp;
				if (sEmp == "HeaderMode") HeaderMode = iEmp;
				if (sEmp == "MPI") MPI = iEmp;
				if (sEmp == "FFERAmode") FFERAmode = iEmp;
				if (sEmp == "EvSerNum") EvSerNum = iEmp;
				if (sEmp == "TrgWidth") TrgWidth = iEmp;
				if (sEmp == "TrgDelay") TrgDelay = iEmp;
				if (sEmp == "TrgClock") TrgClock = iEmp;
				if (sEmp == "MaxHITS") MaxHITS = iEmp;
				if (sEmp == "FullScale") FullScale = iEmp;
				if (sEmp == "ReqDelay") ReqDelay = iEmp;
				if (sEmp == "OffSet") OffSet = iEmp;
				if (sEmp == "MaxTimeRange") MaxTimeRange = iEmp;
				if (sEmp == "StartTO") StartTO = iEmp;
				if (sEmp == "TestPulse") TestPulse = iEmp;
				if (sEmp == "TestClock") TestClock = iEmp;
				if (sEmp == "TestFlag") TestFlag = iEmp;
			}
		}
	}
	//std::cout<<"conf 8"<<std::endl;
	fin.close();
	//std::cout<<"conf 9"<<std::endl;
}
