#ifndef InputVariables_H
#define InputVariables_H

#include <string>
#include <iostream>

#include "Tool.h"

class InputVariables: public Tool {


 public:

  InputVariables();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:





};


#endif
