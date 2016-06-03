#ifndef DAQInit_H
#define DAQInit_H

#include <string>
#include <iostream>

#include "Tool.h"

class DAQInit: public Tool {


 public:

  DAQInit();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:





};


#endif
