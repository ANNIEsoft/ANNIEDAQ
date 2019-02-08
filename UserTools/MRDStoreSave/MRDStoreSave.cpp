#include "MRDStoreSave.h"

MRDStoreSave::MRDStoreSave():Tool(){}

bool MRDStoreSave::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
 
	m_variables.Get("FileCap", FileCap);
	m_variables.Get("OutPath", OutPath);
	m_variables.Get("OutName", OutName);
	m_variables.Get("StartTime", StartTime);
	m_variables.Get("MonitorFrequency",MonitorFrequency);

	FileCount = 0;	
	std::stringstream Tmp;
	OutName = OutPath + OutName;
	Tmp << OutName << "p" << FileCount << ".Store";
//	TFile file(Tmp.str().c_str(), "RECREATE", "", 1);
//	file.Write();
//	file.Close();
	
	//std::cout<<"MRD: attempting connect"<<std::endl;
	//	sPort = new zmq::socket_t(*(m_data->context), ZMQ_REP);
	//sPort->connect("inproc://MRDTree");		//final use
	//	sPort->connect("ipc:///tmp/0");			//test only
	//std::cout<<"MRD: connected"<<std::endl;

	//	std::string Line;
	//std::stringstream ssL;
	//std::string sEmp;
	//int iEmp;

	CCData=new BoostStore(false, 2);
	//	tree = new TTree("CCData", "CCData");

//Edit starts here ...
//Changed tree, watchout!
/*	tree->Branch("Trigger", &Trigger, "Trigger/i");
	tree->Branch("OutNumber", &OutN, "OutN/i");
	tree->Branch("Type", &Type);	//T for TDC, A for ADC
	tree->Branch("Value", &Value);
	tree->Branch("Slot", &Slot);
	tree->Branch("Channel", &Channel);
	tree->Branch("TimeStamp", &TimeStamp, "TimeStamp/l" );
//... and ends here
*/
	Trigger = 0;

	Epoch = new boost::posix_time::ptime(boost::gregorian::from_string(StartTime));

	return true;
}

