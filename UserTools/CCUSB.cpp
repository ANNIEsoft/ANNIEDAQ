#include "CCUSB.h"

CCUSB::CCUSB():Tool(), Lcard(1, "CamacCrate"), Ncard(1, 25) {}


bool CCUSB::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_variables.Get("lecroy", CN);		//Module slots

	std::ifstream fin (configcc.c_str());
	std::string Line;
	std::stringstream ssL;
	std::string sEmp;
	int iEmp;

	int c = 0;
	while (getline(fin, Line))
	{
		if (Line[0] == '#') continue;
		else
		{
			ssL.str("");
			ssL.clear();
			ssL << Line;
			if (ssL.str() != "")
			{
				ssL >> sEmp;
				Lcard.push_back(sEmp);		//Modeli L
				ssL >> iEmp;
				Ncard.push_back(iEmp);		//Slot N
			}
		}
	}
	fin.close();

	std::cout << "Lcard size " << Lcard.size() << std::endl;
	if (Card.size() != 0) Card.clear();		
	Card.push_back(new CamacCrate);			//CamacCrate at 0

	for (int i = 0; i < Lcard.size(); i++)	//CHECK i
	{
		std::cout << "card " << Lcard.at(i) << std::endl;
		if (Lcard.at(i) == "Lecroy3377")
		{
			Card.push_back(new Lecroy3377(Ncard.at(i), 0));
			L3.push_back(i);
		}
		else if (Lcard.at(i) == "Lecroy4300b")
		{
			Card.push_back(new Lecroy4300b(Ncard.at(i), 0));
			L4.push_back(i);
		}
		else if (Lcard.at(i) != "CamacCrate")
			std::cout << "\n\nUnkown card\n" << std::endl;
	}

	int k;

	//
	
	for (int i = 0; i < L3.size(); i++)
	{
		k = L3.at(i);
		Card.at(k)->ClearAll();
		Card.at(k)->CommonStart() ;
		std::cout << "Reading registers" << std::endl;
		Card.at(k)->GetRegister();
		Card.at(k)->DecRegister();
		Card.at(k)->PrintRegister();
		Card.at(k)->PrintRegRaw();
	}

	for (int i = 0; i < L4.size(); i++)
	{
		k = L4.at(i);
		Card.at(k)->ClearAll();
		int array = 0x6000;
		Card.at(k)->WriteReg(array);
		std::cout << "Reading registers" << std::endl;
		Card.at(k)->GetRegister();
		Card.at(k)->DecRegister();
		Card.at(k)->PrintRegister();
		Card.at(k)->PrintRegRaw();
	}

	srand(time(0));
	return true;
}


bool CCUSB::Execute() {

	int k, tdc = 0, adc = 0;

	for (int i = 0; i < L3.size(); i++)
	{
		Data.clear();
		k = L3.at(i);
		Card.at(k)->ClearAll();
		if (rand() % 5 > 0) continue;
		tdc++;
		Card.at(k)->InitTest();
		if (Card.at(k)->TestEvent() == 1)
			Card.at(k)->ReadFIFOall(Data);
		m_data->L3DM.push_back(Data);
	}
	
	for (int i = 0; i < L4.size(); i++)
	{
		Data.clear();
		k = L4.at(i);
		Card.at(k)->ClearAll();
		if (rand() % 5 > 0) continue;
		adc++;
		Card.at(k)->TestAll();
		while (Card.at(k)->TestLAM())
		Card.at(k)->DumpAll(Data);
		m_data->L4DM.push_back(Data);
	}

	return true;
}


bool CCUSB::Finalise() {

//	Card.at(0)->StopStack();

//	Card.at(0)->ClearFIFO();

	for (int i = 0; i < Card.size(); i++)
		delete Card.at(i);

	Card.clear();
	Lcard.clear();
	Ncard.clear();
	L3.clear();
	L4.clear();

	return true;
}
