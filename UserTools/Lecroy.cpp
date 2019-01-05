#include "Lecroy.h"

Lecroy::Lecroy():Tool(){}


bool Lecroy::Initialise(std::string configfile, DataModel &data)
{
	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_variables.Get("lecroy", DC);		//Module slots DC is TDC or ADC
	m_variables.Get("verb", verb);	
	std::string reg;
	std::stringstream ssreg;


	Log("Clearing modules and printing the registers\n", 1, verb);

	for (int i = 0; i < m_data->MRDdata.List.CC[DC].size(); i++)
	{
		m_data->MRDdata.List.CC[DC].at(i)->ClearAll();
		m_data->MRDdata.List.CC[DC].at(i)->GetRegister();
		m_data->MRDdata.List.CC[DC].at(i)->PrintRegRaw();
	}
	
//	count = 0;
//	sum = 0;
//	times = 0;

	return true;
}


bool Lecroy::Execute()
{
  //std::cout<<"L1"<<std::endl;
	bool probe = false;
	if (m_data->MRDdata.TRG)
	{
	  // //std::cout<<"L2"<<std::endl;

	  //std::cout << "TRG on!\n" << std::endl;
		m_data->MRDdata.LocalTime = boost::posix_time::microsec_clock::local_time();
//		++count;
//		Log("TRG!\n", 2, verb);
		//std::cout<<"L3"<<std::endl;

		for (int i = 0; i < m_data->MRDdata.List.CC[DC].size(); i++)
		{
		  //std::cout<<"L4"<<std::endl;

			Data.ch.clear();
			//std::cout<<"L5"<<std::endl;

			if (m_data->MRDdata.trg_mode == 2) m_data->MRDdata.List.CC[DC].at(i)->InitTest();
			//std::cout<<"L5.5"<<std::endl;
			m_data->MRDdata.List.CC[DC].at(i)->GetData(Data.ch);
			//std::cout<<"L6"<<std::endl;

			if (Data.ch.size() != 0)
			{
			  //		  std::cout<<"L7"<<std::endl;

				m_data->MRDdata.List.Data[DC].Slot.push_back(m_data->MRDdata.List.CC[DC].at(i)->GetSlot());
				m_data->MRDdata.List.Data[DC].Num.push_back(Data);
				m_data->MRDdata.List.CC[DC].at(i)->ClearAll();
				//std::cout<<"L8"<<std::endl;

			}
			//std::cout<<"L9"<<std::endl;

		}
		//std::cout<<"L10"<<std::endl;

	}
	//std::cout<<"L11"<<std::endl;

	return true;
}


bool Lecroy::Finalise()
{
	Data.ch.clear();
//	std::cout << "data every " << double(sum)/times << " trigger\n" << std::endl;
	return true;
}
