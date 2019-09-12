#include "VMEStoreSave.h"

VMEStoreSave::VMEStoreSave():Tool(){}

bool VMEStoreSave::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
 

	sPort = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
	sPort->bind("tcp://*:9998");		//final use

	strigPort = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
	strigPort->bind("tcp://*:9997");
      
	//std::cout<<"here4"<<std::endl;

	items[0].socket=*sPort;
	items[0].fd=0;
	items[0].events=ZMQ_POLLIN;
	items[0].revents=0;
	items[1].socket=*strigPort;
	items[1].fd=0;
	items[1].events=ZMQ_POLLIN;
	items[1].revents=0;

	//std::cout<<"here5"<<std::endl;

	zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
	Ireceive.connect("inproc://ServicePublish");
	
	//std::cout<<"here6"<<std::endl;

	boost::uuids::uuid m_UUID;
	m_UUID = boost::uuids::random_generator()();
	std::stringstream test;
	std::stringstream test2;
	test<<"Add "<< "DataSend "<<m_UUID<<" 9998 "<<"0";
	m_UUID = boost::uuids::random_generator()();
	test2<<"Add "<< "DataSend "<<m_UUID<<" 9997 "<<"0";
	zmq::message_t send(test.str().length());
	zmq::message_t send2(test2.str().length());

	snprintf ((char *) send.data(), test.str().length() , "%s" ,test.str().c_str()) ;
	snprintf ((char *) send2.data(), test2.str().length() , "%s" ,test2.str().c_str()) ;

	Ireceive.send(send);  
	Ireceive.send(send2);

	//std::cout<<"here7"<<std::endl;

	//	sPort->connect("ipc:///tmp/0");			//test only
	//std::cout<<"MRD: connected"<<std::endl;

	//	std::string Line;
	//std::stringstream ssL;
	//std::string sEmp;
	//int iEmp;

	PMTData=new BoostStore(false, 2); ////ben need to update
	TrigData=new BoostStore(false, 2);
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

	//	Epoch = new boost::posix_time::ptime(boost::gregorian::from_string(StartTime));
	//std::cout<<"here10"<<std::endl;

	m_data->carddata= new BoostStore(false,2);
	m_data->trigdata= new BoostStore(false,2);

	return true;
}

bool VMEStoreSave::Execute()
{
  //  std::cout<<"E1"<<std::endl;

  zmq::poll(&items[0], 2, 0);

  if (items [0].revents & ZMQ_POLLIN) {

    zmq::message_t message;

    sPort->recv(&message);

    zmq::message_t name(8);
    snprintf ((char *) name.data(), 8 , "%s" ,"PMTData") ;
    sPort->send(name,ZMQ_SNDMORE);

    std::stringstream tmp;
    tmp<<m_data->carddata;

    //std::cout<<" SENDING "<<CCData<<std::endl;
    //std::cout<<" SENDING "<<tmp.str()<<std::endl;

    zmq::message_t message2(tmp.str().length()+1);
    snprintf ((char *) message2.data(), tmp.str().length()+1 , "%s" ,tmp.str().c_str()) ;
    sPort->send(message2);
    //CCData->Send(sPort);
    //delete CCData;
    m_data->carddata=0;
    std::string tt;
    tt=OutName;
    OutName=OutNameB;
    OutNameB=tt;
    m_data->carddata=new BoostStore(false, 2);
  }

  if (items [1].revents & ZMQ_POLLIN) {

    zmq::message_t message;

    strigPort->recv(&message);

    zmq::message_t name(9);
    snprintf ((char *) name.data(), 9 , "%s" ,"TrigData") ;
    strigPort->send(name,ZMQ_SNDMORE);
    
    // TriggerData tmp2;

    //m_data->trigdata->Get("TrigData",tmp2);
    //std::cout<<"VSS Eventsize="<<tmp2.EventSize<<std::endl;
    //std::cout<<"VSS SequenceID="<<tmp2.SequenceID<<std::endl;
    //std::cout<<"VSS EventIDs size="<<tmp2.EventIDs.size()<<std::endl;
    //std::cout<<"VSS EventIDs 3="<<tmp2.EventIDs.at(3)<<std::endl;


    std::stringstream tmp;
    tmp<<m_data->trigdata;

    //std::cout<<" SENDING "<<CCData<<std::endl;
    std::cout<<" SENDING trig"<<tmp.str()<<std::endl;

    zmq::message_t message2(tmp.str().length()+1);
    snprintf ((char *) message2.data(), tmp.str().length()+1 , "%s" ,tmp.str().c_str()) ;
    strigPort->send(message2);
    //CCData->Send(sPort);
    //delete CCData;
    m_data->trigdata=0;
    std::string tt;
    tt=OutName;
    OutName=OutNameB;
    OutNameB=tt;
    m_data->trigdata=new BoostStore(false, 2);
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

bool VMEStoreSave::Finalise()
{
   //  std::cout<<"f1: tree="<<tree<<std::endl;
  //tree->Delete();
  //`std::cout<<"f2"<<std::endl;
  //tree = 0;
  //std::cout<<"f3"<<std::endl;
    zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
    Ireceive.connect("inproc://ServicePublish");
    std::stringstream test;
    std::stringstream test2;
    test<<"Delete "<< "DataSend ";
    test2<<"Delete "<< "DataSend ";
    zmq::message_t send(test.str().length()+1);
    zmq::message_t send2(test2.str().length()+1);
    snprintf ((char *) send.data(), test.str().length()+1 , "%s" ,test.str().c_str()) ;
    snprintf ((char *) send2.data(), test2.str().length()+1 , "%s" ,test2.str().c_str()) ;
    Ireceive.send(send);
    Ireceive.send(send2);

  delete sPort;
  delete strigPort;
  //std::cout<<"f4"<<std::endl;
  sPort = 0;
  strigPort=0;
 
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
  
 //delete Epoch;
  std::cout<<"f10"<<std::endl;
 // Epoch = 0;
  std::cout<<"f11"<<std::endl;
  */
  PMTData->Close();
  TrigData->Close();

  //std::cout<<"f12"<<std::endl;
  delete PMTData;
  delete TrigData;

  //std::cout<<"f13"<<std::endl;
  PMTData =0;
  TrigData =0;
  //std::cout<<"f14"<<std::endl;
  
  return true;
}
