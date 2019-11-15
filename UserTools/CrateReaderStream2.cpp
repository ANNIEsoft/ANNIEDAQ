#include "CrateReaderStream2.h"

CrateReaderStream2::CrateReaderStream2():Tool(){}


bool CrateReaderStream2::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  Isend = new zmq::socket_t(*(m_data->context),ZMQ_PAIR);
  Isend->bind("inproc://ADCThread");

  m_variables.Get("Crate_Num",m_crate_num);

  args=new ADC_args2(m_data->context,  m_crate_num, &(m_data->triggered));
  pthread_create (&thread, NULL, CrateReaderStream2::ADCThread, args);
  

  //m_variables.Get("Crate_Num",m_crate_num);
  
  //std::time_t begin, end;
  //CardData data;
  
  // Crate= new UC500ADCInterface(m_crate_num);
  // Crate->Initialise();
  
  //  m_num_cards=Crate->NumberOfCards();

  //  Crate->EnableTrigger();


  return true;

}


bool CrateReaderStream2::Execute(){

  return true;

}
  
 

bool CrateReaderStream2::Finalise(){
  
  std::string send="Quit";
  zmq::message_t message(send.length()+1);

  Isend->send(message); 

  pthread_join(thread, NULL);

  delete Isend;
  Isend=0;  

  delete args;
  args=0;


  return true;
}

