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



};


#endif
