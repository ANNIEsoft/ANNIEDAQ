#ifndef Trigger_H
#define Trigger_H

#include <string>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <boost/progress.hpp>

#include "Tool.h"

class Trigger: public Tool {


 public:

  Trigger();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  int m_verbose;
  std::string VME_service_name;
  int numVME;
  int VME_port;

  std::vector<zmq::socket_t*> VMESockets;


};


#endif
