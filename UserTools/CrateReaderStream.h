#ifndef CrateReaderStream_H
#define CrateReaderStream_H

#include <string>
#include <iostream>

#include "HardwareInterface.h"
#include "UC500ADCInterface.h"
#include "Tool.h"
#include "CardData.h"
#include <pthread.h>
#include <sstream>

struct ADC_args{

  ADC_args(zmq::context_t* contextin, UC500ADCInterface* Cratein,   ANNIETriggerInterface* TriggerCardin, int crate_numin, bool* softin, bool* triggeredin){
    context=contextin;
    Crate=Cratein;
    TriggerCard=TriggerCardin;
    crate_num=crate_numin;
    soft=softin;
    triggered=triggeredin;
  }

  zmq::context_t* context;
  UC500ADCInterface* Crate;
  ANNIETriggerInterface* TriggerCard;
  int crate_num;
  bool* soft;
  bool* triggered;
  

};           

class CrateReaderStream: public Tool {


 public:

  CrateReaderStream();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  //int m_crate_num;
  // int m_num_cards;rface*
  static void* ADCThread(void *arg);

  zmq::socket_t* Isend;
  pthread_t thread;
  ADC_args *args;
  bool m_soft;  
  int m_crate_num;
};


#endif
