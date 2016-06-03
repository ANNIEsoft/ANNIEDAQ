#include "CCTrigger.h"

//CCTrigger::CCTrigger():Tool(), Lcard(1, "CamacCrate"), Ncard(1, 25) {}
CCTrigger::CCTrigger():Tool() {}

bool CCTrigger::Initialise(std::string configfile, DataModel &data)
{

	if(configfile!="")	m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_variables.Get("verbose", verb);		//Module slots
	m_variables.Get("configcc", configcc);		//Module slots

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

	std::cout << "Number of cards: " << Lcard.size() << std::endl;
//	CC = new CamacCrate;				//CamacCrate at 0
								//must implemented for more CC
	for (int i = 0; i < Lcard.size(); i++)	//CHECK i
	{
		if (Lcard.at(i) == "TDC" || Lcard.at(i) == "ADC")
		{
			m_data->List.CC[Lcard.at(i)].push_back(Create(Lcard.at(i), Ncard.at(i)));	//They use CC at 0
		}

		else if (Lcard.at(i) == "TRG");

		else std::cout << "\n\nUnkown card\n" << std::endl;
	}

	return true;
}


bool CCTrigger::Execute()
{
	if (true)
	{
		m_data->TRG = true;		//20% of full read of card
		std::cout << "Trigger!" << std::endl;
	}
	else m_data->TRG = false;

	return true;
}


bool CCTrigger::Finalise()
{
	Lcard.clear();
	Ncard.clear();

	return true;
}

CamacCrate* CCTrigger::Create(std::string cardname, int cardslot)
{
	CamacCrate* ccp;
	if (cardname == "TDC")
		ccp = new Lecroy3377(cardslot);
	if (cardname == "ADC")
		ccp = new Lecroy4300b(cardslot);
	return ccp;
}
