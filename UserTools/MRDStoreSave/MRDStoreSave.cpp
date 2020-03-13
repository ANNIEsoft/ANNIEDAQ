#include "MRDStoreSave.h"

MRDStoreSave::MRDStoreSave():Tool(){}

bool MRDStoreSave::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
 
	//	m_variables.Get("FileCap", FileCap);
	m_variables.Get("OutPath", OutPath);
	m_variables.Get("OutName", OutName);
	m_variables.Get("StartTime", StartTime);
	//m_variables.Get("MonitorFrequency",MonitorFrequency);

	FileCount = 0;	
	std::stringstream Tmp;
	OutName = OutPath + OutName;
	//std::cout<<"here1"<<std::endl;
	OutNameB= OutName + "B";
	//std::cout<<"here2"<<std::endl;

	//Tmp << OutName << "p" << FileCount << ".Store";
//	TFile file(Tmp.str().c_str(), "RECREATE", "", 1);
//	file.Write();
//	file.Close();
	
	//std::cout<<"MRD: attempting connect"<<std::endl;
	//std::cout<<"here3"<<std::endl;

	sPort = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
	sPort->bind("tcp://*:9999");		//final use

	//std::cout<<"here4"<<std::endl;

	items[0].socket=*sPort;
	items[0].fd=0;
	items[0].events=ZMQ_POLLIN;
	items[0].revents=0;

	//std::cout<<"here5"<<std::endl;

	zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
	Ireceive.connect("inproc://ServicePublish");
	
	//std::cout<<"here6"<<std::endl;

	boost::uuids::uuid m_UUID;
	m_UUID = boost::uuids::random_generator()();
	std::stringstream test;
	test<<"Add "<< "DataSend "<<m_UUID<<" 9999 "<<"0";
	zmq::message_t send(test.str().length());
	snprintf ((char *) send.data(), test.str().length() , "%s" ,test.str().c_str()) ;
	Ireceive.send(send);  

	//std::cout<<"here7"<<std::endl;

	//	sPort->connect("ipc:///tmp/0");			//test only
	//std::cout<<"MRD: connected"<<std::endl;

	//	std::string Line;
	//std::stringstream ssL;
	//std::string sEmp;
	//int iEmp;

	CCData=new BoostStore(false, 2);
	//std::cout<<"here8"<<std::endl;

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

	//std::cout<<"here9"<<std::endl;

	Epoch = new boost::posix_time::ptime(boost::gregorian::from_string(StartTime));
	//std::cout<<"here10"<<std::endl;

	m_data->ma="/data/ma";
	m_data->mb="/data/mb";
	m_data->mn=0;

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
      m_data->MRDout.Trigger= m_data->MRDdata.triggernum;
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
      CCData->Save(m_data->ma);
      CCData->Delete();
	// tree->Fill();		//Fill
      //std::cout<<"ben after fi;;"<<std::endl;


    
      //Trigger++;
      //std::cout<<"E8"<<std::endl;
      m_data->MRDdata.List.Data.clear();		


      //      if(!(Trigger % MonitorFrequency)) m_data->MRDout->Send(m_data->MonitorSocket);

    }

  zmq::poll(&items [0], 1, 0);

  if (items [0].revents & ZMQ_POLLIN) {

    zmq::message_t message;

    sPort->recv(&message);

    zmq::message_t name(7);
    snprintf ((char *) name.data(), 7 , "%s" ,"CCData") ;
    sPort->send(name,ZMQ_SNDMORE);

    std::stringstream tmp;
    tmp<<CCData;

    std::cout<<" SENDING "<<CCData<<std::endl;
    std::cout<<" SENDING "<<tmp.str()<<std::endl;

    zmq::message_t message2(tmp.str().length()+1);
    snprintf ((char *) message2.data(), tmp.str().length()+1 , "%s" ,tmp.str().c_str()) ;
    sPort->send(message2);
    //CCData->Send(sPort);
    //delete CCData;
    CCData=0;
    std::string tt;
    tt=m_data->ma;
    m_data->ma=m_data->mb;
    m_data->mb=tt;
    //m_data->mn++;
    // std::stringstream mm;
    //mm<<m_data->ma<<m_data->mn;
    //m_data->ma=mm.str();

    CCData=new BoostStore(false, 2);
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
   //  std::cout<<"f1: tree="<<tree<<std::endl;
  //tree->Delete();
  //`std::cout<<"f2"<<std::endl;
  //tree = 0;
  //std::cout<<"f3"<<std::endl;
    zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
    Ireceive.connect("inproc://ServicePublish");
    std::stringstream test;
    test<<"Delete "<< "DataSend ";
    zmq::message_t send(test.str().length()+1);
    snprintf ((char *) send.data(), test.str().length()+1 , "%s" ,test.str().c_str()) ;
    Ireceive.send(send);


  delete sPort;
  //std::cout<<"f4"<<std::endl;
  sPort = 0;
 
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
  //std::cout<<"f12"<<std::endl;
  delete CCData;
  //std::cout<<"f13"<<std::endl;
  CCData =0;
  //std::cout<<"f14"<<std::endl;
  
  return true;
}
