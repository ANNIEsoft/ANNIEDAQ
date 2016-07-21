#include "Lecroy.h"

Lecroy::Lecroy():Tool(){}


bool Lecroy::Initialise(std::string configfile, DataModel &data)
{
	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_variables.Get("lecroy", DC);		//Module slots DC is TDC or ADC
//	m_variables.Get("perc", perc);		//Module slots
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
	
//	count = 0;
//	sum = 0;
//	times = 0;

	return true;
}


bool Lecroy::Execute()
{
	bool probe = false;
	if (m_data->TRG)
	{
		std::cout << "TRG on!\n" << std::endl;
		m_data->LocalTime = boost::posix_time::microsec_clock::local_time();
//		++count;
//		Log("TRG!\n", 2, verb);

		for (int i = 0; i < m_data->List.CC[DC].size(); i++)
		{
			Data.ch.clear();

			if (m_data->trg_mode == 2) m_data->List.CC[DC].at(i)->InitTest();
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
//				std::cout << "Data\t" << m_data->List.CC[DC].at(i)->GetSlot();
//				std::cout << "\t" << Data.ch.size() << std::endl;
//				sum += count;
//				++times;
//				count = 0;
				m_data->List.Data[DC].Slot.push_back(m_data->List.CC[DC].at(i)->GetSlot());
				m_data->List.Data[DC].Num.push_back(Data);
				m_data->List.CC[DC].at(i)->ClearAll();
			}
		}
	}

	return true;
}


bool Lecroy::Finalise()
{
	Data.ch.clear();
//	std::cout << "data every " << double(sum)/times << " trigger\n" << std::endl;
	return true;
}
