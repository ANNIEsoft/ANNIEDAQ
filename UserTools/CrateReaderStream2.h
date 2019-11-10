#ifndef CrateReaderStream2_H
#define CrateReaderStream2_H

#include <string>
#include <iostream>

#include "HardwareInterface.h"
#include "UC500ADCInterface.h"
#include "Tool.h"
#include "CardData.h"
#include <pthread.h>
#include <sstream>

struct ADC_args2{

  ADC_args2(zmq::context_t* contextin, int crate_numin, bool* triggeredin){
    context=contextin;
    crate_num=crate_numin;
    triggered=triggeredin;
  }

  zmq::context_t* context;
  int crate_num;
  bool* triggered;
  
};           

class CrateReaderStream2: public Tool {


 public:

  CrateReaderStream2();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  //int m_crate_num;
  // int m_num_cards;rface*
  static void* ADCThread(void *arg);

  zmq::socket_t* Isend;
  pthread_t thread;
  ADC_args2 *args;
  bool m_soft;  
  int m_crate_num;

};


#endif
