#include "CCTrigger.h"

//CCTrigger::CCTrigger():Tool(), Lcard(1, "CamacCrate"), Ncard(1, 25) {}
CCTrigger::CCTrigger():Tool() {}

bool CCTrigger::Initialise(std::string configfile, DataModel &data)
{
  threshold=0;
  reftriggernum=99999;

	if(configfile!="")	m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_variables.Get("verbose", verb);		//Verbosity
	m_variables.Get("configcc", configcc);		//Module slots
	m_variables.Get("percent", perc);		//firing probability
	m_variables.Get("trg_mode", m_data->MRDdata.trg_mode);	//Trigger mode


	TriggerSend=new zmq::socket_t(*(m_data->context),ZMQ_DEALER);
	TriggerSend->bind("tcp://*:6666");


	zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
	Ireceive.connect("inproc://ServicePublish");

	boost::uuids::uuid m_UUID;
	m_UUID = boost::uuids::random_generator()();
	std::stringstream test;
	test<<"Add "<< "TriggerSend "<<m_UUID<<" 6666 "<<"0";
	zmq::message_t send(test.str().length());
	snprintf ((char *) send.data(), test.str().length() , "%s" ,test.str().c_str()) ;
	Ireceive.send(send);


	items[0].socket=*TriggerSend;
	items[0].fd=0;
	items[0].events=ZMQ_POLLIN;
	items[0].revents=0;

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
				ssL >> iEmp;
				Ncrate.push_back(iEmp);         // crate
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
		  //std::cout << "d1 " << Ccard.at(i) << " " << Ncard.at(i) <<" "<< Ncrate.at(i)<<std::endl; 
		  m_data->MRDdata.List.CC[Lcard.at(i)].push_back(Create(Lcard.at(i), Ccard.at(i), Ncard.at(i), Ncrate.at(i)));	//They use CC at 0
			//std::cout << "d2 "<<std::endl;
		}
		else if (Lcard.at(i) == "TRG")
		{
		  //std::cout << "d3 "<<std::endl;
			trg_pos = m_data->MRDdata.List.CC["TDC"].size();
			//std::cout << "d4 "<<std::endl;			
			m_data->MRDdata.List.CC["TDC"].push_back(Create("TDC", Ccard.at(i), Ncard.at(i), Ncrate.at(i)));	//They use CC at 0
			//std::cout << "d5 "<<std::endl;
		}
		else if (Lcard.at(i) == "DISC"){
		  //		  std::string cardname, std::string config, int cardslot, int crate
		  //std::cout<<"setting descriminator crate:"<<Ncrate.at(i)<<", Card:"<<Ncard.at(i)<<std::endl;
		    LeCroy4413* tmp=new LeCroy4413(Ncard.at(i), Ccard.at(i), Ncrate.at(i));
		    disc.push_back(tmp);
		    //		  tmp.
	  
		}
		else std::cout << "\n\nUnkown card\n" << std::endl;
		//std::cout << "for over " << std::endl;
	}

	//std::cout << "Trigger is in slot ";
	//std::cout << m_data->MRDdata.List.CC["TDC"].at(trg_pos)->GetSlot();
	//std::cout << " and crate "<<m_data->MRDdata.List.CC["TDC"].at(trg_pos)->GetCrate() << std::endl;

	srand(time(0));

	m_data->MRDdata.triggernum=0;

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
			if(m_data->MRDdata.List.CC["TDC"].at(trg_pos)->TestEvent() == 1)
				m_data->MRDdata.TRG = true;
			break;

		case 1:
			m_data->MRDdata.TRG = true;
			break;
		
		case 2:
			m_data->MRDdata.TRG = rand() % int(100/perc) == 0;
			break;
		
		default:
			std::cout << "WARNING: Trigger mode unknown\n" << std::endl;
	}
	if(m_data->MRDdata.TRG){
	  m_data->MRDdata.triggernum++;
	  std::stringstream tmp;
	  tmp<<"Trigger No.="<<m_data->MRDdata.triggernum;
	  m_data->vars.Set("Status",tmp.str());
	}
	//	if(m_data->MRDdata.TRG) std::cout<<"yo"<<std::endl;
	
	zmq::poll(&items[0], 1, 0);

	if ((items [0].revents & ZMQ_POLLIN)) {
	  zmq::message_t command;
	  TriggerSend->recv(&command);

	  std::stringstream tmp;
	  tmp<<"MRD "<<m_data->MRDdata.triggernum;
	  zmq::message_t message(tmp.str().length()+1);
	  snprintf ((char *) message.data(), tmp.str().length()+1 , "%s" ,tmp.str().c_str()) ;
	  TriggerSend->send(message);
	  //std::cout<<"MRD sending "<<tmp.str()<<std::endl;
	}

	/*	if((m_data->MRDdata.triggernum % 10000) ==0  && m_data->MRDdata.triggernum != reftriggernum){
	  threshold++;
	  reftriggernum=m_data->MRDdata.triggernum;
	  std::cout<<"triggernum="<<m_data->MRDdata.triggernum<<" , threshold set to "<<threshold<<std::endl;
	  if (threshold>=1024) exit(1);
	  for(int i=0;i<disc.size();i++){

	    disc.at(i)->WriteThresholdValue(threshold);
	  }

	}
	*/
	  return true;
}


bool CCTrigger::Finalise()
{

  zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
  Ireceive.connect("inproc://ServicePublish");
  std::stringstream test;
  test<<"Delete "<< "TriggerSend ";
  zmq::message_t send(test.str().length()+1);
  snprintf ((char *) send.data(), test.str().length()+1 , "%s" ,test.str().c_str()) ;
  Ireceive.send(send);

  delete TriggerSend;
  TriggerSend=0;

	Lcard.clear();
	Ncard.clear();

	return true;
}

inline CamacCrate* CCTrigger::Create(std::string cardname, std::string config, int cardslot, int crate)
{
	CamacCrate* ccp;
	if (cardname == "TDC")
	{
	  std::cout<<"TDC"<<std::endl;
	  ccp = new Lecroy3377(cardslot, config, crate);
	}
	if (cardname == "ADC")
	{
	  std::cout<<"ADC"<<std::endl;
	  ccp = new Lecroy4300b(cardslot, config, crate);
	}
	return ccp;
}
