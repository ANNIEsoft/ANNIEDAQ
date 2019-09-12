#ifndef LAPPDTrigger_H
#define LAPPDTrigger_H

#include <string>
#include <iostream>

#include "Tool.h"

class LAPPDTrigger: public Tool {


 public:

  LAPPDTrigger();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:





};


#endif
