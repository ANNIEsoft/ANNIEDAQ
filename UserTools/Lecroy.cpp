#include "Lecroy.h"

Lecroy::Lecroy():Tool(){}


bool Lecroy::Initialise(std::string configfile, DataModel &data)
{
	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_variables.Get("lecroy", DC);		//Module slots DC is TDC or ADC
	m_variables.Get("perc", perc);		//Module slots
	m_variables.Get("verb", verb);		//Module slots
	std::string reg;
	std::stringstream ssreg;


	Log("Clearing modules and printing the registers\n", 1, verb);

	for (int i = 0; i < m_data->List.CC[DC].size(); i++)
	{
		m_data->List.CC[DC].at(i)->ClearAll();
		m_data->List.CC[DC].at(i)->GetRegister();
		m_data->List.CC[DC].at(i)->PrintRegRaw();
	}
	
	srand(time(0));

	return true;
}


bool Lecroy::Execute()
{
	bool probe;
	if (m_data->TRG)
	{
		for (int i = 0; i < m_data->List.CC[DC].size(); i++)
		{
			Data.ch.clear();
			if (m_data->trg_mode)
				probe = true;
			else probe = rand() % (100/perc) == 0;

			if (probe)
			{

				if (!m_data->trg_mode) m_data->List.CC[DC].at(i)->InitTest();
				m_data->List.CC[DC].at(i)->GetData(Data.ch);

/////////////////////////////////////////////////////////////////////////////////////////////////////
/*
				if (DC == "TDC")
				{
					if (!m_data->trg_mode) m_data->List.CC[DC].at(i)->InitTest();
					if (m_data->List.CC[DC].at(i)->TestEvent() == 1)
						m_data->List.CC[DC].at(i)->ReadFIFOall(Data.ch);
				}
	
				else if (DC == "ADC")
				{
					if (!m_data->trg_mode) m_data->List.CC[DC].at(i)->TestAll();
					while (m_data->List.CC[DC].at(i)->TestLAM())
						m_data->List.CC[DC].at(i)->DumpAll(Data.ch);
				}

*/
				if (Data.ch.size() != 0)
				{
					m_data->List.Data[DC].Slot.push_back(m_data->List.CC[DC].at(i)->GetSlot());
					m_data->List.Data[DC].Num.push_back(Data);
				}
			}
		}
	}

	return true;
}


bool Lecroy::Finalise()
{
	Data.ch.clear();
	return true;
}
