#ifndef NetworkReceiveDataStream_H
#define NetworkReceiveDataStream_H

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


class NetworkReceiveDataStream: public Tool {


 public:

  NetworkReceiveDataStream();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  zmq::socket_t *Receive;
  zmq::socket_t *TrigReceive;
  int m_port;
  int m_trigport;
  std::string m_address1;
  std::string m_address2;
  std::string m_address3;
  std::string m_trigaddress; 

  zmq::pollitem_t in[1];
  zmq::pollitem_t in2[1];

  int cards;
  int channels;
  int buffersize;

  int m_verbose;
  long trign;
  long datan;
};


#endif
