#include "StoreSave.h"

StoreSave::StoreSave():Tool(){}


bool StoreSave::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  outstore=new BoostStore(false,0);

    m_variables.Get("verbose",m_verbose);  
  //m_variables.Get("VME_service_name",VME_service_name);
  m_variables.Get("numSources",numSources);
  m_variables.Get("OutPath",OutPath);
  m_variables.Get("OutFile",OutFile);
  //m_variables.Get("VME_port",VME_port);  
  //numTriggers=1;  
  part=0;
  std::stringstream tmp;
  tmp<<OutPath<<OutFile<<"R"<<m_data->RunNumber<<"S"<<m_data->SubRunNumber<<"p"<<part;
  Out=tmp.str();

  m_data->triggered=false;
  

  FindDataSources();
    

  m_data->MonitoringSocket=new zmq::socket_t(*m_data->context, ZMQ_PUB);
  m_data->MonitoringSocket->bind("tcp://*:63982");
  
  zmq::socket_t serviceadd (*m_data->context, ZMQ_PUSH);
  serviceadd.connect("inproc://ServicePublish");
  
  boost::uuids::uuid m_UUID;
  m_UUID = boost::uuids::random_generator()();
  std::stringstream test;
  test<<"Add "<< "MonitorData "<<m_UUID<<" 63982 "<<"0";
  zmq::message_t send(test.str().length());
  snprintf ((char *) send.data(), test.str().length() , "%s" ,test.str().c_str()) ;
  serviceadd.send(send);  
  

  
  //  m_data->InfoTitle="TriggerVariables";
  // m_variables>>m_data->InfoMessage;
  //m_data->GetTTree("RunInformation")->Fill();
  m_data->Stores["RunInformation"]->Set("StoreSave",m_variables);


  return true;
}


bool StoreSave::Execute(){
  std::cout<<"q0"<<std::endl;

  if(m_data->Restart==1){
    std::cout<<"q1"<<std::endl;
    Finalise();
    std::cout<<"q2"<<std::endl;
  }
  else if(m_data->Restart==2){
    std::cout<<"q3"<<std::endl;
    Initialise("",*m_data);
    std::cout<<"q4"<<std::endl;
  }
  else {
    std::cout<<"q5"<<std::endl; 
    
    FindDataSources();
    std::cout<<"q6"<<std::endl;
    if(m_data->triggered){
      std::cout<<"q7"<<std::endl;
      for (std::map<std::string,zmq::socket_t*>::iterator it=DataSources.begin(); it!=DataSources.end(); ++it){
	
	
	
	zmq::message_t out(2);
	it->second->send(out);
	
	zmq::message_t mesname;
	it->second->recv(&mesname);
	std::istringstream iss(static_cast<char*>(mesname.data()));
	std::string name=iss.str();
	std::cout<<"h0 "<<name<<std::endl;
	
	zmq::message_t in;
	it->second->recv(&in);
	std::istringstream iss2(static_cast<char*>(in.data()));
	unsigned long arg2;
	iss2 >>  std::hex >> arg2;
	BoostStore* tmp;
	tmp=reinterpret_cast<BoostStore*>(arg2);
	//  std::string r="r";
	//  tmp->Set("test",r);
	//tmp->Save("ggg");
	std::cout<<"h1 "<<name<<":"<<tmp<<std::endl;
	//tmp->Print(false);
	//if(name=="TrigData"){
	//TriggerData tt;
	//tmp->Get("TrigData",tt);
	//std::cout<<"SS Eventsize="<<tt.EventSize<<std::endl;
	//std::cout<<"SS SequenceID="<<tt.SequenceID<<std::endl;
	//std::cout<<"SS EventIDs size="<<tt.EventIDs.size()<<std::endl;
	//std::cout<<"SS EventIDs 3="<<tt.EventIDs.at(3)<<std::endl;
	//}
	outstore->Set(name,tmp);
	std::cout<<"h2"<<std::endl;
	//delete tmp;
	std::cout<<"h3"<<std::endl;
      }
      
      std::cout<<"h4"<<std::endl;
      BoostStore* RI=new BoostStore(false,0);
      std::string RItmp="";
      (*(m_data->Stores["RunInformation"]))>>RItmp;
      RI->JsonParser(RItmp);
      outstore->Set("RunInformation",RI);
      outstore->Save(Out);
      //BoostStore * RItmp=0;
      // outstore->Set("RunInformation",RItmp,false);
      outstore->Close();
      std::cout<<"h5"<<std::endl;
      
      zmq::message_t message(9);                  
      snprintf ((char *) message.data(), 9 , "%s" ,"DataFile") ;
      m_data->MonitoringSocket->send(message, ZMQ_SNDMORE);
      std::cout<<"h6"<<std::endl;
      
      zmq::message_t message2(Out.length()+1);
      snprintf ((char *) message2.data(), Out.length()+1 , "%s" ,Out.c_str()) ; 
      m_data->MonitoringSocket->send(message2);
      std::cout<<"h7"<<std::endl;
      
      std::stringstream tmp;
      part++;
      tmp<<OutPath<<OutFile<<"R"<<m_data->RunNumber<<"S"<<m_data->SubRunNumber<<"p"<<part;
      Out= tmp.str();
      delete outstore;
      outstore=0;
      outstore= new BoostStore(false,0); 
      std::cout<<"h8"<<std::endl;
    }
  }
  
  return true;
}