void* CrateReaderStream2::ADCThread(void* arg){
  //std::cout<<"int thread"<<std::endl;
  ADC_args2* args= static_cast<ADC_args2*>(arg);

  zmq::socket_t Ireceive (*(args->context), ZMQ_PAIR);
  Ireceive.connect("inproc://ADCThread");

  zmq::socket_t DataSend (*(args->context), ZMQ_PUSH);
  DataSend.bind("inproc://ADCDataSend");

  zmq::socket_t TrigDataSend (*(args->context), ZMQ_PUSH);
  TrigDataSend.bind("inproc://TrigDataSend");

  zmq::socket_t TriggerCom (*(args->context), ZMQ_DEALER);
  TriggerCom.bind("tcp://*:24020");
  //std::cout<<"setup sokets"<<std::endl;

  bool running =true;

  zmq::pollitem_t items[2]={
    {Ireceive,0,ZMQ_POLLIN,0},
    {TriggerCom,0,ZMQ_POLLIN,0}
  };
  zmq::pollitem_t in[1]={TriggerCom,0,ZMQ_POLLIN,0};
  zmq::pollitem_t out[1]={TriggerCom,0,ZMQ_POLLOUT,0};
  zmq::pollitem_t Dout[1]={DataSend,0,ZMQ_POLLOUT,0};
  zmq::pollitem_t TrigDout[1]={TrigDataSend,0,ZMQ_POLLOUT,0};

  //std::cout<<"setup polls"<<std::endl;

  Store variables;
  UC500ADCInterface Crate(args->crate_num);
  ANNIETriggerInterface TriggerCard(args->crate_num);
  bool triggered=false;
  *(args->triggered)=triggered;
  bool soft=false;

  std::vector<CardData>* carddata;
  TriggerData* triggerdata;
  //std::cout<<"setup data objects"<<std::endl;

  while(running){
        usleep(10000);
    //std::cout<<"m1"<<std::endl;
    zmq::poll(&items[0], 2, 0);
    //std::cout<<"m2, triggered= "<<*(args->triggered)<<std::endl;
    //std::cout<<"m2.1, soft= "<<*(args->soft)<<std::endl;
    //std::cout<<"m2.2, cratenum= "<<  args->crate_num<<std::endl;
    if (items[1].revents & ZMQ_POLLIN){
      //std::cout<<"z1"<<std::endl;
      zmq::message_t receive; 
      TriggerCom.recv(&receive);
      std::istringstream iss(static_cast<char*>(receive.data()));
      std::string ret="";
      
      
      if(iss.str()=="Initialise"){
	
	//std::cout<<"got initialise"<<std::endl;
	zmq::poll (&in[0], 1, 5000);
	
	if (in[0].revents & ZMQ_POLLIN) {
	  TriggerCom.recv(&receive);
	  ////std::cout<<"d1"<<std::endl;
	  std::istringstream iss2(static_cast<char*>(receive.data()));
	  //std::cout<<"d2"<<std::endl;
	  variables.JsonParser(iss2.str());
	  
	  //std::cout<<"d6"<<std::endl;
	  Crate.PresetCounters();
	  Crate.Initialise(variables);
	  //std::cout<<"d6.5"<<std::endl;
	  if(args->crate_num==1) {
	    //std::cout<<"d6.6"<<std::endl;
	    TriggerCard.Initialise(variables);	
	    //std::cout<<"d7"<<std::endl;
	    TriggerCard.EnableTrigger();
	  }
	  //std::cout<<"d8"<<std::endl;
	  ret="Initialised";
	  //std::cout<<"d9"<<std::endl;
	  variables.Get("Soft_Trigger",soft);
	  triggered=true;
	  *(args->triggered)=triggered;
	  
	  
	  //variables.Print();
	  
	}
      }
      
      else if(iss.str()=="Finalise"){
	triggered=false;
	*(args->triggered)=triggered;
	ret="Finalised";
      }
      
      else ret="ERROR unknown message";
      //std::cout<<"polling out"<<std::endl;
      zmq::poll (&out[0], 1, 5000);
      //std::cout<<"end poll"<<std::endl;
      if (out[0].revents & ZMQ_POLLOUT) {
	//std::cout<<"in send"<<std::endl;
	zmq::message_t message(ret.length()+1);
	snprintf ((char *) message.data(), ret.length()+1 , "%s" ,ret.c_str() ) ;
	TriggerCom.send(message);
	//std::cout<<"sent "<<ret<<std::endl;
      }
    }

 
    else if (items[0].revents & ZMQ_POLLIN){
      //std::cout<<"m2.4"<<std::endl; 
      running =false;
    }
    else if(triggered){
      //std::cout<<"m2.5"<<std::endl;
      
      if(Crate.Triggered()){
	// std::cout<<"m3"<<std::endl;
	
	carddata=new std::vector<CardData>;
	*(carddata)=Crate.GetCardData();

	//	for(int k=0;k<carddata->size();k++){

	//if(carddata->at(k).Data.size()==0) std::cout<<"wegot one"<<std::endl;
	//	}
	//std::cout<<"m4"<<std::endl;
	
	zmq::poll(&Dout[0], 1, 100);
	//std::cout<<"m5"<<std::endl;
	
	if (Dout[0].revents & ZMQ_POLLOUT){
	  //std::cout<<"m6"<<std::endl;
	  
	  std::stringstream tmp;
	  tmp<<carddata;
	  
	  zmq::message_t msg(tmp.str().length()+1);
	  snprintf ((char *) msg.data(), tmp.str().length()+1 , "%s" ,tmp.str().c_str());
	  DataSend.send(msg);
	  
	} 
      }
      
      else if(soft && args->crate_num==1){
		//std::cout<<"m7"<<std::endl;
	
	TriggerCard.ForceTriggerNow();
	usleep(100);
      }
      
      else usleep(100);
      
      if(args->crate_num==1){
	//std::cout<<"z2"<<std::endl;
	if(TriggerCard.HasData()){
	  //std::cout<<"m8"<<std::endl;
	  
	  triggerdata=TriggerCard.GetTriggerData();
	  zmq::poll(&TrigDout[0], 1, 100);
	  
	  if (TrigDout[0].revents & ZMQ_POLLOUT){
	    std::stringstream tmp;
	    tmp<<triggerdata;
	    
	    zmq::message_t  msg(tmp.str().length()+1);
	    snprintf ((char *) msg.data(), tmp.str().length()+1 , "%s" ,tmp.str().c_str());
	    TrigDataSend.send(msg);
	    
	  }
	} 
      }
    }
  }
  
  pthread_exit(NULL); 
  
}
