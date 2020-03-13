#include "TriggerStream.h"

TriggerStream::TriggerStream():Tool(){}


bool TriggerStream::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();
  
  m_data= &data;
  
  m_variables.Get("verbose",m_verbose);  
  m_variables.Get("VME_service_name",VME_service_name);
  m_variables.Get("numVME",numVME);
  m_variables.Get("VME_port",VME_port);  
  
  m_data->numVME=numVME;
  m_data->triggered=false;
  
    
  std::vector<Store*> RemoteServices;
  
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
      service->JsonParser(ss.str());
      
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
    //    int a=12000;
    //RemoteSend->setsockopt(ZMQ_SNDTIMEO, a);
    //RemoteSend->setsockopt(ZMQ_RCVTIMEO, a);   
 
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


  ///////// Sending trigger settings ////////////

  //  std::cout<<"VMESockets.size() "<<VMESockets.size()<<std::endl;
  for (int i=0;i<VMESockets.size();i++){
    //std::cout<<"debug 1 "<<i<<std::endl;
    std::string query="Initialise";
    zmq::message_t message(query.length()+1);
    snprintf ((char *) message.data(), query.length()+1 , "%s" ,query.c_str() ) ;
    // std::cout<<"debug 2"<<std::endl;
    if( VMESockets.at(i)->send(message)){
      //std::cout<<"debug 3"<<std::endl;
      std::string setup;
      //m_variables.Print();
      m_variables>>setup;
      //std::cout<<"setup "<<setup<<std::endl;
      zmq::message_t message2(setup.length()+1);
      snprintf ((char *) message2.data(), setup.length()+1 , "%s" ,setup.c_str() ) ;
      //std::cout<<"debug 4 "<<setup<<std::endl;
      if(VMESockets.at(i)->send(message2)){
	// std::cout<<"debug 5"<<std::endl;
	
	zmq::message_t re;
	if(VMESockets.at(i)->recv(&re)){
	  std::istringstream iss2(static_cast<char*>(re.data()));
	  	//std::cout<<"debug 6 "<<iss2.str()<<std::endl;
	}
	else{
	  Log("Error receiving trigger query from VME",0,m_verbose);
        return false;
	
	}
	//std::cout<<"debug 7"<<std::endl;
      }
      else{

	Log("Error sending trigger setup to VME",0,m_verbose);
      }
    } 
    else{
      
      Log("Error sending trigger query to VME",0,m_verbose);
      
      return false;
      
    }
    // std::cout<<"debug 8"<<std::endl;
  }
  // std::cout<<"debug 9"<<std::endl;
  
  
  //  m_data->InfoTitle="TriggerVariables";
  // m_variables>>m_data->InfoMessage;
  // m_data->GetTTree("RunInformation")->Fill();
  
  zmq::socket_t config(*(m_data->context), ZMQ_PUSH);
  config.connect("inproc://Config");

  zmq::message_t confheader(14);
  snprintf ((char *) confheader.data(), 14 , "%s" ,"TriggerConfig");
  config.send(confheader);


  std::string confdata;
  m_variables>>confdata;
  zmq::message_t confmsg(confdata.length()+1);
  snprintf ((char *) confmsg.data(), confdata.length()+1 , "%s" ,confdata.c_str());
  config.send(confmsg);

  
  return true;
}


bool TriggerStream::Execute(){
  //std::cout<<"in TriggerStream"<<std::endl;
  if(m_data->Restart==1)Finalise();
  else if(m_data->Restart==2)Initialise("",*m_data);
    else {
      /*  std::cout<<"d1"<<std::endl;
    //boost::progress_timer t;
    m_data->triggered=false;

    bool trigger=true;  
    if(VMESockets.size()==0)trigger=false;
    std::cout<<"d2 "<<  VMESockets.size()<<std::endl;
    
    for (int i=0;i<VMESockets.size();i++){
      std::cout<<"d3"<<std::endl;
      std::string query="Status";
      zmq::message_t message(query.length()+1);
      snprintf ((char *) message.data(), query.length()+1 , "%s" ,query.c_str() ) ;
      std::cout<<"d4"<<std::endl;
      if( VMESockets.at(i)->send(message)){
	std::cout<<"d5"<<std::endl;
	zmq::message_t receive;
	if(VMESockets.at(i)->recv(&receive)){
	  std::cout<<"d6"<<std::endl;
	  std::istringstream iss(static_cast<char*>(receive.data()));
	  std::cout<<" got trigger message :"<<i<<" = "<<iss.str()<<std::endl;	
	  bool tmptrigger;
	  iss>>tmptrigger;
	  trigger*=tmptrigger;
	  std::cout<<"d7 "<<tmptrigger<<std::endl;
	}
	else{
	  std::cout<<"d8"<<std::endl;
	  Log("Error receiving trigger query from VME",0,m_verbose);	
	  return false;
	  
	}
      }
      
      else{
	std::cout<<"d9"<<std::endl;
	Log("Error sending trigger query to VME",0,m_verbose);
	
	return false;
	
      }   
    }
    std::cout<<"d10"<<std::endl;
    m_data->triggered=trigger;
    if( m_data->triggered) m_data->triggernum++;
    std::cout<< "trigger status= "<<m_data->triggered<<std::endl;
    std::cout<<"d11"<<std::endl;  */
    }
  //std::cout<<"end Trigger"<<std::endl;
  
  return true;
  
}


bool TriggerStream::Finalise(){

  for (int i=0;i<VMESockets.size();i++){
    //std::cout<<"d3"<<std::endl;
    std::string query="Finalise";
    zmq::message_t message(query.length()+1);
    snprintf ((char *) message.data(), query.length()+1 , "%s" ,query.c_str() ) ;
    //std::cout<<"d4"<<std::endl;
    if( VMESockets.at(i)->send(message)){
      //std::cout<<"d5"<<std::endl;
      zmq::message_t receive;
      if(VMESockets.at(i)->recv(&receive)){
	//std::cout<<"d6"<<std::endl;
	std::istringstream iss(static_cast<char*>(receive.data()));
	//std::cout<<" got trigger message :"<<i<<" = "<<iss.str()<<std::endl;	
      }
    }
  }
  
  for (int i=0;i<VMESockets.size();i++){
    
    delete VMESockets.at(i);
    
    
  }
  
  VMESockets.clear();
  
  
  return true;
}
