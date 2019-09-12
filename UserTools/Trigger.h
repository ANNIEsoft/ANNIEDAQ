#ifndef Trigger_H
#define Trigger_H

#include <string>
#include <iostream>

#include "Tool.h"
#include <zmq.hpp>

class Trigger: public Tool {


 public:

  Trigger();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:


  zmq::socket_t* sock;
  zmq_pollitem_t in[1];
  zmq_pollitem_t out[1]; 

  int m_verbose;
  int Trigger_port;
  
};


#endif