bool MRDStoreSave::Execute()
{
  //  std::cout<<"E1"<<std::endl;
  if (m_data->MRDdata.TRG)
    {
      //      std::cout<<"positive trigger"<<std::endl;
      //	  std::cout<<"E2"<<std::endl;
      boost::posix_time::time_duration Time = m_data->MRDdata.LocalTime - *Epoch;
      m_data->MRDout.TimeStamp = Time.total_milliseconds();
      //		std::cout << "TimeStamp " << TimeStamp << std::endl; 	
      //		TOutN = 0, AOutN = 0;
      //		std::fill(TDC, TDC+512, 0);
      //		std::fill(ADC, ADC+512, 0);
      //	std::cout<<"E3"<<std::endl;
      m_data->MRDout.Type.clear();
      m_data->MRDout.Value.clear();
      m_data->MRDout.Slot.clear();
      m_data->MRDout.Crate.clear();
      m_data->MRDout.Channel.clear();
      m_data->MRDout.OutN = 0;
      //std::cout<<"E4"<<std::endl;
      m_data->MRDout.Trigger=Trigger;
      // std::cout<<"E5 size="<<m_data->MRDdata.List.Data.size()<<std::endl;
            
      if (m_data->MRDdata.List.Data.size() > 0)	//There is something to be saved
	{
	  //  std::cout<<"E6 "<<std::endl;
	  in = m_data->MRDdata.List.Data.begin();		//iterates over Module.Data map<type, Cards>
        
	  //loop on Module.Data types, either TDC or ADC
	  for (; in != m_data->MRDdata.List.Data.end(); ++in)
	    {
	      //is = m_data->MRDdata.List.Data["TDC"].Num.begin();
	      is = in->second.Num.begin();
	      
	      //loop on active Module.Card.Num vector
	      for (int i = 0; is != in->second.Num.end(); ++is, ++i)
		{
		  m_data->MRDout.OutN += is->ch.size();	//number of channels on
		  it = is->ch.begin();
		  
		  //loop on active channels
		  for (; it != is->ch.end(); ++it)
		    {
		      m_data->MRDout.Type.push_back(in->first);
		      m_data->MRDout.Value.push_back(it->second);
		      m_data->MRDout.Channel.push_back(it->first);
		      m_data->MRDout.Slot.push_back(in->second.Slot.at(i));
		      m_data->MRDout.Crate.push_back(in->second.Crate.at(i));
		    }
		}
	    }
	  //	std::cout<<"E6"<<std::endl;
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
	
      //		std::cout<<"E7"<<std::endl;
	}
      //		OutN = (TOutN >= AOutN) ? TOutN : AOutN;
      //		OutN = TOutN + AOutN;
      //std::cout<<"ben before fill: tree="<<tree<<std::endl;
      /*      if (OutN != 0){
	CCData->`
	tree->Branch("Trigger", &Trigger, "Trigger/i");
	tree->Branch("OutNumber", &OutN, "OutN/i");
	tree->Branch("Type", &Type);	//T for TDC, A for ADC
	tree->Branch("Value", &Value);
	tree->Branch("Slot", &Slot);
	tree->Branch("Channel", &Channel);
	tree->Branch("TimeStamp", &TimeStamp, "TimeStamp/l" );
      */

      CCData->Set("Data",m_data->MRDout);
      CCData->Save(OutName);
      CCData->Delete();
	// tree->Fill();		//Fill
      //std::cout<<"ben after fi;;"<<std::endl;


    
      Trigger++;
      //std::cout<<"E8"<<std::endl;
      m_data->MRDdata.List.Data.clear();		


      //      if(!(Trigger % MonitorFrequency)) m_data->MRDout->Send(m_data->MonitorSocket);

    }
/* //std::cout<<"looking for message ben"<<std::endl;
  zmq::message_t comm;
  if(sPort->recv(&comm,ZMQ_NOBLOCK))
    {
      //std::cout<<"E9"<<std::endl;
      std::stringstream tmptree;
      tmptree << "TTree " << tree;
      //	std::cout << "Root : " << std::hex << tree << std::endl;
      zmq::message_t message(tmptree.str().length()+1);
      snprintf ((char *) message.data(), tmptree.str().length()+1, "%s", tmptree.str().c_str()) ;
      //std::cout<<"MRD: sending reply"<<std::endl;
      sPort->send(message);
      //std::cout<<"MRD: sent"<<std::endl;
      
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
  //std::cout<<"E10"<<std::endl;
  
  //std::cout<<"E11"<<std::endl;
  */

  return true;
}

bool MRDStoreSave::Finalise()
{
  /* //  std::cout<<"f1: tree="<<tree<<std::endl;
  tree->Delete();
  //`std::cout<<"f2"<<std::endl;
  tree = 0;
  //std::cout<<"f3"<<std::endl;
  delete sPort;
  //std::cout<<"f4"<<std::endl;
  sPort = 0;
  */
  /*  
  std::cout<<"f5"<<std::endl;
  for (int i = 0; i < m_data->MRDdata.List.CC["TDC"].size(); i++)
    delete m_data->MRDdata.List.CC["TDC"].at(i);
  std::cout<<"f6"<<std::endl;
  for (int i = 0; i < m_data->MRDdata.List.CC["ADC"].size(); i++)
    delete m_data->MRDdata.List.CC["ADC"].at(i);
  std::cout<<"f7"<<std::endl;
  
  m_data->MRDdata.List.CC.clear();
  std::cout<<"f8"<<std::endl;
  m_data->MRDdata.List.Data.clear();
  std::cout<<"f9"<<std::endl;
  
 delete Epoch;
  std::cout<<"f10"<<std::endl;
  Epoch = 0;
  std::cout<<"f11"<<std::endl;
  */
  CCData->Close();
  std::cout<<"f12"<<std::endl;
  //  delete CCData;
  std::cout<<"f13"<<std::endl;
  // CCData =0;
  std::cout<<"f14"<<std::endl;
  
  return true;
}
