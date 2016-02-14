#include "DummyVMETriggerSend.h"

DummyVMETriggerSend::DummyVMETriggerSend():Tool(){}


bool DummyVMETriggerSend::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_data->triggered=false;

  m_variables.Get("Trigger_port",Trigger_port);
  m_variables.Get("verbose",m_verbose);
  m_variables.Get("Crate_Num",m_crate_num);

  TriggerCom = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
  int a=12000;
  TriggerCom->setsockopt(ZMQ_SNDTIMEO, a);
  TriggerCom->setsockopt(ZMQ_RCVTIMEO, a);

  std::stringstream tmp;
  tmp<<"tcp://*:"<<Trigger_port;  
  TriggerCom->bind(tmp.str().c_str());

  //  m_data->Crate= new UC500ADCInterface(m_crate_num);
  //m_data->Crate->Initialise();

  //m_data->Crate->EnableTrigger();

  return true;
}


bool DummyVMETriggerSend::Execute(){

  m_data->triggered=false;
  
  zmq::message_t receive;
  if(TriggerCom->recv(&receive,ZMQ_NOBLOCK)){
    std::istringstream iss(static_cast<char*>(receive.data()));
    
    std::string ret="";
    
    if (iss.str()=="Status"){
      
      usleep(100000);

      //  if(m_data->Crate->Triggered()){
	m_data->triggered=true;
	ret="1";
	//      }
	//else {
	//m_data->triggered=false;
	
	//}
      
      //ret=1;
      //  std::cout << "Waiting for triggers " << std::endl;
      
      
      
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


bool DummyVMETriggerSend::Finalise(){


  delete TriggerCom;
  TriggerCom=0; 


  return true;
}
