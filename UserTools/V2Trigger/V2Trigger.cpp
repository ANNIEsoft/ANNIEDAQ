#include "V2Trigger.h"

V2Trigger::V2Trigger():Tool(){}


bool V2Trigger::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;


    m_variables.Get("verbose",m_verbose);  
  //m_variables.Get("VME_service_name",VME_service_name);
  m_variables.Get("numTriggers",numTriggers);
  //m_variables.Get("VME_port",VME_port);  
  //numTriggers=1;  
  m_data->triggered=false;
  
    
  FindTriggerSources();  
  
  //  m_data->InfoTitle="TriggerVariables";
  // m_variables>>m_data->InfoMessage;
  //m_data->GetTTree("RunInformation")->Fill();


  return true;
}


bool V2Trigger::Execute(){

  if(m_data->Restart==1)Finalise();
  else if(m_data->Restart==2)Initialise("",*m_data);
  else {
    FindTriggerSources();
    sleep(1);
    //    std::cout<<"d1"<<std::endl;
  //boost::progress_timer t;
    std::map<std::string,long> triggers;

  bool trigger=false;  
  //std::cout<<"d2"<<std::endl;
  for (std::map<std::string,zmq::socket_t*>::iterator it=Sockets.begin(); it!=Sockets.end(); ++it){
    
    std::cout<<"d3"<<std::endl;
    std::string query="Status";
    zmq::message_t message(query.length()+1);
    snprintf ((char *) message.data(), query.length()+1 , "%s" ,query.c_str() ) ;
    std::cout<<"d4"<<std::endl;
    if( it->second->send(message)){
       std::cout<<"d5"<<std::endl;
      zmq::message_t receive;
      if(it->second->recv(&receive)){
	std::cout<<"d6"<<std::endl;
	std::istringstream iss(static_cast<char*>(receive.data()));
	//std::cout<<" got trigger message "<<iss.str()<<std::endl;	
	std::string type="";
	long trigger=0;;
	iss>>type>>trigger;
	triggers[type]=trigger;
	//trigger*=tmptrigger;
	std::cout<<"d7"<<std::endl;
      }
      else{
	std::cout<<"d8"<<std::endl;
	Log("Error receiving trigger query from Socket",0,m_verbose);	
	return false;
	
      }
    }
    
    else{
      std::cout<<"d9"<<std::endl;
      Log("Error sending trigger query to Socket",0,m_verbose);
      
      return false;
   
    }   
  }
  // std::cout<<"d10"<<std::endl;


  for (std::map<std::string,long>::iterator it=triggers.begin(); it!=triggers.end(); ++it){
    std::cout<<it->first<<" = "<<it->second<<std::endl;
    if(m_variables.Has(it->first)){
      long tmp=0;
      m_variables.Get(it->first,tmp);

      if((it->second % tmp)< TriggerResiduals[it->first]){
	trigger=true;
      }
      TriggerResiduals[it->first]=it->second % tmp;
    }

  }


  m_data->triggered=trigger;

  // std::cout<< "trigger status= "<<m_data->triggered<<std::endl;
  //std::cout<<"d11"<<std::endl;  
  }


  return true;
}


bool V2Trigger::Finalise(){


 
 for (std::map<std::string,zmq::socket_t*>::iterator it=Sockets.begin(); it!=Sockets.end(); ++it){
 
    delete it->second;
    it->second=0;

  }

  Sockets.clear();
  TriggerResiduals.clear();

  return true;
}


void V2Trigger::FindTriggerSources(){


  
  zmq::socket_t Ireceive (*(m_data->context), ZMQ_DEALER);
  Ireceive.connect("inproc://ServiceDiscovery");
  
  for(int i=0;i<11;i++){
  
  zmq::message_t send(8);
  snprintf ((char *) send.data(), 8 , "%s" ,"All NULL") ;
  
  Ireceive.send(send);
  
  zmq::message_t receive;
  Ireceive.recv(&receive);
  std::istringstream iss(static_cast<char*>(receive.data()));
  
  int size;
  iss>>size;
  
    
  for(int i=0;i<size;i++){
    
    Store *service = new Store;
    
    zmq::message_t servicem;
    Ireceive.recv(&servicem);
    
    std::istringstream ss(static_cast<char*>(servicem.data()));
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
    if(servicetype=="TriggerSend" && Sockets.count(uuid)==0){
      zmq::socket_t *RemoteSend = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
      int a=12000;
      RemoteSend->setsockopt(ZMQ_SNDTIMEO, a);
      RemoteSend->setsockopt(ZMQ_RCVTIMEO, a);   
      
      std::stringstream tmp;
      tmp<<"tcp://"<<ip<<":"<<port;
      
      // printf("%s \n",tmp.str().c_str());
      RemoteSend->connect(tmp.str().c_str());
      Sockets[uuid]=RemoteSend;
    }

    else delete service;
    
  }
  
  
  }
 

}
