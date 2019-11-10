#include "CrateReaderStream.h"

CrateReaderStream::CrateReaderStream():Tool(){}


bool CrateReaderStream::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  Isend = new zmq::socket_t(*(m_data->context),ZMQ_PAIR);
  Isend->bind("inproc://ADCThread");

  m_variables.Get("Crate_Num",m_crate_num);

  args=new ADC_args(m_data->context, m_data->Crate, m_data->TriggerCard, m_crate_num,  m_data->SoftTrigger,  &(m_data->triggered));
  pthread_create (&thread, NULL, CrateReaderStream::ADCThread, args);
  

  //m_variables.Get("Crate_Num",m_crate_num);
  
  //std::time_t begin, end;
  //CardData data;
  
  // Crate= new UC500ADCInterface(m_crate_num);
  // Crate->Initialise();
  
  //  m_num_cards=Crate->NumberOfCards();

  //  Crate->EnableTrigger();


  return true;

}


bool CrateReaderStream::Execute(){

  return true;

}
  
 

bool CrateReaderStream::Finalise(){
  
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

void* CrateReaderStream::ADCThread(void* arg){

  ADC_args* args= static_cast<ADC_args*>(arg);

  zmq::socket_t Ireceive (*(args->context), ZMQ_PAIR);
  Ireceive.connect("inproc://ADCThread");

  zmq::socket_t DataSend (*(args->context), ZMQ_PUSH);
  DataSend.bind("inproc://ADCDataSend");

  zmq::socket_t TrigDataSend (*(args->context), ZMQ_PUSH);
  TrigDataSend.bind("inproc://TrigDataSend");

  bool running =true;

  zmq::pollitem_t items[1]={Ireceive,0,ZMQ_POLLIN,0};
  zmq::pollitem_t Dout[1]={DataSend,0,ZMQ_POLLOUT,0};
  zmq::pollitem_t TrigDout[1]={TrigDataSend,0,ZMQ_POLLOUT,0};

  std::vector<CardData>* carddata;
  TriggerData* triggerdata;

  while(running){
    //    usleep(10000);
    //std::cout<<"m1"<<std::endl;
    zmq::poll(&items[0], 1, 0);
    //std::cout<<"m2, triggered= "<<*(args->triggered)<<std::endl;
    //std::cout<<"m2.1, soft= "<<*(args->soft)<<std::endl;
    //std::cout<<"m2.2, cratenum= "<<  args->crate_num<<std::endl;

    
    if (items[0].revents & ZMQ_POLLIN){
      std::cout<<"m2.4"<<std::endl; 
 running =false;
    }
    else if(*(args->triggered)){
      std::cout<<"m2.5"<<std::endl;
      std::cout<<"m2.6 "<<args->Crate<<std::endl;
      std::cout<<"m2.7 "<<args->Crate->Triggered()<<std::endl;

      if(args->Crate->Triggered()){
	std::cout<<"m3"<<std::endl;
	
	*carddata=  args->Crate->GetCardData();
       std::cout<<"m4"<<std::endl;
	
	zmq::poll(&Dout[0], 1, 100);
	std::cout<<"m5"<<std::endl;
	
	if (Dout[0].revents & ZMQ_POLLOUT){
	  std::cout<<"m6"<<std::endl;

	  std::stringstream tmp;
	  tmp<<carddata;
	  
	  zmq::message_t msg(tmp.str().length()+1);
	  snprintf ((char *) msg.data(), tmp.str().length()+1 , "%s" ,tmp.str().c_str());
	  DataSend.send(msg);
	  
	} 
      }
    
      else if(*(args->soft) && args->crate_num==1){
	//	std::cout<<"m7"<<std::endl;

	args->TriggerCard->ForceTriggerNow();
	usleep(100);
      }
      
      else usleep(100);
     
      if(args->crate_num==1){
	if(args->TriggerCard->HasData()){
	  //std::cout<<"m8"<<std::endl;
	  
	  triggerdata=args->TriggerCard->GetTriggerData();
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
