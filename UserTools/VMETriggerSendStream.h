#ifndef VMETriggerSendStream_H
#define VMETriggerSendStream_H

#include <string>
#include <iostream>
#include <unistd.h>
#include <unistd.h>

#include "Tool.h"
#include "UC500ADCInterface.h"

class VMETriggerSendStream: public Tool {


 public:

  VMETriggerSendStream();
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
