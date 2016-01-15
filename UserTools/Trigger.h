#ifndef Trigger_H
#define Trigger_H

#include <string>
#include <iostream>
#include <unistd.h>

#include "Tool.h"

class Trigger: public Tool {


 public:

  Trigger();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:





};


#endif
