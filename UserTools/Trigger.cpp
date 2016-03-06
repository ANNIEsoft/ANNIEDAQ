#include "Trigger.h"

Trigger::Trigger():Tool(){}


bool Trigger::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();
  
  m_data= &data;

  m_variables.Get("verbose",m_verbose);  
  m_variables.Get("VME_service_name",VME_service_name);
  m_variables.Get("numVME",numVME);
  m_variables.Get("VME_port",VME_port);  
  
  m_data->triggered=false;
  
    
  std::vector<Store*> RemoteServices;
  
  zmq::socket_t Ireceive (*(m_data->context), ZMQ_DEALER);
  Ireceive.connect("inproc://ServiceDiscovery");
  
  for(int i=0;i<11;i++){
  
  zmq::message_t send(256);
  snprintf ((char *) send.data(), 256 , "%s" ,"All NULL") ;
  
  Ireceive.send(send);
  
  zmq::message_t receive;
  Ireceive.recv(&receive);
  std::istringstream iss(static_cast<char*>(receive.data()));
  
  int size;
  iss>>size;
  
  
  for(int i=0;i<RemoteServices.size();i++){

    delete RemoteServices.at(i);
    RemoteServices.at(i)=0;
  }
  
  RemoteServices.clear();
  
  for(int i=0;i<size;i++){
    
    Store *service = new Store;
    
    zmq::message_t servicem;
    Ireceive.recv(&servicem);
    
    std::istringstream ss(static_cast<char*>(servicem.data()));
    service->JsonPaser(ss.str());
    
    std::string servicetype;
    service->Get("msg_value",servicetype);
    //printf("%s \n",servicetype.c_str());
    if(servicetype==VME_service_name)  RemoteServices.push_back(service);
    else delete service  ;
    
  }

  if (RemoteServices.size()==numVME)break;
  else  usleep(1500000);
 
  }

  if (RemoteServices.size()!=numVME){
    Log("ERROR!! Cant find all of the VME boards",0,m_verbose);
    return false;
  }

  
  for(int i=0;i<RemoteServices.size();i++){
    
    std::string ip;
    //int logport=24010;
    
    //*(it->second)>> output;
    ip=*((*(RemoteServices.at(i)))["ip"]);
    
    
    zmq::socket_t *RemoteSend = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
    int a=12000;
    RemoteSend->setsockopt(ZMQ_SNDTIMEO, a);
    RemoteSend->setsockopt(ZMQ_RCVTIMEO, a);   
 
    std::stringstream tmp;
    tmp<<"tcp://"<<ip<<":"<<VME_port;
    
    // printf("%s \n",tmp.str().c_str());
    RemoteSend->connect(tmp.str().c_str());
    
    VMESockets.push_back(RemoteSend);

  }


  for(int i=0;i<RemoteServices.size();i++){

    delete RemoteServices.at(i);
    RemoteServices.at(i)=0;
  }

  RemoteServices.clear();
  

  /* backup
  zmq::socket_t *RemoteSend = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);                                                                             
  int a=12000;                                                                
  RemoteSend->setsockopt(ZMQ_SNDTIMEO, a);                                    
  RemoteSend->setsockopt(ZMQ_RCVTIMEO, a);                                    
                                                                                
  std::stringstream tmp;                                                      
  tmp<<"tcp://"<<"192.168.1.21"<<":"<<VME_port;                                           
                                                                                
  // printf("%s \n",tmp.str().c_str());                                       
  RemoteSend->connect(tmp.str().c_str());                                     
                                                                                
  VMESockets.push_back(RemoteSend);
  */

  return true;
}


bool Trigger::Execute(){

  //boost::progress_timer t;

  bool trigger=true;  
  
  for (int i=0;i<VMESockets.size();i++){
    
    std::string query="Status";
    zmq::message_t message(query.length()+1);
    snprintf ((char *) message.data(), query.length()+1 , "%s" ,query.c_str() ) ;

    if( VMESockets.at(i)->send(message)){
      
      zmq::message_t receive;
      if(VMESockets.at(i)->recv(&receive)){
	std::istringstream iss(static_cast<char*>(receive.data()));
	//std::cout<<" got trigger message "<<iss.str()<<std::endl;	
	bool tmptrigger;
	iss>>tmptrigger;
	trigger*=tmptrigger;
	
      }
      else{
	Log("Error receiving trigger query from VME",0,m_verbose);	
	return false;
	
      }
    }
    
    else{
      
      Log("Error sending trigger query to VME",0,m_verbose);
      
      return false;
   
    }   
  }
  
  m_data->triggered=trigger;
  
  return true;
  
}


bool Trigger::Finalise(){

  for (int i=0;i<VMESockets.size();i++){

    delete VMESockets.at(i);


  }

  VMESockets.clear();


  return true;
}
