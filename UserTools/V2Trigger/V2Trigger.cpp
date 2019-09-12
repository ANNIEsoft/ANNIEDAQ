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

  m_data->Stores["RunInformation"]->Set("V2Trigger",m_variables);

  return true;
}


bool V2Trigger::Execute(){
  // printf("d0");
  //std::cout<<"d0"<<std::endl;

  if(m_data->Restart==1)Finalise();
  else if(m_data->Restart==2)Initialise("",*m_data);
  else {
    //printf("d0.1");
    // std::cout<<"d0.1"<<std::endl;
    FindTriggerSources();
    //printf("d0.2");
    //std::cout<<"d0.2"<<std::endl;
    usleep(100);
    // printf("d1");
    //std::cout<<"d1"<<std::endl;

  //boost::progress_timer t;
    std::map<std::string,long> triggers;

  bool trigger=false;
  //printf("d2");  
  // std::cout<<"d2"<<std::endl;
  std::stringstream statusmsg;
  for (std::map<std::string,zmq::socket_t*>::iterator it=Sockets.begin(); it!=Sockets.end(); ++it){
    
    //printf("d3");
    //std::cout<<"d3"<<std::endl;
    std::string query="Status";
    zmq::message_t message(query.length()+1);
    snprintf ((char *) message.data(), query.length()+1 , "%s" ,query.c_str() ) ;

    //printf("d4");
    //std::cout<<"d4"<<std::endl;
    if( it->second->send(message)){
      //printf("d5");
      //std::cout<<"d5"<<std::endl;
      zmq::message_t receive;
      if(it->second->recv(&receive)){
	//printf("d6");
	//std::cout<<"d6"<<std::endl;
	std::istringstream iss(static_cast<char*>(receive.data()));
	std::cout<<" got trigger message "<<iss.str()<<std::endl;	
	std::string type="";
	long trigger=0;
	//std::cout<<"V2Trigger received "<<iss.str()<<std::endl;
	iss>>type>>trigger;
	triggers[type]=trigger;
	statusmsg<<type<<":"<<trigger<<" , ";
	//trigger*=tmptrigger;
	//printf("d7");
	//std::cout<<"d7 trigger="<<trigger<<std::endl;
      }
      else{
	//printf("d8");
	//std::cout<<"d8"<<std::endl;
	Log("Error receiving trigger query from Socket",0,m_verbose);	
	return false;
	
      }
    }
    
    else{
      //printf("d9");
      //std::cout<<"d9"<<std::endl;
      Log("Error sending trigger query to Socket",0,m_verbose);
      
      return false;
   
    }   
  }
  m_data->vars.Set("Status",statusmsg.str());
  //printf("d10"); 
  ///std::cout<<"d10"<<std::endl;


  for (std::map<std::string,long>::iterator it=triggers.begin(); it!=triggers.end(); ++it){
    //std::cout<<it->first<<" = "<<it->second<<std::endl;
    if(m_variables.Has(it->first)){
      long tmp=0;
      m_variables.Get(it->first,tmp);
      std::cout<<tmp<<" : "<<TriggerResiduals[it->first]<<std::endl;
      if((it->second % tmp)< TriggerResiduals[it->first]){
	trigger=true;
      }
      TriggerResiduals[it->first]=it->second % tmp;
    }

  }


  m_data->triggered=trigger;

  // std::cout<< "trigger status= "<<m_data->triggered<<std::endl;
  //printf("d11");
  //std::cout<<"d11"<<std::endl;  
  }


  return true;
}


bool V2Trigger::Finalise(){

  std::string tmp("");
  m_data->vars.Set("Status",tmp);
 
 for (std::map<std::string,zmq::socket_t*>::iterator it=Sockets.begin(); it!=Sockets.end(); ++it){
 
    delete it->second;
    it->second=0;

  }

  Sockets.clear();
  TriggerResiduals.clear();

  return true;
}


void V2Trigger::FindTriggerSources(){

  //std::cout<<"s1"<<std::endl;

  
  zmq::socket_t Ireceive (*(m_data->context), ZMQ_DEALER);
  Ireceive.connect("inproc://ServiceDiscovery");
  //std::cout<<"s2"<<std::endl;

  zmq::pollitem_t items[1]={{Ireceive,0,ZMQ_POLLOUT,0},};
  //std::cout<<"s3"<<std::endl;

  //for(int i=0;i<11;i++){

  zmq::poll(&items[0], 1, 0);
  //  std::cout<<"s4"<<std::endl;

  if ((items [0].revents & ZMQ_POLLOUT)) {
    //std::cout<<"s5"<<std::endl;

  zmq::message_t send(8);
  snprintf ((char *) send.data(), 8 , "%s" ,"All NULL") ;
  //std::cout<<"s6"<<std::endl;

  Ireceive.send(send);
  //std::cout<<"s7"<<std::endl;

  zmq::message_t receive;
  Ireceive.recv(&receive);
  //std::cout<<"trigger  message size="<<sizeof(receive.data())<<std::endl;
  std::istringstream iss(static_cast<char*>(receive.data()));
  //std::cout<<"s8 iss="<<iss.str()<<std::endl;

  int size;
  iss>>size;
  //std::cout<<"s8 size="<<size<<std::endl;

    
  for(int i=0;i<size;i++){
    //std::cout<<"w1 "<<i<<std::endl;

    Store *service = new Store;
    //std::cout<<"w2"<<std::endl;

    zmq::message_t servicem;
    Ireceive.recv(&servicem);
    //std::cout<<"w3"<<std::endl;

    std::istringstream ss(static_cast<char*>(servicem.data()));
    //std::cout<<"trigger message="<<i<<" : "<<ss.str()<<std::endl;   
 service->JsonParser(ss.str());
 //std::cout<<"w4"<<std::endl;

    std::string servicetype;
    std::string uuid;
    std::string ip;
    int port=0;
    //std::cout<<"w5"<<std::endl;

    service->Get("msg_value",servicetype);
    service->Get("uuid",uuid);
    service->Get("ip",ip);
    service->Get("remote_port",port); 
   //printf("%s \n",servicetype.c_str());
    //std::cout<<"w6"<<std::endl;

    if(servicetype=="TriggerSend" && Sockets.count(uuid)==0){
      //std::cout<<"w7"<<std::endl;
      zmq::socket_t *RemoteSend = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
      int a=12000;
      RemoteSend->setsockopt(ZMQ_SNDTIMEO, a);
      RemoteSend->setsockopt(ZMQ_RCVTIMEO, a);   
      //std::cout<<"w8"<<std::endl;

      std::stringstream tmp;
      tmp<<"tcp://"<<ip<<":"<<port;
      //std::cout<<"w9"<<std::endl;

      // printf("%s \n",tmp.str().c_str());
      RemoteSend->connect(tmp.str().c_str());
      //std::cout<<"w10"<<std::endl;

      Sockets[uuid]=RemoteSend;
      //std::cout<<"w11"<<std::endl;
  
  }


    else delete service;
    //std::cout<<"s9"<<std::endl;

  }
  //std::cout<<"s10"<<std::endl;

  
  }
 
  //std::cout<<"s11"<<std::endl;

}
