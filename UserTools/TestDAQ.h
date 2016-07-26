#ifndef TestDAQ_H
#define TestDAQ_H

#include <string>
#include <iostream>

#include "Tool.h"

class TestDAQ: public Tool {


 public:

  TestDAQ();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:


  zmq::socket_t* Test;


};


#endif
