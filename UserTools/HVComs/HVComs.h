#ifndef HVComs_H
#define HVComs_H

#include <string>
#include <iostream>

#include "Tool.h"

class HVComs: public Tool {


 public:

  HVComs();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  std::string HVIP;
  int HVPort;
  zmq::socket_t* HV;
  zmq::pollitem_t HVin[1];
  zmq::pollitem_t HVout[1];
  bool VChange;
  int VStart;
  int VStop;
  int VStep;
  int VCurrent;

};


#endif
