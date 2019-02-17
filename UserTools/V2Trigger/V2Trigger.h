#ifndef V2Trigger_H
#define V2Trigger_H

#include <string>
#include <iostream>

#include "Tool.h"

class V2Trigger: public Tool {


 public:

  V2Trigger();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  int numTriggers;
  std::map<std::string,zmq::socket_t*> Sockets;
  int m_verbose;
  std::map<std::string,int> TriggerResiduals;

  void FindTriggerSources();

};


#endif
