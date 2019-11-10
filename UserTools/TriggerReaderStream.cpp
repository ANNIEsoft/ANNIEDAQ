#include "TriggerReaderStream.h"

TriggerReaderStream::TriggerReaderStream():Tool(){}


bool TriggerReaderStream::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  Isend = new zmq::socket_t(*(m_data->contect),ZMQ_PAIR);
  Isend->bind("inproc://TriggerThread");

  args=new Trigger_args(m_data->context, m_data->TriggerCard);
  pthread_create (&thread, NULL, TriggerReaderStream::TriggerThread, args);
  

  //m_variables.Get("Crate_Num",m_crate_num);
  
  //std::time_t begin, end;
  //CardData data;
  
  // Crate= new UC500ADCInterface(m_crate_num);
  // Crate->Initialise();
  
  //  m_num_cards=Crate->NumberOfCards();

  //  Crate->EnableTrigger();


  return true;

}


bool TriggerReaderStream::Execute(){

  return true;

}
  
 

bool TriggerReaderStream::Finalise(){

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

void* TriggerReaderStream::TriggerThread(void* arg){

  Trigger_args* args= static_cast<monitor_thread_args*>(arg);

  zmq::socket_t Ireceive (*(args->context), ZMQ_PAIR);
  Ireceive.connect("inproc://TriggerThread");

  zmq::socket_t DataSend (*(args->context), ZMQ_PUSH);
  DataSend.connect("inproc://TriggerSend");

  bool running =true;

  zmq::pollitem_t items[1]={Ireceive,0,ZMQ_POLLIN,0};
  zmq::pollitem_t Dout[1]={DataSend,0,ZMQ_POLLOUT,0};
  
  TriggerData* triggerdata;
  
  while(running){
    
    zmq::poll(&items[0], 1, 0);
    
    if (items[0].revents & ZMQ_POLLIN)  running =false;
    
    else if(m_soft){
      
      args->TriggerCard->ForceTriggerNow();
      usleep(100);
    }
    
    else if(args->TriggerCard->HasData()){
      
      *triggerdata=args->TriggerCard->GetTriggerData();

      zmq::poll(&Dout[0], 1, 100);
      
      if (Dout[0].revents & ZMQ_POLLOUT){
	std::stringstream tmp;
	tmp<<triggerdata;

	zmq::message_t	msg(tmp.str().length()+1);
	snprintf ((char *) msg.data(), tmp.str().length()+1 , "%s" ,tmp.str().c_str());
	DataSend.send(msg);

      } 
     
    }

    else usleep(100);
    
    
  }
  
  pthread_exit(NULL);  
  
}
