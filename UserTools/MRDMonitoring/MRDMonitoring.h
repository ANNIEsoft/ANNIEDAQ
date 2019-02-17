#ifndef MRDMonitoring_H
#define MRDMonitoring_H

#include <string>
#include <iostream>

#include "Tool.h"

class MRDMonitoring: public Tool {


 public:

  MRDMonitoring();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  bool cleanup;
  long num;
  long period;

};


#endif
