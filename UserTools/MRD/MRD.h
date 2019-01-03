#ifndef MRD_H
#define MRD_H

#include <string>
#include <iostream>
#include <pthread.h>

#include "TThread.h"
#include "Tool.h"

class ToolChain;
struct mrd_thread_args{

  mrd_thread_args(){};
  mrd_thread_args( zmq::context_t* incontext, std::string inlog_mode, std::string inlog_path, std::string inlog_service, int inlog_port, int inverbose, int inerrorlevel, int inkick_time, std::string intoolsfile, int inremote_port, std::string inmulticastaddress, int inmulticastport){
    context=incontext;
    log_mode=inlog_mode;
    log_path=inlog_path;
    log_service=inlog_service;
    log_port=inlog_port;
    verbose=inverbose;
    errorlevel=inerrorlevel;
    kick_time=inkick_time;
    toolsfile=intoolsfile;
    remote_port=inremote_port;
    multicastaddress=inmulticastaddress;
    multicastport=inmulticastport;
   
  }

  zmq::context_t* context;
  std::string log_mode;
  std::string log_path;
  std::string log_service;
  int log_port;
  int verbose;
  int errorlevel;
  int kick_time;
  std::string toolsfile;
  int remote_port;
  std::string multicastaddress;
  int multicastport;
};


class MRD: public Tool {


 public:

  MRD();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  TThread *h1;
  //pthread_t thread;
  mrd_thread_args *args;

  zmq::socket_t* Isend;

  static void *MRDThread(void* arg);
  
};


#endif
