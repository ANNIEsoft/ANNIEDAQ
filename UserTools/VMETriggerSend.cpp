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
  m_data->CrateNum=m_crate_num;

  m_soft=false;

  TriggerCom = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
  //  int a=12000;
  //TriggerCom->setsockopt(ZMQ_SNDTIMEO, a);
  //TriggerCom->setsockopt(ZMQ_RCVTIMEO, a);

  std::stringstream tmp;
  tmp<<"tcp://*:"<<Trigger_port;  
  TriggerCom->bind(tmp.str().c_str());

  in[0].socket= *TriggerCom;
  in[0].events=ZMQ_POLLIN;
  out[0].socket= *TriggerCom;
  out[0].events=ZMQ_POLLOUT;


  m_data->Crate= new UC500ADCInterface(m_crate_num);
  m_data->Crate->Initialise(m_variables);
  if(m_crate_num==1){
    m_data->TriggerCard= new ANNIETriggerInterface(m_crate_num);
    m_data->TriggerCard->Initialise(m_variables);
    m_data->TriggerCard->EnableTrigger();
  }

  return true;
}


bool VMETriggerSend::Execute(){

  m_data->triggered=false;
  
  /*  zmq::pollitem_t in[] = {
    { *TriggerCom, 0, ZMQ_POLLIN, 0 }
  };

  zmq::pollitem_t out[] = {
    { *TriggerCom, 0, ZMQ_POLLOUT, 0 }
  };
  */

  zmq::message_t receive;
  if(TriggerCom->recv(&receive,ZMQ_NOBLOCK)){
    std::istringstream iss(static_cast<char*>(receive.data()));
    
    std::string ret="";
    
    if (iss.str()=="Status"){
      //std::cout<<"got status"<<std::endl;      
      
      if(m_data->Crate->Triggered()){
	//	std::cout<<"v1"<<std::endl;
	m_data->triggered=true;
	ret="1";
      }
      else {
	//	std::cout<<"v2 "<<m_soft<<std::endl;
	m_data->triggered=false;
	if(m_soft && m_crate_num==1 ){
	  //std::cout<<"v3"<<std::endl;
	  while(!m_data->Crate->Triggered()){
	    // std::cout<<"v4"<<std::endl;  
	     m_data->TriggerCard->ForceTriggerNow();
	    usleep(1000);
	  }
	  ret="1";
	  m_data->triggered=true;	
	  
	}
	
	//usleep(10000);
      }

      //////////////////// BEN faking trigger remove///////
      ret="1";            
      m_data->triggered=true;
      usleep(1000);  
      //////////////////////////////////////////////////

      //ret=1;
      //  std::cout << "Waiting for triggers " << std::endl;
      
      
      
    }

    else if(iss.str()=="Initialise"){

      //std::cout<<"got initialise"<<std::endl;
      zmq::poll (&in[0], 1, 5000);
      
      if (in[0].revents & ZMQ_POLLIN) {
	TriggerCom->recv(&receive);
	//std::cout<<"d1"<<std::endl;
	std::istringstream iss2(static_cast<char*>(receive.data()));
	//std::cout<<"d2"<<std::endl;
	m_variables.JsonParser(iss2.str());
	//std::cout<<"d3"<<std::endl;
	//m_data->Crate->Finalise();
	//std::cout<<"d4"<<std::endl;
	//  delete m_data->Crate;
	// sleep(2);
	//std::cout<<"d5"<<std::endl;
	// m_data->Crate= new UC500ADCInterface(m_crate_num);
	 //std::cout<<"d6"<<std::endl;
	m_data->Crate->Initialise(m_variables);
	//std::cout<<"d6.5"<<std::endl;
	if(m_crate_num==1) {
	  //std::cout<<"d6.6"<<std::endl;
	  m_data->TriggerCard->Initialise(m_variables);	
	  //std::cout<<"d7"<<std::endl;
	  m_data->TriggerCard->EnableTrigger();
	}
	//std::cout<<"d8"<<std::endl;
	ret="Initialised";
	//std::cout<<"d9"<<std::endl;
	m_variables.Get("Soft_Trigger",m_soft);
	if(m_soft &&  m_crate_num==1){
	  while(!m_data->Crate->Triggered()){
	    //std::cout<<"p1 "<<m_data->Crate->Triggered()<<std::endl;
	    m_data->TriggerCard->ForceTriggerNow();
	    usleep(1000);
          }
	  //std::cout<<"p2"<<std::endl;
	}
	//std::cout<<"p3"<<std::endl;
      }
      //std::cout<<"p4"<<std::endl;
    }
    
    else ret="ERROR unknown message";
    //std::cout<<"polling out"<<std::endl;
    zmq::poll (&out[0], 1, 5000);
    //std::cout<<"end poll"<<std::endl;
    if (out[0].revents & ZMQ_POLLOUT) {
      //std::cout<<"in send"<<std::endl;
      zmq::message_t message(ret.length()+1);
      snprintf ((char *) message.data(), ret.length()+1 , "%s" ,ret.c_str() ) ;
      TriggerCom->send(message);
      //std::cout<<"sent "<<ret<<std::endl;
    }
    
    
  }
  
  return true;
}


bool VMETriggerSend::Finalise(){

  //if(m_crate_num==1) m_data->TriggerCard->DisableADCTriggers();
  delete  m_data->TriggerCard;
  m_data->TriggerCard=0;

  delete  m_data->Crate;
  m_data->Crate=0;

  delete TriggerCom;
  TriggerCom=0; 
    
  return true;
}
