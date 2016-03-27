#ifndef Monitoring_H
#define Monitoring_H

#include <string>
#include <iostream>
#include <sstream>

#include "Tool.h"

#include "TH1I.h"
#include "TH2I.h"
#include "TCanvas.h"

struct monitor_thread_args{


  monitor_thread_args( zmq::context_t* incontext, std::string inoutputpath){
    context=incontext;
    ouputpath=inoutptupath
  }
  
  zmq::context_t* context;
  std::string outputpath;
};



class Monitoring: public Tool {


 public:

  Monitoring();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  int MonitoringLevel;

  zmq::socket_t *Isend;
  static void *MonitorThread(void* arg);
  pthread_t thread;


};


#endif
