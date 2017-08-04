#ifndef Monitoring2_H
#define Monitoring2_H

#include <string>
#include <iostream>
#include <sstream>

#include "Tool.h"

struct Plot_thread{
  
  Plot_thread(zmq::context_t* incontext, std::string inoutputpath, std::string intype, int infrequency){
    context=incontext;
    outputpath=inoutputpath;
    type=intype;
    frequency=infrequency;
  }

  TThread *h1;
  zmq::context_t* context;
  std::string outputpath;
  std::string type;
  int frequency;

};
  
struct monitor_thread_args2{
  
  
  monitor_thread_args2( zmq::context_t* incontext, std::string inoutputpath,std::vector<Plot_thread*> * inplots){
    context=incontext;
    outputpath=inoutputpath;
    plots=inplots;
  }
  
  zmq::context_t* context;
  std::string outputpath;
  std::vector<Plot_thread*> *plots;
  
};

class Monitoring2: public Tool {


 public:

  Monitoring2();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  // int MonitoringLevel;

  zmq::socket_t *Isend;
  //  static void *MonitorThread(void* arg);
  static void *PlotThread(void* arg);
  //  pthread_t thread;
  //  TThread *h1;
  //  monitor_thread_args *args;
  std::vector<Plot_thread*> Plots;

};


#endif
