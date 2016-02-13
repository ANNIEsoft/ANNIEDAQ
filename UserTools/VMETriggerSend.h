#ifndef VMETriggerSend_H
#define VMETriggerSend_H

#include <string>
#include <iostream>
#include <unistd.h>

#include "Tool.h"

class VMETriggerSend: public Tool {


 public:

  VMETriggerSend();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:


  zmq::socket_t *TriggerCom;

  int m_verbose;
  int Trigger_port;




};


#endif
