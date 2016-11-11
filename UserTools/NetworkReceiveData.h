#ifndef NetworkReceiveData_H
#define NetworkReceiveData_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/progress.hpp>

#include "Tool.h"

#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/iostreams/basic_array_source.hpp>
//#include <boost/iostreams/stream.hpp> 


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

  int m_verbose;

};


#endif
