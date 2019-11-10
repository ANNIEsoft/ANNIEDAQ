#ifndef TriggerReaderStream_H
#define TriggerReaderStream_H

#include <string>
#include <iostream>

#include "HardwareInterface.h"
#include "UC500ADCInterface.h"
#include "Tool.h"
#include "CardData.h"
#include <pthread.h>
#include <sstream>

struct Trigger_args{
  Trigger_args(zmq::context_t* contextin,  ANNIETriggerInterface* TriggerCardin){
    context=contextin;
    TriggerCard=TriggerCardin;
  }
  zmq::context_t* context;
  ANNIETriggerInterface* TriggerCard;

};           

class TriggerReaderStream: public Tool {


 public:

  TriggerReaderStream();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  //int m_crate_num;
  // int m_num_cards;
  static void* TriggerThread(void *arg);

  zmq::socket_t* Isend;
  pthread_t thread;
  Trigger_args *args;
  
};


#endif
