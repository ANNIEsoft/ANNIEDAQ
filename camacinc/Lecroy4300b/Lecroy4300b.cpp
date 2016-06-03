#include "Lecroy4300b.h"

Lecroy4300b::Lecroy4300b(int NSlot, int i) : CamacCrate(i)
{
	Slot.push_back(NSlot);
	ID = Slot.size()-1;
}

int Lecroy4300b::ReadReg(int &Data)		//Read status word register. Q = 1 if BUSY = 0.
{
	int Q = 0, X = 0;
	int ret = READ(0, 0, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy4300b::ReadPed(int Ch, int &Data)	//Read pedestal memory (8 bits) for the 16 channels. Q = 1 if BUSY = 0.
{
	int Q = 0, X = 0;
	int ret = READ(Ch, 1, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy4300b::ReadOut(int &Data, int Ch = 0)	//Random access or sequential readout of the 16 ADC values. Q = 1 if BUSY = 1.
{
	int Q = 0, X = 0;
	int ret = READ(Ch, 2, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy4300b::TestLAM()	//Test LAM. Q = 1 if LAM is present.
{	
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(0, 8, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy4300b::ClearAll()	//Clear Module (not pedestal?)
{	
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(0, 9, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy4300b::ClearLAM()		//Test and clear LAM, if present.
{	
	int Data = 0;
	int Q = 0, X = 0;
	int ret = READ(0, 10, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy4300b::WriteReg(int &Data)	//Write status word register. Q = 1 if BUSY = 0.
{
	int Q = 0, X = 0;
	int ret = WRITE(0, 16, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy4300b::WritePed(int Ch, int &Data)	//Write pedestal memory (8 bits) for the 16 channels. Q = 1 if BUSY = 0.
{
	int Q = 0, X = 0;
	int ret = WRITE(Ch, 17, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy4300b::TestAll()	//Enable test. Q = 1 if BUSY = 0. 
{	
	int Data = 0;
	int Q = 0, X = 0;

//	Control = 0x6000;
//	SetRegister();

	int ret = READ(0, 25, Data, Q, X);
	if (ret < 0) return ret;
	else return Q;
}

int Lecroy4300b::READ(int F, int A, int &Data, int &Q, int &X)	//Generic READ
{
	long lData;
	int ret = CamacCrate::READ(GetID(), F, A, lData, Q, X);
	Data = lData;
	return ret;
}

int Lecroy4300b::WRITE(int F, int A, int &Data, int &Q, int &X)	//Gneric WRITE
{
	long lData = long(Data);
	return CamacCrate::WRITE(GetID(), F, A, lData, Q, X);
}

int Lecroy4300b::GetData(std::map<int, int> &mData) 
{
	int ret;
	if (ECE || CCE) ret = DumpCompressed(mData);
	else ret = DumpAll(mData);
	return ret;
}

int Lecroy4300b::DumpCompressed(std::map<int, int> &mData) 
{
	mData.clear();
	int Data = 0, Chan = 0, Word = 0;
	bool Head;
	int ret = ReadOut(Word);
	ParseCompData(Word, Data, Chan, Head);
	if (Head)
	{
		for (int i = 0; i < Chan; i++)
		{
			ret = ReadOut(Data);
			ParseCompData(Word, Data, Chan, Head);
			mData[Chan] = Data;
		}
		return mData.size();
	}
	else return -1;
}

int Lecroy4300b::DumpAll(std::map<int, int> &mData)
{
	mData.clear();
	int Data = 0;
	int ret;
	for (int i = 0; i < 16; i++)
	{
		ret = ReadOut(Data, i);
		mData[i] = Data;
	}
	return ret * mData.size();
}

void Lecroy4300b::DecRegister()
{
	std::bitset<16> breg(Control);
	VSN = BittoInt(breg, 0, 7);	//b0
	EPS = breg.test(8); 		//b1
	ECE = breg.test(9);		//b2
	EEN = breg.test(10);		//b3
	CPS = breg.test(11);		//b4
	CCE = breg.test(12);		//b5
	CSR = breg.test(13);		//b6
	CLE = breg.test(14);		//b7
	OFS = breg.test(15);		//b8
}

void Lecroy4300b::EncRegister()
{
	Control = 0;
	Control += OFS;
	Control << 1;
	Control += CLE;
	Control << 1;
	Control += CSR;
	Control << 1;
	Control += CCE;
	Control << 1;
	Control += CPS;
	Control << 1;
	Control += EEN;
	Control << 1;
	Control += ECE;
	Control << 1;
	Control += EPS;
	Control << 8;
	Control += VSN;

	std::cout << "Register is " << std::hex << Control;
	std::cout << std::dec << std::endl;
}

void Lecroy4300b::GetRegister()
{
	int ret = ReadReg(Control);
}

void Lecroy4300b::SetRegister()
{
	int ret = WriteReg(Control);
}

void Lecroy4300b::PrintRegRaw()
{
	std::cout << "Lecroy 4300b\nReg:\t" << std::hex;
	std::cout << Control << std::dec << std::endl;
}

void Lecroy4300b::PrintRegister()
{
	DecRegister();
	std::cout << "[" << GetID() << "] :\t";
	std::cout << "Lecroy4300b in slot " << GetSlot() << " register control" << std::endl;
	std::cout << "VSN " << VSN << std::endl;
	std::cout << "EPS " << EPS << std::endl;
	std::cout << "ECE " << ECE << std::endl;
	std::cout << "EEN " << EEN << std::endl;
	std::cout << "CPS " << CPS << std::endl;
	std::cout << "CCE " << CCE << std::endl;
	std::cout << "CSR " << CSR << std::endl;
	std::cout << "CLE " << CLE << std::endl;
	std::cout << "OFS " << OFS << std::endl;
	std::cout << std::endl; 
}

void Lecroy4300b::ParseCompData(int Word, int &Stat, int &Num, bool &B1)
{
	std::bitset<16> bWord(Word);
	B1 = bWord.test(15);
	Num = BittoInt(bWord, 11, 14);
	Stat = BittoInt(bWord, 0, 10);
}

int Lecroy4300b::GetPedestal()
{
	int ret;
	for (int i = 0; i < 16; i++)
		ret = ReadPed(i, Ped[i]);
	return ret;
}

int Lecroy4300b::SetPedestal()
{
	int ret;
	for (int i = 0; i < 16; i++)
		ret = WritePed(i, Ped[i]);
	return ret;
}

int Lecroy4300b::GetPedFile(std::string fname)
{
	std::ofstream fout(fname.c_str());
	int ret = GetPedestal();
	std::cout << "#Pedestal saved on ";
	std::time_t now = std::time(NULL);
	std::cout << std::asctime(std::localtime(&now)) << std::endl;	
	for (int i = 0; i < 16; i++)
		fout << Ped[i] << std::endl;
	return ret;
}

int Lecroy4300b::SetPedFile(std::string fname)
{
	std::ifstream fin(fname.c_str());
	std::string line;
	std::stringstream ss;
	int ped;
	std::vector<int> vPed;
	while (getline(fin, line))
	{
		if (line[0] == '#') continue;
		ss.str("");
		ss.clear();
		ss << line;
		ss >> ped;
		vPed.push_back(ped);
	}
	if (vPed.size() == 16) 
		for (int i = 0; i < 16; i++)
			Ped[i] = vPed.at(i);
	return SetPedestal();
}

int Lecroy4300b::GetID()	//Return ID of module
{
	return ID;
}

int Lecroy4300b::GetSlot()	//Return n of Slot of module
{
	return Slot.at(GetID());
}
