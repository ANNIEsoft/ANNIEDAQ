#include "CCTrigger.h"

//CCTrigger::CCTrigger():Tool(), Lcard(1, "CamacCrate"), Ncard(1, 25) {}
CCTrigger::CCTrigger():Tool() {}

bool CCTrigger::Initialise(std::string configfile, DataModel &data)
{

	if(configfile!="")	m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_variables.Get("verbose", verb);		//Verbosity
	m_variables.Get("configcc", configcc);		//Module slots
	m_variables.Get("percent", perc);		//firing probability
	m_variables.Get("trg_mode", m_data->trg_mode);	//Trigger mode
	m_data->TRG = false;

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
				ssL >> sEmp;
				Ccard.push_back(sEmp);		//Slot N
			}
		}
	}
	fin.close();

	std::cout << "Number of cards: " << Lcard.size() << std::endl;
//	CC = new CamacCrate;				//CamacCrate at 0

	trg_pos = 0;								//must implemented for more CC
	for (int i = 0; i < Lcard.size(); i++)	//CHECK i
	{
		if (Lcard.at(i) == "TDC" || Lcard.at(i) == "ADC")
		{
			m_data->List.CC[Lcard.at(i)].push_back(Create(Lcard.at(i), Ccard.at(i), Ncard.at(i)));	//They use CC at 0
		}
		else if (Lcard.at(i) == "TRG")
		{
			trg_pos = m_data->List.CC["TDC"].size();
			m_data->List.CC["TDC"].push_back(Create("TDC", Ccard.at(i), Ncard.at(i)));	//They use CC at 0
		}
		else std::cout << "\n\nUnkown card\n" << std::endl;
	}

	std::cout << "Trigger is in slot ";
	std::cout << m_data->List.CC["TDC"].at(trg_pos)->GetSlot() << std::endl;

	srand(time(0));
	return true;
}


bool CCTrigger::Execute()
{
//Clearing all the cards, using iterators
/*
	iL = m_data->List.CC.begin();		//iL is an iterator over a map<string, vector<CamacCrate*> >
	for ( ; iL != m_data->List.CC.end(); ++iL)
	{
		iC = iL->second.begin();	//iC is an iterator over a vector<CamacCrate*>
		for ( ; iC != iL->second.end(); ++iC)
			(*iC)->ClearAll();
	}
*/
/*	
//Clearing all the cards, using indices
	for (int i = 0; i < m_data->List.CC["TDC"].size(); i++)
		m_data->List.CC["TDC"].at(i)->ClearAll();
	for (int i = 0; i < m_data->List.CC["ADC"].size(); i++)
		m_data->List.CC["ADC"].at(i)->ClearAll();
*/

	m_data->TRG = false;
	switch (m_data->trg_mode) //0 is real trg, 1 is soft trg, 2 is with test
	{
		case 0:
			if(m_data->List.CC["TDC"].at(trg_pos)->TestEvent() == 1)
				m_data->TRG = true;
			break;

		case 1:
			m_data->TRG = true;
			break;
		
		case 2:
			m_data->TRG = rand() % int(100/perc) == 0;
			break;
		
		default:
			std::cout << "WARNING: Trigger mode unknown\n" << std::endl;
	}

	return true;
}


bool CCTrigger::Finalise()
{
	Lcard.clear();
	Ncard.clear();

	return true;
}

inline CamacCrate* CCTrigger::Create(std::string cardname, std::string config, int cardslot)
{
	CamacCrate* ccp;
	if (cardname == "TDC")
		ccp = new Lecroy3377(cardslot, config);
	if (cardname == "ADC")
		ccp = new Lecroy4300b(cardslot, config);
	return ccp;
}
