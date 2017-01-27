#include "VMETriggerSend.h"

VMETriggerSend::VMETriggerSend():Tool(){}


bool VMETriggerSend::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_data->triggered=false;

  m_variables.Get("Trigger_port",Trigger_port);
  m_variables.Get("verbose",m_verbose);
  m_variables.Get("Crate_Num",m_crate_num);
  m_soft=false;

  TriggerCom = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
  //  int a=12000;
  //TriggerCom->setsockopt(ZMQ_SNDTIMEO, a);
  //TriggerCom->setsockopt(ZMQ_RCVTIMEO, a);

  std::stringstream tmp;
  tmp<<"tcp://*:"<<Trigger_port;  
  TriggerCom->bind(tmp.str().c_str());

  m_data->Crate= new UC500ADCInterface(m_crate_num);
  m_data->Crate->Initialise(m_variables);

  m_data->Crate->EnableTrigger();


  return true;
}


bool VMETriggerSend::Execute(){

  m_data->triggered=false;
  
  zmq::pollitem_t in[] = {
    { *TriggerCom, 0, ZMQ_POLLIN, 0 }
  };

  zmq::pollitem_t out[] = {
    { *TriggerCom, 0, ZMQ_POLLOUT, 0 }
  };


  zmq::message_t receive;
  if(TriggerCom->recv(&receive,ZMQ_NOBLOCK)){
    std::istringstream iss(static_cast<char*>(receive.data()));
    
    std::string ret="";
    
    if (iss.str()=="Status"){
      
      
      if(m_data->Crate->Triggered()){
	m_data->triggered=true;
	ret="1";
      }
      else {
	m_data->triggered=false;
	if(m_soft){
	  while(!m_data->Crate->Triggered()){  
	  m_data->Crate->ForceTriggerNow();
	  usleep(1000);
	  }
	  ret="1";
	  m_data->triggered=true;	
	
	}
	
	//usleep(10000);
      }
      
      //ret=1;
      //  std::cout << "Waiting for triggers " << std::endl;
      
      
      
    }
    else if(iss.str()=="Initialise"){
      //  std::cout<<"got initialise"<<std::endl;
      zmq::poll (&in[0], 1, 5000);
      
      if (in[0].revents & ZMQ_POLLIN) {
	TriggerCom->recv(&receive);
	//std::cout<<"d1"<<std::endl;
	std::istringstream iss2(static_cast<char*>(receive.data()));
	//std::cout<<"d2"<<std::endl;
	m_variables.JsonPaser(iss2.str());
	//std::cout<<"d3"<<std::endl;
	//m_data->Crate->Finalise();
	//std::cout<<"d4"<<std::endl;
	//  delete m_data->Crate;
	// sleep(2);
	//std::cout<<"d5"<<std::endl;
	// m_data->Crate= new UC500ADCInterface(m_crate_num);
	// std::cout<<"d6"<<std::endl;
	m_data->Crate->Initialise(m_variables);
	//std::cout<<"d7"<<std::endl;
	m_data->Crate->EnableTrigger();
	//std::cout<<"d8"<<std::endl;
	ret="Initialised";
	//std::cout<<"d9"<<std::endl;
	m_variables.Get("Soft_Trigger",m_soft);
	if(m_soft){
	  while(!m_data->Crate->Triggered()){
	    m_data->Crate->ForceTriggerNow();
	    usleep(1000);
          }
	}
      }
    }
    
    
    else ret="ERROR unknown message";
    
    zmq::poll (&out[0], 1, 5000);
    
    if (out[0].revents & ZMQ_POLLOUT) {
      
      zmq::message_t message(ret.length()+1);
      snprintf ((char *) message.data(), ret.length()+1 , "%s" ,ret.c_str() ) ;
      TriggerCom->send(message);
    }
    
    
  }
  
  return true;
}


bool VMETriggerSend::Finalise(){


  delete TriggerCom;
  TriggerCom=0; 
    
  return true;
}
