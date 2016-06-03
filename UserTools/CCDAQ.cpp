#include "CCDAQ.h"

CCDAQ::CCDAQ():Tool(){}


bool CCDAQ::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_variables.Get("configcc", configcc);		//Module slots
	m_variables.Get("stackfile", stackfile);	//Stack file

	std::ifstream fin (configcc.c_str());
	std::string Line;
	std::stringstream ssL;
	std::string sEmp;
	int iEmp;

	while (getline(fin, Line))
	{
		if (Line[0] == '#') continue;
		else
		{
			ssL.str("");
			ssL.clear();
			ssL << Line;
			ssL >> sEmp;
			Lcard.push_back(sEmp);
			ssL >> iEmp;
			Ncard.push_back(iEmp);
		}
	}
	fin.close();

	if (Card.size() != 0) Card.clear();		
	Card.push_back(new CamacCrate);			//CamacCrate at 0

	for (int i = 0; i < Lcard.size(); i++)
	{
		if (Lcard.at(i) == "Lecroy3377")
		{
			Card.push_back(new Lecroy3377(Ncard.at(i), 0));
			L3377.push_back(i);
		}
		else if (Lcard.at(i) == "Lecroy4300b")
		{
			Card.push_back(new Lecroy4300b(Ncard.at(i), 0));
			L4300.push_back(i);
		}
		else std::cout << "\n\nUnkown card\n\n";
	}

	long mask = 0;
	Card.at(0)->SetLAMmask(mask);
	Card.at(0)->GlobalRegister(mask);	
	Card.at(0)->EncStack(stackfile);
	Card.at(0)->PushStack();

	int k;
	for (int i = 0; i < L3377.size(); i++)
	{
		k = L3377.at(i);
		Card.at(k)->CommonStart() ;
	}

	for (int i = 0; i < L4300.size(); i++)
	{
		k = L3377.at(i);
		Card.at(k)->ClearAll();
		int array = 0x6000;
		Card.at(k)->WriteReg(array);
	}

	for (int i = 1; i < Card.size(); i++)
	{
		std::cout << "...Analysing card in slot " << Ncard.at(i) << std::endl;
		Card.at(i)->GetRegister();
		Card.at(i)->DecRegister();
		Card.at(i)->PrintRegRaw();
	}

	Card.at(0)->StartStack();

	return true;
}


bool CCDAQ::Execute() {

	vWord.clear();
	Card.at(0)->ReadFIFO(vWord);
	for (int i = 0; i < vWord.size(); i++)
	{
//		m_data->CCdata.push_back(vWord.at(i));
	}

	return true;
}


bool CCDAQ::Finalise() {

        Card.at(0)->StopStack();

	Card.at(0)->ClearFIFO();

	for (int i = 0; i < Card.size(); i++)
		delete Card.at(i);

	return true;
}
