#ifndef ThreadedSubToolChain_H
#define ThreadedSubToolChain_H

#include <string>
#include <iostream>
#include <pthread.h>

#include "Tool.h"

class ToolChain;
struct ToolChain_thread_args{

  ToolChain_thread_args(std::string ToolChainConfigin){
    ToolChainConfig=ToolChainConfigin;  
  }

  std::string ToolChainConfig;

};



class ThreadedSubToolChain: public Tool {


 public:

  ThreadedSubToolChain();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  pthread_t thread;
  //  zmq::socket_t* Isend;
  ToolChain_thread_args *args;

  static void *SubThread(void* arg);


};


#endif
