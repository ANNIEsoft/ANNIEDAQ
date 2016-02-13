#ifndef NetworkReceiveData_H
#define NetworkReceiveData_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Tool.h"

#include <boost/archive/text_iarchive.hpp>

class NetworkReceiveData: public Tool {


 public:

  NetworkReceiveData();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  zmq::socket_t *Receive;
  int m_port;
  std::string m_address;

  int cards;
  int channels;
  int buffersize;


};


#endif
