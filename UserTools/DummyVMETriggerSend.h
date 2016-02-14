#ifndef DummyVMETriggerSend_H
#define DummyVMETriggerSend_H

#include <string>
#include <iostream>
#include <unistd.h>

#include "Tool.h"
#include "UC500ADCInterface.h"

class DummyVMETriggerSend: public Tool {


 public:

  DummyVMETriggerSend();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:


  zmq::socket_t *TriggerCom;

  int m_verbose;
  int Trigger_port;
  int m_crate_num;



};


#endif
