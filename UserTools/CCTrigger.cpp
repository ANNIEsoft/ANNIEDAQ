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
	m_variables.Get("trg_mode", m_data->MRDdata.trg_mode);	//Trigger mode
	m_data->MRDdata.TRG = false;

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
				Ccard.push_back(sEmp);		//Slot register file
			}
		}
	}
	fin.close();

	std::cout << "Number of cards: " << Lcard.size() << std::endl;
//	CC = new CamacCrate;				//CamacCrate at 0

	trg_pos = 0;								//must implemented for more CC
	for (int i = 0; i < Lcard.size(); i++)	//CHECK i
	{
	  std::cout << "for begin " <<Lcard.at(i)<< std::endl;
		if (Lcard.at(i) == "TDC" || Lcard.at(i) == "ADC")
		{
		  //std::cout << "d1 " << Ccard.at(i) << " " << Ncard.at(i) <<std::endl; 
			m_data->MRDdata.List.CC[Lcard.at(i)].push_back(Create(Lcard.at(i), Ccard.at(i), Ncard.at(i)));	//They use CC at 0
			//	std::cout << "d2 "<<std::endl;
		}
		else if (Lcard.at(i) == "TRG")
		{
		  //	  std::cout << "d3 "<<std::endl;
			trg_pos = m_data->MRDdata.List.CC["TDC"].size();
			std::cout << "d4 "<<std::endl;			
			m_data->MRDdata.List.CC["TDC"].push_back(Create("TDC", Ccard.at(i), Ncard.at(i)));	//They use CC at 0
			std::cout << "d5 "<<std::endl;
		}
		else std::cout << "\n\nUnkown card\n" << std::endl;
		std::cout << "for over " << std::endl;
	}

	std::cout << "Trigger is in slot ";
	std::cout << m_data->MRDdata.List.CC["TDC"].at(trg_pos)->GetSlot() << std::endl;

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

	m_data->MRDdata.TRG = false;
	switch (m_data->MRDdata.trg_mode) //0 is real trg, 1 is soft trg, 2 is with test
	{
		case 0:
		  if(m_data->MRDdata.List.CC["TDC"].at(trg_pos)->TestEvent() == 1){
		    //std::cout<<"triggered 0"<<std::endl;
		    m_data->MRDdata.TRG = true;}
			break;

		case 1:
			m_data->MRDdata.TRG = true;
			//	std::cout<<"triggered 1"<<std::endl;
			break;
		
		case 2:
			m_data->MRDdata.TRG = rand() % int(100/perc) == 0;
			//		if ( m_data->MRDdata.TRG)  std::cout<<"triggered 2"<<std::endl;
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
	{
	  std::cout<<"TDC"<<std::endl;
		ccp = new Lecroy3377(cardslot, config);
	}
	if (cardname == "ADC")
	{
	  std::cout<<"ADC"<<std::endl;
		ccp = new Lecroy4300b(cardslot, config);
	}
	return ccp;
}
