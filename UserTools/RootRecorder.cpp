#include "RootRecorder.h"

RootRecorder::RootRecorder():Tool(){}

bool RootRecorder::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;

	m_variables.Get("TreeCap", TreeCap); 
	m_variables.Get("FileCap", FileCap);
	m_variables.Get("OutPath", OutPath);
	m_variables.Get("OutName", OutName);
	m_variables.Get("StartTime", StartTime);

	FileCount = 0;	
	std::stringstream Tmp;
	OutName = OutPath + OutName;
	Tmp << OutName << "p" << FileCount << ".root";
//	TFile file(Tmp.str().c_str(), "RECREATE", "", 1);
//	file.Write();
//	file.Close();
	
	sPort = new zmq::socket_t(*(m_data->context), ZMQ_REP);
	sPort->connect("inproc://RootWriter");

	std::string Line;
	std::stringstream ssL;
	std::string sEmp;
	int iEmp;

	tree = new TTree("CCData", "CCData");

//Edit starts here ...
//Changed tree, watchout!
	tree->Branch("Trigger", &Trigger, "Trigger/i");
	tree->Branch("OutNumber", &OutN, "OutN/i");
	tree->Branch("Type", &Type);	//T for TDC, A for ADC
	tree->Branch("Value", &Value);
	tree->Branch("Slot", &Slot);
	tree->Branch("Channel", &Channel);
//	tree->Branch("TDC", TDC, "TDC[OutN]/i");
//	tree->Branch("TSlot", &TSlot, "TSlot[OutN]/i");
//	tree->Branch("TChannel", &TChannel, "TChannel[OutN]/i" );
//	tree->Branch("ADC", ADC, "ADC[OutN]/i");
//	tree->Branch("ASlot", &ASlot, "ASlot[OutN]/i");
//	tree->Branch("AChannel", &AChannel, "AChannel[OutN]/i" );
	tree->Branch("TimeStamp", &TimeStamp, "TimeStamp/l" );
//... and ends here

	Trigger = 0;

	Epoch = new boost::posix_time::ptime(boost::gregorian::from_string(StartTime));

	return true;
}

bool RootRecorder::Execute()
{
	if (m_data->TRG)
	{
		boost::posix_time::time_duration Time = m_data->LocalTime - *Epoch;
		TimeStamp = Time.total_milliseconds();
//		std::cout << "TimeStamp " << TimeStamp << std::endl; 	
//		TOutN = 0, AOutN = 0;
//		std::fill(TDC, TDC+512, 0);
//		std::fill(ADC, ADC+512, 0);

		Type.clear();
		Value.clear();
		Slot.clear();
		Channel.clear();
		OutN = 0;

		if (m_data->List.Data.size() > 0)	//There is something to be saved
		{
			in = m_data->List.Data.begin();		//iterates over Module.Data map<type, Cards>

			//loop on Module.Data types, either TDC or ADC
			for (; in != m_data->List.Data.end(); ++in)
			{
				//is = m_data->List.Data["TDC"].Num.begin();
				is = in->second.Num.begin();

				//loop on active Module.Card.Num vector
				for (int i = 0; is != in->second.Num.end(); ++is, ++i)
				{
					OutN += is->ch.size();	//number of channels on
					it = is->ch.begin();

					//loop on active channels
					for (; it != is->ch.end(); ++it)
					{
						Type.push_back(in->first);
						Value.push_back(it->second);
						Channel.push_back(it->first);
						Slot.push_back(in->second.Slot.at(i));
					}
				}
			}

//			i = 0, j = 0;
//			is = m_data->List.Data["ADC"].Num.begin();
//			for (; is != m_data->List.Data["ADC"].Num.end(); ++is, ++i)
//			{
//				it = is->ch.begin();
//				AOutN += is->ch.size();	//number of channels on
//				for (; it != is->ch.end(); ++it, ++j)
//				{
//					*(ASlot+j) = m_data->List.Data["ADC"].Slot.at(i);
//					*(AChannel+j) = it->first;
//					*(ADC+j) = it->second;
//					Type.push_back("ADC");
//					*(Value+j) = it->second;
//					*(Channel+j) = it->first;
//					*(Slot+j) = m_data->List.Data["ADC"].Slot.at(i);
//				}
//			}
		}


//		OutN = (TOutN >= AOutN) ? TOutN : AOutN;
//		OutN = TOutN + AOutN;
	
		if (OutN != 0) tree->Fill();		//Fill
	
		++Trigger;
		m_data->List.Data.clear();		

		zmq::message_t comm;
		if(sPort->recv(&comm, ZMQ_NOBLOCK))
		{
			zmq::message_t message(512);
			std::stringstream tmptree;
			tmptree << "TTree " << tree;
		
			snprintf ((char *) message.data(), 512, "%s", tmptree.str().c_str()) ;
			sPort->send(message);

			tree = new TTree("CCData", "CCData");

			tree->Branch("Trigger", &Trigger, "Trigger/i");
			tree->Branch("OutNumber", &OutN, "OutN/i");
			tree->Branch("Type", &Type);	//T for TDC, A for ADC
			tree->Branch("Value", &Value);
			tree->Branch("Slot", &Slot);
			tree->Branch("Channel", &Channel);
//			tree->Branch("TDC", TDC, "TDC[OutN]/i");
//			tree->Branch("TSlot", &TSlot, "TSlot[OutN]/i");
//			tree->Branch("TChannel", &TChannel, "TChannel[OutN]/i" );
//			tree->Branch("ADC", ADC, "ADC[OutN]/i");
//			tree->Branch("ASlot", &ASlot, "ASlot[OutN]/i");
//			tree->Branch("AChannel", &AChannel, "AChannel[OutN]/i" );
			tree->Branch("TimeStamp", &TimeStamp, "TimeStamp/l" );
		}
	}
	
	return true;
}
	
bool RootRecorder::Finalise()
{
	tree->Delete();
	tree = 0;

	delete sPort;
	sPort = 0;
	
	for (int i = 0; i < m_data->List.CC["TDC"].size(); i++)
		delete m_data->List.CC["TDC"].at(i);
	for (int i = 0; i < m_data->List.CC["ADC"].size(); i++)
		delete m_data->List.CC["ADC"].at(i);

	m_data->List.CC.clear();
	m_data->List.Data.clear();

	delete Epoch;
	Epoch = 0;
	
	return true;
}