bool StoreSave::Finalise(){


    for (std::map<std::string,zmq::socket_t*>::iterator it=DataSources.begin(); it!=DataSources.end(); ++it){


      zmq::message_t out(2);
      it->second->send(out);
      
      zmq::message_t mesname;
      it->second->recv(&mesname);
      std::istringstream iss(static_cast<char*>(mesname.data()));
      std::string name=iss.str();

      zmq::message_t in;
      it->second->recv(&in);
      std::istringstream iss2(static_cast<char*>(in.data()));
      unsigned long arg2;
      iss2 >>  std::hex >> arg2;
      BoostStore* tmp;
      tmp=reinterpret_cast<BoostStore *>(arg2);
      std::cout<<"h1 "<<name<<":"<<tmp<<std::endl;
      std::cout<<"r-1"<<std::endl; 
      outstore->Set(name,tmp);
      std::cout<<"r-2"<<std::endl;
      delete tmp;
      std::cout<<"r-3"<<std::endl;
      delete it->second;
      std::cout<<"r-4"<<std::endl;
      it->second=0;
    }

    m_data->Stores["RunInformation"]->Set("InputVariables",m_variables);
    outstore->Save(Out);
    std::cout<<"r1"<<std::endl;
    outstore->Close();
    std::cout<<"r1.5"<<std::endl;  
    //delete outstore;
    outstore=0;
    std::cout<<"r2"<<std::endl;     
//    outstore= new BoostStore(false,0); 
    std::cout<<"r3"<<std::endl; 
    DataSources.clear();
    std::cout<<"r4"<<std::endl;
 
    zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
    Ireceive.connect("inproc://ServicePublish");
    std::stringstream test;
    test<<"Delete "<< "MonitorData ";
    zmq::message_t send(test.str().length()+1);
    snprintf ((char *) send.data(), test.str().length()+1 , "%s" ,test.str().c_str()) ;
    Ireceive.send(send);
    std::cout<<"r5"<<std::endl; 

    delete m_data->MonitoringSocket;
    m_data->MonitoringSocket==0;

    std::cout<<"r6"<<std::endl; 
  return true;
}


void StoreSave::FindDataSources(){

  
  zmq::socket_t Ireceive (*(m_data->context), ZMQ_DEALER);
  Ireceive.connect("inproc://ServiceDiscovery");
  
  //  for(int i=0;i<11;i++){
  zmq::pollitem_t items[1]={{Ireceive,0,ZMQ_POLLOUT,0},}; 
  
  zmq::message_t send(8);
  snprintf ((char *) send.data(), 8 , "%s" ,"All NULL") ;

  zmq::poll(&items[0], 1, 0); 

  if ((items [0].revents & ZMQ_POLLOUT)) {
  
    Ireceive.send(send);
  
    zmq::message_t receive;
    Ireceive.recv(&receive);
    //std::cout<<"storesave message size="<<sizeof(receive.data())<<std::endl;


    std::istringstream iss(static_cast<char*>(receive.data()));
    //std::cout<<"storesave iss="<<iss.str()<<std::endl;
    
    int size;
    iss>>size;
    //std::cout<<"storesave size="<<size<<std::endl;
    
    for(int i=0;i<size;i++){
      
      //std::cout<<"storesave loop="<<i<<std::endl;
      
      Store *service = new Store;
      
      zmq::message_t servicem;
      Ireceive.recv(&servicem);
      
      std::istringstream ss(static_cast<char*>(servicem.data()));
      //std::cout<<"storesave message="<<i<<" : "<<ss.str()<<std::endl;
      
      service->JsonParser(ss.str());
      
      std::string servicetype;
      std::string uuid;
      std::string ip;
      int port=0;
      
      service->Get("msg_value",servicetype);
      service->Get("uuid",uuid);
      service->Get("ip",ip);
      service->Get("remote_port",port); 
      //printf("%s \n",servicetype.c_str());
      if(servicetype=="DataSend" && DataSources.count(uuid)==0){
	zmq::socket_t *RemoteSend = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
	int a=12000;
	RemoteSend->setsockopt(ZMQ_SNDTIMEO, a);
	RemoteSend->setsockopt(ZMQ_RCVTIMEO, a);   
	
	std::stringstream tmp;
	tmp<<"tcp://"<<ip<<":"<<port;
	
	// printf("%s \n",tmp.str().c_str());
	RemoteSend->connect(tmp.str().c_str());
	DataSources[uuid]=RemoteSend;
      }
      
      else delete service;
      
    }
    
    
  }
}
