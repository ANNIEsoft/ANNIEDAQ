#ifndef Monitoring_H
#define Monitoring_H

#include <string>
#include <iostream>

#include "Tool.h"

class Monitoring: public Tool {


 public:

  Monitoring();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:





};


#endif
