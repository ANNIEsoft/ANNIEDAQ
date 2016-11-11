#ifndef SlackBot_H
#define SlackBot_H

#include <string>
#include <iostream>
#include <stdlib.h>
#include <curl/curl.h>

#include "Tool.h"

class SlackBot: public Tool {


 public:

  SlackBot();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:





};


#endif
