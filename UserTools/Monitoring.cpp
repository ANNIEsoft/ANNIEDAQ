#include "Monitoring.h"

Monitoring::Monitoring():Tool(){}


bool Monitoring::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  std::string MonitorPort;
  m_variables.Get("MonitorPort",MonitorPort);
  m_variables.Get("MonitorLevel",MonitoringLevel);

  Isend= new zmq::socket_t(*(m_data->context),"ZMQ_PUSH");
  Isend->bind("inproc://MonitorThread");

  monitor_thread_args *args=new monitor_thread_args(MonitorPort,m_data->context);
  pthread_create (&thread, NULL, Monitoring::MoniterThread, args);
  
  return true;

}


bool Monitoring::Execute(){

  if ((tree->GetEntriesFast() % m_monitoringlevel)==0){
  
    for(int card =0 ; card<m_data->LastSync.size();card++){
      LastSync.push_back(m_data->LastSync.at(card));
      SequenceID.push_back(m_data->SequenceID.at(card));
      StartTime.push_back(m_data->StartTime.at(card));
      CardID.push_back(m_data->CardID.at(card));
      channels.push_back(m_data->channels.at(card));
      buffersize.push_back(m_data->buffersize.at(card));
      fullbuffsize.push_back(m_data->fullbuffsize.at(card));

      int*tmppmtid=new int[channels.at(card)];
      
      for (int channel=0;channel<channels.at(card);channel++){
	tmppmtid[cahnnel](m_data->PMTID.at(card)[channel]);
      }
      PMTID.push_back(tmppmtid);
      
      int*tmpdata=new int[fullbuffsize.at(card)];
      
      for (int buffpos=0;buffpos<fullbuffsize.at(card);buffpos++){
	tmpdata[buffpos](m_data->Data.at(card)[buffpos]);
      }
      Data.push_back(tmpdata);

    }

    zmq::message_t message(512);
    std::stringstream send;
    send<<"Data "<<&LastSync<<" "<< &SequenceID<<" "<<&StartTime<<" "<<&CardID<<" "<< &channels <<" "<< &buffersize<<" "<< &fullbuffsize<<" "<< &PMTID &Data;
    
    snprintf ((char *) message.data(), 512 , "%s" ,send.str().c_str()) ;
    Isend->send(message);
  
  }

  return true;
}


bool Monitoring::Finalise(){


  zmq::message_t message(256);
  std::string send="Quit";

  snprintf ((char *) message.data(), 256 , "%s" ,send.c_str()) ;
  Isend->send(message);


 (void) pthread_join(thread, NULL);

 Isend->close();


  return true;
}


void* Monitoring::MoniterThread(void* arg){
  
  monitor_thread_args* args= static_cast<monitor_thread_args*>(arg);

  zmq::socket_t Ireceive (*(args->context), ZMQ_PULL);
  Ireceive.connect("inproc://MonitorThread");
  
  zmq::socket_t Ipub (*(args->context), ZMQ_PUB);
  std::stringstream tmp;
  tmp<<"tcp://*:"<<args->MonitorPort;
  Ipub.bind(tmp.str().c_str());

  bool running=true

    while (running){

     
      zmq::message_t comm;
      Ireceive.recv(&comm);
      
      std::istringstream iss(static_cast<char*>(comm.data()));
      std::string arg1="";

      arg1=iss.str();

      if(arg1==""){

      zmq::message_t mpub(256);
      snprintf ((char *) Esend.data(), 256 , "%s" ,command.c_str()) ;
      Ipub.send(mpub);

      }

      else if(arg1=="Quit")running=false;

    }


  pthread_exit(NULL);
   
}
