#include "VMETriggerSend.h"

VMETriggerSend::VMETriggerSend():Tool(){}


bool VMETriggerSend::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_data->triggered=false;

  m_variables.Get("Trigger_port",Trigger_port);
  m_variables.Get("verbose",m_verbose);

  TriggerCom = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
  int a=12000;
  TriggerCom->setsockopt(ZMQ_SNDTIMEO, a);
  TriggerCom->setsockopt(ZMQ_RCVTIMEO, a);

  std::stringstream tmp;
  tmp<<"tcp://*:"<<Trigger_port;  
  TriggerCom->bind(tmp.str().c_str());


  return true;
}


bool VMETriggerSend::Execute(){

  m_data->triggered=false;
  
  zmq::message_t receive;
  if(TriggerCom->recv(&receive,ZMQ_NOBLOCK)){
    std::istringstream iss(static_cast<char*>(receive.data()));
    
    std::string ret="";
    
    if (iss.str()=="Status"){
      
      
      usleep(100000); //GET STATUS OF CARD 
      
      ret="1";
      m_data->triggered=1;
      
  }
    else ret="ERROR unknown message";
    
    zmq::message_t message(ret.length()+1);
    snprintf ((char *) message.data(), ret.length()+1 , "%s" ,ret.c_str() ) ;
    
    if(!( TriggerCom->send(message))){
      
      Log("Error could not return trigger status",0,m_verbose);
      return false;
      
    }
   
  } 
    return true;
  }
  

bool VMETriggerSend::Finalise(){


  delete TriggerCom;
  TriggerCom=0; 


  return true;
}
