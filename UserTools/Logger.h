#ifndef Logger_H
#define Logger_H

#include <string>
#include <iostream>
#include <pthread.h>

#include "Tool.h"


struct logger_args{

  logger_args(){};

  zmq::context_t* context;
  int log_port;

};


class Logger: public Tool {
  
  
 public:
  
  Logger();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  int m_log_port;
  zmq::socket_t *Thread;
  pthread_t thread; 
  logger_args *args;  
  
  static  void* LogThread(void* arg);
    

};


#endif
