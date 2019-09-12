#ifndef VMETriggerSend_H
#define VMETriggerSend_H

#include <string>
#include <iostream>
#include <unistd.h>
#include <unistd.h>

#include "Tool.h"
#include "UC500ADCInterface.h"

class VMETriggerSend: public Tool {


 public:

  VMETriggerSend();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:


  zmq::socket_t *TriggerCom;
  zmq_pollitem_t in[1];
  zmq_pollitem_t out[1];

  int m_verbose;
  int Trigger_port;
  int m_crate_num;
  bool m_soft;

};


#endif
